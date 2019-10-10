#include "main.h"


xQueueHandle TransmitDataTemp1;
xQueueHandle TransmitDataTemp2;
xQueueHandle TransmitDataADC3;
xQueueHandle TransmitDataADC4;
xQueueHandle ReceiveCommand;
uint16_t valueADC[2];



int main (void){

	RCC_PLL_Init();

	//LED C13
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13; // PC13   - output
	GPIOC->CRH &= ~GPIO_CRH_CNF13;  //PC13   - GP out PP


	//--------------------------------------------------------------------------------
	TransmitDataTemp1 = xQueueCreate(1,sizeof(uint16_t));
	TransmitDataTemp2 = xQueueCreate(1,sizeof(uint16_t));
	TransmitDataADC3 = xQueueCreate(3,sizeof(uint16_t));
	TransmitDataADC4 = xQueueCreate(3,sizeof(uint16_t));
	ReceiveCommand = xQueueCreate(30, sizeof(char));


	xTaskCreate(vTaskBlink, "Led Blink 13", 16, NULL, 1, NULL);
	xTaskCreate(vTaskADCConvert, "ADC ", 128, NULL, 1, NULL);
	xTaskCreate(vTask1Wire, "Temp", 128, NULL, 1, NULL);


	//xTaskCreate(vTaskReadROM, "ROM", 128, NULL, 1, NULL);
	xTaskCreate(vTaskSendToUART, "UART ", 128, NULL, 1, NULL);

	vTaskStartScheduler();

	while (1){

	}


}

//=================================================================================


