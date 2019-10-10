#include  "libUSART.h"
#include "defines.h"


extern xQueueHandle ReceiveCommand;



void USART1_GPIO_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	// UART ports
	GPIOA->CRH |= GPIO_CRH_MODE9; // PA9    - output      TX
	GPIOA->CRH |= GPIO_CRH_CNF9_1;  //PA9   - GP out PP   TX
	GPIOA->CRH &= ~ GPIO_CRH_MODE10; // PA10  - input                     RX
	GPIOA->CRH |= GPIO_CRH_CNF10_0;  //PA10  - Alternative input float   RX

}


void USART1_Mode_Init(void){

	USART1->BRR =  0x1A1; // 417   -- 115200 48MHz

	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);



}
void USART1_IRQHandler (void ){
	char data;
	if (USART1->SR & USART_SR_RXNE){
		USART1->SR &= ~USART_SR_RXNE;
		data=USART1->DR;
		xQueueSendToBackFromISR(ReceiveCommand,&data,0);
	}

}
void USART1SendByte(uint8_t b){
	while (! (USART1->SR & USART_SR_TC));
	USART1->DR = b;

}
void USART1SendChar(char symb){
	while (! (USART1->SR & USART_SR_TC));
	USART1->DR = symb;

}
void USART1SendStr(char* str){
	uint8_t i=0;
	while (str[i]){
		USART1SendChar(str[i++]);
	}

}





