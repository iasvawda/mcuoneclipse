/*
 * application.c
 *
 *  Created on: 22.04.2019
 *      Author: Erich Styger
 */

#include "platform.h"
#include "application.h"
#include "board.h"
#include "McuLib.h"
#include "McuWait.h"
#include "McuLED.h"
#include "McuRTOS.h"

#include "fsl_iocon.h"
#include "pin_mux.h"
#include "leds.h"

static SemaphoreHandle_t mutex;

static void AppTask(void *pv) {
  for(;;) {
    McuLED_Neg(LED_Red);
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

static TimerHandle_t timerHndl;
#define TIMER_PERIOD_MS 100

static void vTimerCallback(TimerHandle_t pxTimer) {
  /* TIMER_PERIOD_MS ms timer */
}

void APP_Run(void) {
  PL_Init(); /* init modules */

  McuLED_Neg(LED_Red);
  McuWait_Waitms(100);
  McuLED_Neg(LED_Green);
  McuWait_Waitms(100);
  McuLED_Neg(LED_Blue);
  McuWait_Waitms(100);

  if (xTaskCreate(
      AppTask,  /* pointer to the task */
      "App", /* task name for kernel awareness debugging */
      300/sizeof(StackType_t), /* task stack size */
      (void*)NULL, /* optional task startup argument */
      tskIDLE_PRIORITY+2,  /* initial priority */
      (TaskHandle_t*)NULL /* optional task handle to create */
    ) != pdPASS) {
     for(;;){} /* error! probably out of memory */
  }
  timerHndl = xTimerCreate("timer0", pdMS_TO_TICKS(TIMER_PERIOD_MS), pdTRUE, (void *)0, vTimerCallback);
  if (timerHndl==NULL) {
    for(;;); /* failure! */
  }
  if (xTimerStart(timerHndl, 0)!=pdPASS) {
    for(;;); /* failure! */
  }
  mutex = xSemaphoreCreateMutex();
  if (mutex!=NULL) {
    vQueueAddToRegistry(mutex, "Mutex");
  }
  vTaskStartScheduler();
  for(;;) { /* should not get here */ }
}