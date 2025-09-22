#include "defines.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

void app_main(void)
{
  // Initialize UART for debugging
  const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(UART_NUM_0, &uart_config);
  uart_set_pin(UART_NUM_0, UART_TXPIN, UART_RXPIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0);

  printf("Minimal system started\n");

  // Test LED if defined
#if defined(LEDPIN)
  printf("Testing LED on GPIO%d\n", LEDPIN);
  gpio_set_direction(LEDPIN, GPIO_MODE_DEF_OUTPUT);
  gpio_set_level(LEDPIN, 1);
  vTaskDelay(pdMS_TO_TICKS(1000));
  gpio_set_level(LEDPIN, 0);
  printf("LED test completed\n");
#endif

  printf("System running...\n");

  // Simple blink task
  while(1) {
#if defined(LEDPIN)
    gpio_set_level(LEDPIN, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LEDPIN, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
#else
    printf("Heartbeat...\n");
    vTaskDelay(pdMS_TO_TICKS(1000));
#endif
  }
}