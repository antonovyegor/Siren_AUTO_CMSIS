#include "stm32f1xx.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "ds18b20.h"


void vTaskUART(void *argument);
void vTaskMainConvert (void *argument);
void vTaskTemp (void *argument);
void vTaskUSART(void *argument);



void USART1SendByte(uint8_t b);
void USART1SendChar(char symb);
void USART1SendStr(char* str);
