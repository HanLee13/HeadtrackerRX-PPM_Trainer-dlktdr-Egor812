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
  createChannelsDataMutex();

#ifdef DEBUG
  printf("Starting app_main...\n");
#endif

  TaskHandle_t ppmTaskHandle = NULL; // PPM Task Handle
  BaseType_t ppmTaskResult = xTaskCreate(ppmTask, "PPM_Task", 4096, NULL, configMAX_PRIORITIES - 1, &ppmTaskHandle); // High priority
  if (ppmTaskResult != pdPASS) {
#ifdef DEBUG
    printf("Failed to create PPM task\n");
#endif
    return;
  }
  configASSERT(ppmTaskHandle);

  TaskHandle_t tUartHnd = NULL;
  BaseType_t uartTaskResult = xTaskCreate(runUARTHead, "UART", 8192, NULL, configMAX_PRIORITIES - 2, &tUartHnd);
  if (uartTaskResult != pdPASS) {
#ifdef DEBUG
    printf("Failed to create UART task\n");
#endif
    return;
  }
  configASSERT(tUartHnd);

#if defined(LEDPIN)
  TaskHandle_t tBlinkHnd = NULL;
  BaseType_t blinkyTaskResult = xTaskCreate(runBlinky, "Blinky", 2048, NULL, tskIDLE_PRIORITY, &tBlinkHnd);
  if (blinkyTaskResult != pdPASS) {
#ifdef DEBUG
    printf("Failed to create Blinky task\n");
#endif
    // Don't return here as LED is optional
  } else {
    configASSERT(tBlinkHnd);
  }
#endif

  esp_err_t ret;

  // Initialize NVS. 
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(nvs_open("btwifi", NVS_READWRITE, &nvs_flsh_btw));

    loadSettings();

  // Initialize Bluetooth
  bt_init();

  // Initialize Bluetooth client or server based on settings
  if (settings.role == ROLE_BLE_CENTRAL) {
    btcInit();
  } else if (settings.role == ROLE_BLE_PERIPHERAL) {
    btpInit();
  }
}
