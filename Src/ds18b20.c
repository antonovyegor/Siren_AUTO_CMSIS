#include "ds18b20.h"
#include "defines.h"

//--------------------------------------------------
void DelayMicro( uint32_t micros)
{
micros *= (SystemCoreClock / 1000000) / 9;
/* Wait till done */
while (micros--) ;
}
//--------------------------------------------------
void port_init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	GPIOB->CRH |= GPIO_CRH_MODE11;
	GPIOB->CRH |= GPIO_CRH_CNF11_0;
	GPIOB->CRH &= ~GPIO_CRH_CNF11_1;

	GPIOB->CRH |= GPIO_CRH_MODE10;
	GPIOB->CRH |= GPIO_CRH_CNF10_0;
	GPIOB->CRH &= ~GPIO_CRH_CNF10_1;
}
//--------------------------------------------------
uint8_t ds18b20_Reset(void)
{
  uint16_t status1 , status2;
  GPIOB->ODR &= ~GPIO_ODR_ODR11;
  GPIOB->ODR &= ~GPIO_ODR_ODR10;

  DelayMicro(485);
  GPIOB->ODR |= GPIO_ODR_ODR11;
  GPIOB->ODR |= GPIO_ODR_ODR10;
  DelayMicro(65);
  status1 = GPIOB->IDR & GPIO_IDR_IDR11;
  status2 = GPIOB->IDR & GPIO_IDR_IDR10;
  DelayMicro(500);

  return (status1||status2 ? 1 : 0);
}
//----------------------------------------------------------
uint8_t ds18b20_ReadBit_11(void)
{
  uint8_t bit = 0;
  GPIOB->ODR &= ~GPIO_ODR_ODR11;
  DelayMicro(2);
	GPIOB->ODR |= GPIO_ODR_ODR11;
	DelayMicro(13);
	bit = (GPIOB->IDR & GPIO_IDR_IDR11 ? 1 : 0);
	DelayMicro(45);
  return bit;
}
uint8_t ds18b20_ReadBit_10(void)
{
  uint8_t bit = 0;
  GPIOB->ODR &= ~GPIO_ODR_ODR10;
  DelayMicro(2);
	GPIOB->ODR |= GPIO_ODR_ODR10;
	DelayMicro(13);
	bit = (GPIOB->IDR & GPIO_IDR_IDR10 ? 1 : 0);
	DelayMicro(45);
  return bit;
}
//-----------------------------------------------
uint8_t ds18b20_ReadByte_11(void)
{
  uint8_t data = 0;
  for (uint8_t i = 0; i <= 7; i++)
  data += ds18b20_ReadBit_11() << i;
  return data;
}
uint8_t ds18b20_ReadByte_10(void)
{
  uint8_t data = 0;
  for (uint8_t i = 0; i <= 7; i++)
  data += ds18b20_ReadBit_10() << i;
  return data;
}
//-----------------------------------------------
void ds18b20_WriteBit_11(uint8_t bit)
{
  GPIOB->ODR &= ~GPIO_ODR_ODR11;
  DelayMicro(bit ? 3 : 65);
  GPIOB->ODR |= GPIO_ODR_ODR11;
  DelayMicro(bit ? 65 : 3);
}
void ds18b20_WriteBit_10(uint8_t bit)
{
  GPIOB->ODR &= ~GPIO_ODR_ODR10;
  DelayMicro(bit ? 3 : 65);
  GPIOB->ODR |= GPIO_ODR_ODR10;
  DelayMicro(bit ? 65 : 3);
}
//-----------------------------------------------
void ds18b20_WriteByte_11(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_WriteBit_11(dt >> i & 1);
    DelayMicro(5);
  }
}
void ds18b20_WriteByte_10(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_WriteBit_10(dt >> i & 1);
    DelayMicro(5);
  }
}
//-----------------------------------------------
uint8_t ds18b20_init(uint8_t mode, uint8_t *ROM)
{
	if(ds18b20_Reset()) return 1;

	if(mode==SKIP_ROM){
		ds18b20_WriteByte_11(0xCC);
		ds18b20_WriteByte_10(0xCC);//SKIP ROM

	}
	else{
//		ds18b20_WriteByte_11(0x55); // MATCH ROM
//		ds18b20_WriteByte_10(0x55); // MATCH ROM
//		vTaskDelay(1);
//		for(int j=0;j<8;j++)
//			ds18b20_WriteByte(&ROM[j]); //SKIP ROM
//
//		vTaskDelay(1);
	}
	ds18b20_WriteByte_11(0x4E);//WRITE SCRATCHPAD
	ds18b20_WriteByte_10(0x4E);//WRITE SCRATCHPAD
	ds18b20_WriteByte_11(0x64);//TH REGISTER 100C
	ds18b20_WriteByte_10(0x64);//TH REGISTER 100C
	ds18b20_WriteByte_11(0x1E);//TL REGISTER - 30C
	ds18b20_WriteByte_10(0x1E);//TL REGISTER - 30C
	ds18b20_WriteByte_11(RESOLUTION_12BIT);//Resolution 12 bit
	ds18b20_WriteByte_10(RESOLUTION_12BIT);//Resolution 12 bit
  return 0;
}
//----------------------------------------------------------
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t *ROM)
{
  ds18b20_Reset();
  if(mode==SKIP_ROM){
	  ds18b20_WriteByte_11(0xCC);//SKIP ROM
  	  ds18b20_WriteByte_10(0xCC);//SKIP ROM
  }
  else {
//	  ds18b20_WriteByte(0x55); // MATCH ROM
//	  for(int j=0;j<8;j++)
//		  ds18b20_WriteByte(ROM[j]); //SKIP ROM
  }
  //CONVERT T
  ds18b20_WriteByte_11(0x44);
  ds18b20_WriteByte_10(0x44);
}
//----------------------------------------------------------
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data1,uint8_t *Data2, uint8_t *ROM)
{
  uint8_t i;
  ds18b20_Reset();

  if(mode==SKIP_ROM){
	  ds18b20_WriteByte_11(0xCC); //SKIP ROM
	  ds18b20_WriteByte_10(0xCC);
  }
  else{
//	  ds18b20_WriteByte(0x55); // MATCH ROM
//	  vTaskDelay(1);
//	  for(int j=0;j<8;j++)
//		  ds18b20_WriteByte(ROM[j]); //SKIP ROM
//	  vTaskDelay(1);
  }
  //READ SCRATCHPAD
  ds18b20_WriteByte_11(0xBE);
  ds18b20_WriteByte_10(0xBE);
  for(i=0;i<8;i++)
  {
    Data1[i] = ds18b20_ReadByte_11();
    Data2[i] = ds18b20_ReadByte_10();
  }
}
//----------------------------------------------------------
uint8_t ds18b20_GetSign(uint16_t dt)
{
  if (dt&(1<<11)) return 1;
  else return 0;
}
//----------------------------------------------------------
float ds18b20_Convert(uint16_t dt)
{
  float t;
  t = (float) ((dt&0x07FF)>>4);
  t += (float)(dt&0x000F) / 16.0f;
  return t;
}
//----------------------------------------------------------

void ds18b20_ReadROM(uint8_t *Data)
{
//  uint8_t i;
//  ds18b20_Reset();
//  ds18b20_WriteByte(0x33);  // Read ROM  1 slave on the bus
//  for(i=0;i<8;i++)
//  {
//    Data[i] = ds18b20_ReadByte_1();
//  }

}


