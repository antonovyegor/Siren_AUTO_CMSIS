#include "stm32f1xx.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

void USART1_GPIO_Init(void);
void USART1_Mode_Init(void);
void USART1SendByte(uint8_t b);
void USART1SendChar(char symb);
void USART1SendStr(char* str);
void USART1_DMA_Init(void);


