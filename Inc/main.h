#include "stm32f1xx.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ds18b20.h"


void vTaskBlink( void *argument);
void vTaskHello (void *argument);
void vTaskSendToUART(void *argument);
void vTaskADCConvert (void *argument);
void vTask1Wire (void *argument);
void vTaskTempUSART(void *argument);



void USART1SendByte(uint8_t b);
void USART1SendChar(char symb);
void USART1SendStr(char* str);
