#include "libRTOS.h"

const char *OK = "OK";
const char *ReleOn = "Rele=ON";
const char *ReleOff = "Rele=OFF";
const char *EXTOn = "EXTPWR=ON";
const char *EXTOff = "EXTPWR=OFF";
const char *ADCOn = "ADC=ON";
const char *ADCOff = "ADC=OFF";


extern char commandStr[20];

extern uint16_t valueADC[5];
char strbuffer[10];



void vTask1Wire (void *argument){

	uint8_t dt1[8];
	uint16_t raw_temper1;
	uint8_t dt2[8];
	uint16_t raw_temper2;

	port_init();
	ds18b20_init(SKIP_ROM,0);

while (1){
	ds18b20_MeasureTemperCmd(SKIP_ROM, 0);
	vTaskDelay(1300);
	ds18b20_ReadStratcpad(SKIP_ROM, dt1,dt2, 0);
	raw_temper1 = ((uint16_t)dt1[1]<<8)|dt1[0];
	raw_temper2 = ((uint16_t)dt2[1]<<8)|dt2[0];

	sprintf(strbuffer, "%u",raw_temper1);
	USART1SendStr("Temp1      =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
	sprintf(strbuffer, "%u",raw_temper2);
	USART1SendStr("Temp2      =");USART1SendStr(strbuffer);USART1SendStr("\r\n");

}

}

void vTaskBlink( void *argument){
	//LED C13
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	GPIOC->CRH |= GPIO_CRH_MODE13; // PC13   - output
	GPIOC->CRH &= ~GPIO_CRH_CNF13;  //PC13   - GP out PP

	while(1){
	GPIOC->BSRR |= GPIO_BSRR_BS13;
	vTaskDelay(900);
	GPIOC->BSRR |= GPIO_BSRR_BR13;
	vTaskDelay(100);

	}
}

void vTaskADCConvert (void *argument){
	ADC_GPIO_Init();
	ADC_Mode_Init();
	ADC_POWER(ON);
	ADC_DMA_Init();

	USART1_GPIO_Init();
	USART1_Mode_Init();

	while (1){
		ADC1->CR2 |= ADC_CR2_SWSTART; // start
		while (  (DMA1->ISR & DMA_ISR_TCIF1) == 0  );


		sprintf(strbuffer, "%u",valueADC[0]);
		USART1SendStr("BatLevel      =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[1]);
		USART1SendStr("CurrentLevel  =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[2]);
		USART1SendStr("LED1          =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[3]);
		USART1SendStr("LED2          =");USART1SendStr(strbuffer);USART1SendStr("\r\n");
		sprintf(strbuffer, "%u",valueADC[4]);
		USART1SendStr("LED3          =");USART1SendStr(strbuffer);USART1SendStr("\r\n");

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

const char* const strsub(char* s, size_t pos, size_t count)
{
   static char buf[BUFSIZ];
   buf[sizeof buf - 1] = '\0';
   if ( count >= BUFSIZ )
      return NULL;
   else
      return strncpy(buf, s + pos, count);
}



void vTaskPWM(void *argument){
	PWM_Init_TIM3();
	PWM_Level(60);
	vTaskDelay(2000);
	while(1){
		PWM_Level(85);
		vTaskDelay(15000);
		PWM_Level(60);
		vTaskDelay(15000);

	}



}




void vTaskExeCmd (void *argument){


		if (strncmp(commandStr, OK , 2)==0){
			USART1SendStr("---------OK---------");
		}
		if (strncmp(commandStr, EXTOn , 9)==0){
			ExtPWR(ON);
			USART1SendStr("--- EXT POWER -- ON ---");
		}
		if (strncmp(commandStr, EXTOff , 10)==0){
			ExtPWR(OFF);
			USART1SendStr("--- EXT POWER -- OFF ---");
		}
		if (strncmp(commandStr, ReleOn , 7)==0){
			Rele(ON);
			USART1SendStr("--- RELE -- ON ---");
		}
		if (strncmp(commandStr, ReleOff , 8)==0){
			Rele(OFF);
			USART1SendStr("--- RELE -- OFF ---");
		}
		if (strncmp(commandStr, ADCOn , 6)==0){
			ADC_POWER(ON);
			USART1SendStr("--- ADC -- ON ---");
		}
		if (strncmp(commandStr, ADCOff , 7)==0){
			ADC_POWER(OFF);
			USART1SendStr("--- ADC -- OFF ---");
		}




}
