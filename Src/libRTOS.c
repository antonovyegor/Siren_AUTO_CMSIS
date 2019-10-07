#include "libRTOS.h"
extern xQueueHandle TransmitDataTemp1;
extern xQueueHandle TransmitDataTemp2;
extern xQueueHandle TransmitDataADC3;
extern xQueueHandle TransmitDataADC4;
extern xQueueHandle ReceiveCommand;
extern uint16_t valueADC[2];




void vTask1Wire (void *argument){

	uint8_t dt[8];
	uint8_t ROM1[8] = {0xFC, 0x02, 0x03,0x92,0x45,0xB8, 0x89,0x28};
	//uint8_t ROM2[8] = {0x28, 0x89, 0xB8,0x45,0x92,0x03, 0x02,0xfc};
	//uint8_t status;
	uint16_t raw_temper;

	port_init();
	ds18b20_init(SKIP_ROM,ROM1);

while (1){
	ds18b20_MeasureTemperCmd(SKIP_ROM, ROM1);
	vTaskDelay(1300);
	ds18b20_ReadStratcpad(SKIP_ROM, dt, ROM1);
	raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	xQueueSend(TransmitDataTemp1,&raw_temper,0);
}

}

void vTaskSendToUART(void *argument){
	uint16_t uBuffer;
	char strbuffer[6];

	//USART  1
	USART1_GPIO_Init();
	USART1_Mode_Init();

	while (1){
		if (uxQueueMessagesWaiting(TransmitDataTemp1)!= 0){
			xQueueReceive(TransmitDataTemp1, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("Temp1=        ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataTemp2)!= 0){
			xQueueReceive(TransmitDataTemp2, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("Temp2=        ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC3)!= 0){
			xQueueReceive(TransmitDataADC3, &uBuffer, 0);

			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("BatLevel=     ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC4)!= 0){
			xQueueReceive(TransmitDataADC4, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("NoiseLevel=   ");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		vTaskDelay(1000);

	}
}
void vTaskBlink( void *argument){
	while(1){
	GPIOC->BSRR |= GPIO_BSRR_BS13;
	vTaskDelay(1000);
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	vTaskDelay(1000);




	}
}

void vTaskADCConvert (void *argument){
	ADC_GPIO_Init();
	ADC_Mode_Init();
	ADC_POWER(ON);
	ADC_DMA_Init();
	while (1){
		ADC1->CR2 |= ADC_CR2_SWSTART; // start
		while (  (DMA1->ISR & DMA_ISR_TCIF1) == 0  );
		xQueueSend(TransmitDataADC3,&valueADC[0],0);
		xQueueSend(TransmitDataADC4,&valueADC[1],0);
		vTaskDelay(1000);
	}
}

void vTaskReadROM(void *argument){
	char str[50];
	uint8_t ROM[8];

	port_init();
	ds18b20_init(SKIP_ROM,0);
	vTaskDelay(10);
	ds18b20_ReadROM(ROM);
	sprintf(str,"%x %x %x %x %x %x %x %x \r\n",ROM[0],ROM[1],ROM[2],ROM[3],ROM[4],ROM[5],ROM[6],ROM[7]);
	USART1SendStr(str);


}



