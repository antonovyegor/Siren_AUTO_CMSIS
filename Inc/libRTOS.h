#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "ds18b20.h"
#include "stm32f1xx.h"
#include "libADC.h"
#include "defines.h"
#include "libUSART.h"
#include <stdio.h>



void vTaskBlink( void *argument);
void vTaskSendToUART(void *argument);
void vTaskADCConvert (void *argument);
void vTask1Wire (void *argument);
void vTaskTempUSART(void *argument);
void vTaskReadROM(void *argument);
void vTaskReadROM(void *argument);

