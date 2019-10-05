#include "stm32f1xx.h"
#include "ds18b20.h"


void USART1SendByte(uint8_t b);
void USART1SendChar(char symb);
void USART1SendStr(char* str);


void delay(int milisec);
