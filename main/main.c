#include <stdio.h>
#include <inttypes.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "TIMER"

#define TIMER_FREQUENCY 80000000
#define DIVIDER 1024
#define ALARM_VALUE (TIMER_FREQUENCY / DIVIDER) // every second

// Timer registers
#define TIM_G0_T0_CONFIG_REGISTER 0x3FF5F000
#define TIM_G0_T0_UPDATE_REGISTER 0x3FF5F00C  // set this register for copying counter value to hi/lo registers
#define TIM_G0_T0_HI_REGISTER 0x3FF5F008      // hi bits of the counter
#define TIM_G0_T0_LO_REGISTER 0x3FF5F004      // low bits of the counter
#define TIM_G0_T0_LOAD_HI_REGISTER 0x3FF5F014 // timer counter value low bits initial value for the counter
#define TIM_G0_T0_LOAD_LO_REGISTER 0x3FF5F018 // timer counter value low bits  initial value for the counter
#define TIM_G0_T0_LOAD_REGISTER 0x3FF5F020    // any value can be written here to reload the timer counter from TIM_G0_T0_LOAD_HI_REGISTER TIM_G0_T0_LOAD_LO_REGISTER

// Config values
#define ALARM_ENABLE (0 << 10)
#define LEVEL_INTERRUPT_ENABLE (0 << 11)
#define EDGE_INTERRUPT_ENABLE (0 << 12)
#define DIVIDER_CONFIG_VALUE (DIVIDER << 13) // Divide clock frequency
#define AUTORELOAD_ENABLE (1 << 29)
#define INCREASE_ENABLE (1 << 30) // 1 for increment, 0 for decrement
#define TIMER_ENABLE (1 << 31)

void check_timer_counter(void *args)
{
  volatile uint32_t *update_register = (volatile uint32_t *)TIM_G0_T0_UPDATE_REGISTER;
  volatile uint32_t *hi_register = (volatile uint32_t *)TIM_G0_T0_HI_REGISTER;
  volatile uint32_t *low_register = (volatile uint32_t *)TIM_G0_T0_LO_REGISTER;

  volatile uint32_t *counter_reload = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;
  volatile uint32_t *counter_reload_hi = (volatile uint32_t *)TIM_G0_T0_LOAD_HI_REGISTER;
  volatile uint32_t *counter_reload_lo = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;

  *(counter_reload_hi) = (uint32_t)0;
  *(counter_reload_lo) = (uint32_t)0;

  uint8_t seconds = 0;

  while (1)
  {
    *(update_register) = 1; // any value can be written here to trigger copying timer's counter
    uint64_t counter_value = *(hi_register) | *(low_register);
    if (counter_value > ALARM_VALUE)
    {
      ESP_LOGI(TAG, "seconds passed: %d", ++seconds);
      *(counter_reload) = 1;
    }
  }
}

void app_main(void)
{
  volatile uint32_t *config_register = (volatile uint32_t *)TIM_G0_T0_CONFIG_REGISTER;

  uint32_t config_value = (uint32_t)(TIMER_ENABLE | INCREASE_ENABLE | AUTORELOAD_ENABLE |
                                     DIVIDER_CONFIG_VALUE | EDGE_INTERRUPT_ENABLE | LEVEL_INTERRUPT_ENABLE | ALARM_ENABLE);

  ESP_LOGI(TAG, "starting timer...");
  *(config_register) = config_value; // start timer

  xTaskCreate(&check_timer_counter, "check_timer_counter", 2048, NULL, tskIDLE_PRIORITY, NULL);
}
