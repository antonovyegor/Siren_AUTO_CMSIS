#include "main.h"


uint16_t valueADC[5];

char data;
uint8_t indexC =0;
char commandStr[20];
char commandStrBuf[20];

uint8_t newCommandFlag=0;



int main (void){


	RCC_PLL_Init();
	GPIO_Rele_Init();



	xTaskCreate(vTaskBlink, "Led Blink 13", 32, NULL, 3, NULL);
	xTaskCreate(vTaskADCConvert, "ADC ", 128, NULL, 1, NULL);
	xTaskCreate(vTask1Wire, "Temp", 128, NULL, 2, NULL);


	vTaskStartScheduler();

	while (1){

	}


}



void USART1_IRQHandler(){

	if (USART1->SR & USART_SR_RXNE)
	{
		data=USART1->DR;
		if (data=='\r' || data=='\n' || data==0 || indexC==sizeof(commandStrBuf)-1)
		{
			if (!newCommandFlag){
				int i=0;
				memset(commandStr,0,sizeof(commandStr));
				while(commandStrBuf[i]){
					commandStr[i]=commandStrBuf[i];
					i++;
				}
				newCommandFlag=1;
			}
			else{
				USART1SendStr("---REPEAT---");
			}
			memset(commandStrBuf,0,sizeof(commandStrBuf));
			indexC=0;
		}
		else {
			commandStrBuf[indexC++]=data;
		}
		USART1->SR &= ~USART_SR_RXNE;
	}
}


