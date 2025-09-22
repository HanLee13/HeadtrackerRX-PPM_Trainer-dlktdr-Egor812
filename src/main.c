/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
 *
 * This demo showcases creating a GATT database using a predefined attribute
 *table. It acts as a GATT server and can send adv data, be connected by client.
 * Run the gatt_client demo, the client demo will automatically connect to the
 *gatt_server_service_table demo. Client demo will enable GATT server's notify
 *after connection. The two devices will then exchange data.
 *
 ****************************************************************************/

#include "bt.h"
#include "ppm.h"
#include "frskybt.h"
#include "defines.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "settings.h"
#include "terminal.h"
#include "bt_client.h"
#include "bt_server.h"

nvs_handle_t nvs_flsh_btw;

#if defined(LEDPIN)
void runBlinky()
{
  // Initialize LED pin
  esp_err_t err = gpio_set_direction(LEDPIN, GPIO_MODE_DEF_OUTPUT);
  if (err != ESP_OK) {
    // If GPIO initialization fails, just return instead of causing a reboot
    return;
  }

  // Turn LED off initially
  gpio_set_level(LEDPIN, 0);

  for (;;) {
    // Check if either client or server is connected
    // Use local variables to avoid potential race conditions
    bool is_connected = false;

    // Safely check connection status
    is_connected = btc_connected || btp_connected;

    if (is_connected) {
      // Bluetooth connected - LED on constantly
      gpio_set_level(LEDPIN, 1);
      vTaskDelay(pdMS_TO_TICKS(1000)); // Check connection status every second
    } else {
      // Bluetooth not connected - blink every 1 second
      gpio_set_level(LEDPIN, 1);
      vTaskDelay(pdMS_TO_TICKS(1000));
      gpio_set_level(LEDPIN, 0);
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
  }
}
#endif

void app_main(void)
{
  // Initialize UART for debugging first
  const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_TXPIN, UART_RXPIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, UART_RX_BUFFER, UART_TX_BUFFER, 0, NULL, 0);

  printf("Starting app_main...\n");
  printf("Debug: createChannelsDataMutex...\n");
  createChannelsDataMutex();
  printf("Debug: Channels mutex created\n");

  printf("Debug: Creating PPM task...\n");
  TaskHandle_t ppmTaskHandle = NULL; // PPM Task Handle
  BaseType_t ppmTaskResult = xTaskCreate(ppmTask, "PPM_Task", 4096, NULL, configMAX_PRIORITIES - 1, &ppmTaskHandle); // High priority
  if (ppmTaskResult != pdPASS) {
    printf("Failed to create PPM task: %d\n", ppmTaskResult);
    return;
  }
  configASSERT(ppmTaskHandle);
  printf("Debug: PPM task created\n");

  printf("Debug: Creating UART task...\n");
  TaskHandle_t tUartHnd = NULL;
  BaseType_t uartTaskResult = xTaskCreate(runUARTHead, "UART", 8192, NULL, configMAX_PRIORITIES - 2, &tUartHnd);
  if (uartTaskResult != pdPASS) {
    printf("Failed to create UART task: %d\n", uartTaskResult);
    return;
  }
  configASSERT(tUartHnd);
  printf("Debug: UART task created\n");

#if defined(LEDPIN)
  printf("Debug: Creating LED task...\n");
  TaskHandle_t tBlinkHnd = NULL;
  BaseType_t blinkyTaskResult = xTaskCreate(runBlinky, "Blinky", 2048, NULL, tskIDLE_PRIORITY, &tBlinkHnd);
  if (blinkyTaskResult != pdPASS) {
    printf("Failed to create Blinky task: %d\n", blinkyTaskResult);
    // Don't return here as LED is optional
  } else {
    configASSERT(tBlinkHnd);
    printf("Debug: LED task created\n");
  }
#endif

  printf("Debug: Initializing NVS...\n");
  esp_err_t ret;

  // Initialize NVS.
  printf("Debug: Initializing NVS...\n");
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND || ret == ESP_ERR_NVS_NOT_INITIALIZED) {
    printf("Debug: Erasing NVS and reinitializing due to error: %d\n", ret);
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  if (ret != ESP_OK) {
    printf("Debug: NVS initialization failed with error: %d. Attempting full erase.\n", ret);
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
    if (ret != ESP_OK) {
      printf("Debug: NVS initialization failed again with error: %d\n", ret);
      // Continue anyway, but settings may not be available
  }
  } else {
    printf("Debug: NVS initialized successfully\n");
  }

  printf("Debug: Opening NVS...\n");
  ret = nvs_open("btwifi", NVS_READWRITE, &nvs_flsh_btw);
  if (ret != ESP_OK) {
    printf("Debug: Failed to open NVS namespace 'btwifi', error: %d\n", ret);
    // Try to create/recover
    ret = nvs_open("btwifi", NVS_READWRITE, &nvs_flsh_btw);
    if (ret != ESP_OK) {
      printf("Debug: Failed to open NVS namespace again, error: %d. Continuing without NVS.\n", ret);
      // We'll continue without NVS, settings will use defaults
    } else {
      printf("Debug: NVS opened successfully on second attempt\n");
    }
  } else {
    printf("Debug: NVS opened successfully\n");
  }

  printf("Debug: Loading settings...\n");
  // Only load settings if NVS is available
  if (nvs_flsh_btw != 0) {
    loadSettings();
    printf("Debug: Settings loaded\n");
  } else {
    printf("Debug: NVS not available, using default settings\n");
    // Initialize default settings
    settings.role = ROLE_BLE_CENTRAL; // Default to client mode
    // Initialize other default settings as needed
  }

  // Initialize Bluetooth
  printf("Debug: Initializing Bluetooth...\n");
  bt_init();
  printf("Debug: Bluetooth initialized\n");

  // Initialize Bluetooth client or server based on settings
  printf("Debug: Initializing Bluetooth role: %d\n", settings.role);
  if (settings.role == ROLE_BLE_CENTRAL) {
    printf("Debug: Initializing Bluetooth client...\n");
    btcInit();
    printf("Debug: Bluetooth client initialized\n");
  } else if (settings.role == ROLE_BLE_PERIPHERAL) {
    printf("Debug: Initializing Bluetooth server...\n");
    btpInit();
    printf("Debug: Bluetooth server initialized\n");
  } else {
    printf("Debug: Unknown Bluetooth role, skipping Bluetooth initialization\n");
  }
  printf("Debug: App initialization complete\n");
}
