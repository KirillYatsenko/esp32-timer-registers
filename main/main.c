#include <stdio.h>
#include <inttypes.h>

#include "esp_log.h"
#include "esp_intr_alloc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/timer.h"
#include "freertos/queue.h"

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

#define TIM_G0_T0_ALARM_HI_REGISTER 0x3FF5F014 // alarm value high bits
#define TIM_G0_T0_ALARM_LO_REGISTER 0x3FF5F010 // alarm value low bits

#define TIM_G0_INT_SET_REGISTER 0x3FF5F098 // clear interrupt (status?)
#define TIM_G0_INT_CLR_REGISTER 0x3FF5F0A4 // clear interrupt (status?)
#define TO_ITR_STATUS (1)
#define WDT_ITR_STATUS (1 << 2)

// Config values
#define ALARM_ENABLE (1 << 10)
#define ALARM_DISABLE (0 << 10)
#define LEVEL_INTERRUPT_ENABLE (1 << 11)
#define LEVEL_INTERRUPT_DISABLE (0 << 11)
#define EDGE_INTERRUPT_ENABLE (0 << 12)
#define DIVIDER_CONFIG_VALUE (DIVIDER << 13) // Divide clock frequency
#define AUTORELOAD_ENABLE (1 << 29)
#define INCREASE_ENABLE (1 << 30) // 1 for increment, 0 for decrement
#define TIMER_ENABLE (1 << 31)

volatile uint32_t *config_register = (volatile uint32_t *)TIM_G0_T0_CONFIG_REGISTER;
xQueueHandle timer_queue;

typedef struct
{
  uint64_t timer_counter;
} timer_event;

void IRAM_ATTR alarm_handler(void *args)
{
  timer_event evt;

  // clear interrupt status bit
  volatile uint32_t *interrupt_clear_register = (volatile uint32_t *)TIM_G0_INT_CLR_REGISTER;
  *(interrupt_clear_register) |= TO_ITR_STATUS;

  // get the counter value
  volatile uint32_t *counter_update_register = (volatile uint32_t *)TIM_G0_T0_UPDATE_REGISTER;
  volatile uint32_t *counter_hi_register = (volatile uint32_t *)TIM_G0_T0_HI_REGISTER;
  volatile uint32_t *counter_low_register = (volatile uint32_t *)TIM_G0_T0_LO_REGISTER;
  *(counter_update_register) = 1;
  evt.timer_counter = *(counter_hi_register) | *(counter_low_register);
  
  // send counter value to the queue
  xQueueSendFromISR(timer_queue, &evt, NULL);

  // reenable alarm since it's disabled after the alarm
  volatile uint32_t *interrupt_set_register = (volatile uint32_t *)TIM_G0_INT_SET_REGISTER;
  *(config_register) |= ALARM_ENABLE;
}

void timer_event_handler(void *arg)
{
  while (1)
  {
    timer_event evt;
    xQueueReceive(timer_queue, &evt, portMAX_DELAY);
    ESP_LOGI("TIMER", "tick");
  }
}

void app_main(void)
{
  ESP_LOGI(TAG, "starting timer...");

  volatile uint32_t *alarm_hi_register = (volatile uint32_t *)TIM_G0_T0_ALARM_HI_REGISTER;
  volatile uint32_t *alarm_lo_register = (volatile uint32_t *)TIM_G0_T0_ALARM_LO_REGISTER;

  *(alarm_hi_register) = (uint32_t)0;
  *(alarm_lo_register) = (uint32_t)ALARM_VALUE;

  // setting up counter reload initial value
  volatile uint32_t *counter_reload_hi_register = (volatile uint32_t *)TIM_G0_T0_LOAD_HI_REGISTER;
  volatile uint32_t *counter_reload_lo_register = (volatile uint32_t *)TIM_G0_T0_LOAD_LO_REGISTER;
  volatile uint32_t *counter_reload_register = (volatile uint32_t *)TIM_G0_T0_LOAD_REGISTER;

  *(counter_reload_hi_register) = (uint32_t)0;
  *(counter_reload_lo_register) = (uint32_t)0;
  *(counter_reload_register) = (uint32_t)1;

  // register interrupt handler
  esp_intr_alloc(ETS_TG0_T0_LEVEL_INTR_SOURCE, ESP_INTR_FLAG_IRAM, alarm_handler, NULL, NULL);

  // start timer
  uint32_t config_value = (uint32_t)(TIMER_ENABLE | INCREASE_ENABLE | AUTORELOAD_ENABLE |
                                     DIVIDER_CONFIG_VALUE | EDGE_INTERRUPT_ENABLE | LEVEL_INTERRUPT_ENABLE | ALARM_ENABLE);

  *(config_register) = config_value;

  // start queue handler
  timer_queue = xQueueCreate(10, sizeof(timer_event));
  xTaskCreate(timer_event_handler, "timer_event_handler", 2048, NULL, 5, NULL);
}
