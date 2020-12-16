// #include <stdio.h>
// #include <inttypes.h>

// #include "esp_log.h"
// #include "esp_intr_alloc.h"

// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/timer.h"

// #define TAG "TIMER"

// #define TIMER_FREQUENCY 80000000
// #define DIVIDER 1024
// #define ALARM_VALUE (TIMER_FREQUENCY / DIVIDER) // every second

// // Timer registers
// #define TIM_G0_T0_CONFIG_REGISTER 0x3FF5F000
// #define TIM_G0_T0_UPDATE_REGISTER 0x3FF5F00C  // set this register for copying counter value to hi/lo registers
// #define TIM_G0_T0_HI_REGISTER 0x3FF5F008      // hi bits of the counter
// #define TIM_G0_T0_LO_REGISTER 0x3FF5F004      // low bits of the counter
// #define TIM_G0_T0_LOAD_HI_REGISTER 0x3FF5F014 // timer counter value low bits initial value for the counter
// #define TIM_G0_T0_LOAD_LO_REGISTER 0x3FF5F018 // timer counter value low bits  initial value for the counter
// #define TIM_G0_T0_LOAD_REGISTER 0x3FF5F020    // any value can be written here to reload the timer counter from TIM_G0_T0_LOAD_HI_REGISTER TIM_G0_T0_LOAD_LO_REGISTER

// #define TIM_G0_T0_ALARM_HI_REGISTER 0x3FF5F014 // alarm value high bits
// #define TIM_G0_T0_ALARM_LO_REGISTER 0x3FF5F010 // alarm value low bits

// // Config values
// #define ALARM_ENABLE (1 << 10)
// #define LEVEL_INTERRUPT_ENABLE (1 << 11)
// #define EDGE_INTERRUPT_ENABLE (0 << 12)
// #define DIVIDER_CONFIG_VALUE (DIVIDER << 13) // Divide clock frequency
// #define AUTORELOAD_ENABLE (1 << 29)
// #define INCREASE_ENABLE (1 << 30) // 1 for increment, 0 for decrement
// #define TIMER_ENABLE (1 << 31)

// portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

// void alarm_handler(void *args)
// {
//   printf("called\n");

//   // portENTER_CRITICAL_ISR(&timerMux);

//   // portEXIT_CRITICAL_ISR(&timerMux);
// }

// void start_timer(void *args)
// {
//   // setting up alarm trigger value
//   volatile uint32_t *alarm_hi_register = (volatile uint32_t *)TIM_G0_T0_ALARM_HI_REGISTER;
//   volatile uint32_t *alarm_lo_register = (volatile uint32_t *)TIM_G0_T0_ALARM_LO_REGISTER;

//   *(alarm_hi_register) = (uint32_t)0;
//   *(alarm_lo_register) = (uint32_t)ALARM_VALUE;

//   // setting up counter reload initial value
//   volatile uint32_t *counter_reload_hi_register = (volatile uint32_t *)TIM_G0_T0_LOAD_HI_REGISTER;
//   volatile uint32_t *counter_reload_lo_register = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;

//   *(counter_reload_hi_register) = (uint32_t)0;
//   *(counter_reload_lo_register) = (uint32_t)0;

//   volatile uint32_t *config_register = (volatile uint32_t *)TIM_G0_T0_CONFIG_REGISTER;

//   // enable interrupt
//   *(config_register) |= LEVEL_INTERRUPT_ENABLE;

//   // register interrupt handler
//   esp_intr_alloc(ETS_TG0_T0_LEVEL_INTR_SOURCE, 0, alarm_handler, NULL, NULL);

//   // start timer
//   uint32_t config_value = (uint32_t)(TIMER_ENABLE | INCREASE_ENABLE | AUTORELOAD_ENABLE |
//                                      DIVIDER_CONFIG_VALUE | EDGE_INTERRUPT_ENABLE | LEVEL_INTERRUPT_ENABLE | ALARM_ENABLE);

//   *(config_register) = config_value;

//   while (1)
//   {
//     // task should not return
//   }

//   // volatile uint32_t *counter_update_register = (volatile uint32_t *)TIM_G0_T0_UPDATE_REGISTER;
//   // volatile uint32_t *counter_hi_register = (volatile uint32_t *)TIM_G0_T0_HI_REGISTER;
//   // volatile uint32_t *counter_low_register = (volatile uint32_t *)TIM_G0_T0_LO_REGISTER;
//   // // volatile uint32_t *counter_reload_register = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;

//   // // uint8_t seconds = 0;

//   // while (1)
//   // {
//   //   *(counter_update_register) = 1; // any value can be written here to trigger copying timer's counter
//   //   uint64_t counter_value = *(counter_hi_register) | *(counter_low_register);
//   //   printf("counter-value: %llu\n", counter_value);
//   //   // if (counter_value > ALARM_VALUE)
//   //   // {
//   //   //   ESP_LOGI(TAG, "seconds passed: %d", ++seconds);
//   //   //   *(counter_reload) = 1;
//   //   // }
//   // }
// }

// void app_main(void)
// {
//   ESP_LOGI(TAG, "starting timer...");
//   xTaskCreate(&start_timer, "start timer", 2048, NULL, tskIDLE_PRIORITY, NULL);

//   // volatile uint32_t *alarm_hi_register = (volatile uint32_t *)TIM_G0_T0_ALARM_HI_REGISTER;
//   // volatile uint32_t *alarm_lo_register = (volatile uint32_t *)TIM_G0_T0_ALARM_LO_REGISTER;

//   // *(alarm_hi_register) = (uint32_t)0;
//   // *(alarm_lo_register) = (uint32_t)ALARM_VALUE;

//   // // setting up counter reload initial value
//   // volatile uint32_t *counter_reload_hi_register = (volatile uint32_t *)TIM_G0_T0_LOAD_HI_REGISTER;
//   // volatile uint32_t *counter_reload_lo_register = (volatile uint32_t *)TIM_G0_T0_LOAD_LO_REGISTER;
//   // volatile uint32_t *counter_reload_register = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;

//   // *(counter_reload_hi_register) = (uint32_t)0;
//   // *(counter_reload_lo_register) = (uint32_t)0;
//   // *(counter_reload_register) = (uint32_t)1;

//   // // register interrupt handler
//   // esp_intr_alloc(ETS_TG0_T0_LEVEL_INTR_SOURCE, 0, alarm_handler, NULL, NULL);
// }
