#include "main.h"
#include "stdio.h"


//-------------------------------------

//------------------------------------------------------------
static xQueueHandle TransmitDataTemp1;
static xQueueHandle TransmitDataTemp2;
static xQueueHandle TransmitDataADC3;
static xQueueHandle TransmitDataADC4;
static xQueueHandle ReceiveCommand;

static uint16_t valueADC[2];


//------------------------------------------------------------

int main (void){

	 // настройка HSE PLL SYSCLK
	RCC->CR |= RCC_CR_HSEON;
	while (!  (RCC->CR & RCC_CR_HSERDY)   );

	FLASH ->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	RCC->CFGR &= ~RCC_CFGR_HPRE;  // 0000   /1

	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;//100 - /2

	RCC->CFGR &= ~RCC_CFGR_PPRE2; //000  - /1

	RCC->CFGR |=  RCC_CFGR_ADCPRE_DIV4;


	RCC->CFGR &= ~RCC_CFGR_PLLSRC;
	RCC->CFGR &= ~RCC_CFGR_PLLMULL;
	RCC->CFGR &= ~RCC_CFGR_PLLXTPRE;

	RCC->CFGR |= RCC_CFGR_PLLSRC;
	RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE;
	RCC->CFGR |= RCC_CFGR_PLLMULL6;



	RCC->CR |= RCC_CR_PLLON;
	while (!  (RCC->CR & RCC_CR_PLLRDY)   );

	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |=  RCC_CFGR_SW_PLL;

	RCC->CFGR |= RCC_CFGR_MCOSEL_PLL_DIV2;


	while (  (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_1   ) ;

	// тактирование
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	//RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	//GPIO
		// 1 Wire pins
	GPIOA->CRH |= GPIO_CRH_MODE13; // output
	GPIOA->CRH |= GPIO_CRH_CNF13_0; // open drain


		// ADC ports
	GPIOA->CRL &= ~GPIO_CRL_MODE0; // PA0   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF0;  //PA0    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE1; // PA1   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF1;  //PA1    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE2; // PA2  - input
	GPIOA->CRL &= ~GPIO_CRL_CNF2;  //PA2    - analog mode

	GPIOA->CRL &= ~GPIO_CRL_MODE3; // PA3   - input
	GPIOA->CRL &= ~GPIO_CRL_CNF3;  //PA3    - analog mode



		// UART ports
	GPIOA->CRH |= GPIO_CRH_MODE9; // PA9    - output      TX
	GPIOA->CRH |= GPIO_CRH_CNF9_1;  //PA9   - GP out PP   TX
	GPIOA->CRH &= ~ GPIO_CRH_MODE10; // PA10  - input                     RX
	GPIOA->CRH |= GPIO_CRH_CNF10_0;  //PA10  - Alternative input float   RX



	//UART INIT

	USART1->BRR =  0x1A1; // 417   -- 115200 48MHz

	USART1->CR1 |= USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	USART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(USART1_IRQn);



	//ADC


	ADC1->CR2 |= ADC_CR2_CONT;
	//ADC1->CR2 &= ~ADC_CR2_CONT;



	ADC1->CR2 |= ADC_CR2_EXTSEL;
	ADC1->CR2 |= ADC_CR2_EXTTRIG;

	ADC1->SQR1 &= ~ADC_SQR1_L;

	ADC1->SQR1 |= ADC_SQR1_L_0;  /// 2



	ADC1->SMPR2 &= ~ADC_SMPR2_SMP0; //  0 канал  сэмплирование  циклов
	ADC1->SMPR2 &= ~ADC_SMPR2_SMP1; //  1 канал  сэмплирование  циклов


	ADC1->SMPR2 |= ADC_SMPR2_SMP0 ;
	ADC1->SMPR2 |= ADC_SMPR2_SMP1 ;

	ADC1->SQR3 &= ~ADC_SQR3_SQ1;  //PA0
	ADC1->SQR3 |= ADC_SQR3_SQ2_0;


	ADC1->CR1 |= ADC_CR1_SCAN;

	ADC1->CR2 |= ADC_CR2_ADON;

	ADC1->CR2 |= ADC_CR2_RSTCAL;
	while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_RSTCAL);
	ADC1->CR2 |= ADC_CR2_CAL;
	while ((ADC1->CR2 & ADC_CR2_RSTCAL) == ADC_CR2_CAL); // пока там 1 идет калибровка по окончанию идет сброс бита


	//--------------------------------------------------------------------------------
	//     DMA   ADC1

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel1->CPAR = (uint32_t) &ADC1->DR;
	DMA1_Channel1->CMAR = (uint32_t) &valueADC[0];
	DMA1_Channel1->CNDTR = 2;

	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
	DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

	DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
	DMA1_Channel1->CCR |= DMA_CCR_MINC;

	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	//DMA1_Channel1->CCR &= ~DMA_CCR_CIRC;

	DMA1_Channel1->CCR &= ~DMA_CCR_DIR;

	DMA1_Channel1->CCR |= DMA_CCR_EN;

	ADC1->CR2 |= ADC_CR2_DMA;

	//--------------------------------------------------------------------------------
	TransmitDataTemp1 = xQueueCreate(3,sizeof(uint16_t));
	TransmitDataTemp2 = xQueueCreate(3,sizeof(uint16_t));
	TransmitDataADC3 = xQueueCreate(3,sizeof(uint16_t));
	TransmitDataADC4 = xQueueCreate(3,sizeof(uint16_t));

	ReceiveCommand = xQueueCreate(1, sizeof(uint8_t));

	TIM2->CR1 |= TIM_CR1_UDIS;
	GPIOC->CRH |= GPIO_CRH_MODE13; // PC13   - output
	GPIOC->CRH &= ~GPIO_CRH_CNF13;  //PC13   - GP out PP


	xTaskCreate(vTaskMainConvert, "ADC ", 128, NULL, 2, NULL);
	xTaskCreate(vTaskTEMP, "Temp", 128, NULL, 1, NULL);
	xTaskCreate(vTaskUSART, "UART ", 128, NULL, 1, NULL);

	vTaskStartScheduler();

	while (1){
		USART1SendStr("Hello ");
		USART1SendStr("\r\n");
	}


}


//================================================================================


void vTaskTemp (void *argument){
	uint8_t dt[8];
	uint8_t status;
	uint16_t raw_temper;

	char c;
	char str1[60];

	port_init();
	status = ds18b20_init(SKIP_ROM);
	sprintf(str1,"Init Status: %d\r\n",status);
	USART1SendStr(str1);USART1SendStr("\r\n");

while (1){
	ds18b20_MeasureTemperCmd(SKIP_ROM, 0);
	vTaskDelay(1000);
	ds18b20_ReadStratcpad(SKIP_ROM, dt, 0);
	raw_temper = ((uint16_t)dt[1]<<8)|dt[0];
	xQueueSend(TransmitDataTemp1,&raw_temper,0);
	vTaskDelay(1000);
}

}


void vTaskUSART(void *argument){
	uint16_t uBuffer;
	char strbuffer[6];


	while (1){
		if (uxQueueMessagesWaiting(TransmitDataTemp1)!= 0){
			xQueueReceive(TransmitDataTemp1, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("Temp1=");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataTemp2)!= 0){
			xQueueReceive(TransmitDataTemp2, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("Temp2=");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC3)!= 0){
			xQueueReceive(TransmitDataADC3, &uBuffer, 0);

			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("BatLevel=");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		if (uxQueueMessagesWaiting(TransmitDataADC4)!= 0){
			xQueueReceive(TransmitDataADC4, &uBuffer, 0);
			sprintf(strbuffer, "%u",uBuffer);
			USART1SendStr("NoiseLevel=");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		}
		vTaskDelay(1000);

	}
}

void vTaskMainConvert (void *argument){
	while (1){
		ADC1->CR2 |= ADC_CR2_SWSTART; // start
		while (  (DMA1->ISR & DMA_ISR_TCIF1) == 0  );
		xQueueSend(TransmitDataADC3,&valueADC[0],0);
		xQueueSend(TransmitDataADC4,&valueADC[1],0);
		vTaskDelay(1000);
	}
}


//=================================================================================
void USART1_IRQHandler (void ){
	uint8_t commandBuffer;
	char data;
	char str[] = "";
	uint8_t i=0;
	if (USART1->SR & USART_SR_RXNE){
		USART1->SR &= ~USART_SR_RXNE;

		data=USART1->DR;

		if (data==0){
			i=0;
			commandBuffer = 0xFF;
			xQueueSendToBackFromISR(ReceiveCommand,&commandBuffer,0);
					// описать команды получения
					// лучше чтобы уменьшить время прерывания, сохранять команду и устанавливать флаг что пришла новая команда
					// после ее обработки этот флаг сбрасывать

			// обнуление формирование строки, команды, отправка команды в очередь
		}
		else {
			str[i++]=data;
		}
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




