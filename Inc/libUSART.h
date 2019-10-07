#include "stm32f1xx.h"
#include "stdio.h"
void USART1_GPIO_Init(void);
void USART1_Mode_Init(void);
void USART1SendByte(uint8_t b);
void USART1SendChar(char symb);
void USART1SendStr(char* str);
