#include "defines.h"
#include "libRCC.h"

void RCC_PLL_Init(void){
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
}
