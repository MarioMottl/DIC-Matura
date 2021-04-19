/*
  ampel.h
  March 2021
  Paul Raffer, Stefan Grubmueller
  HTBL Hollabrunn 5BHEL
  
  Funktion:
  Dieses Headerfile beinhaltet alle Prototypen und Konstanten,
  welche in main.c und ampel.c benoetigt werden.
*/
/* --------------Define to prevent recursive inclusion ----------------*/ 
#ifndef __AMPEL_H__
#define __AMPEL_H__

//includes
#include "stm32f10x.h"          //standard library 
#include "ARMV10_STD.h"			//wait_ms()
#include "stm32f10x_rcc.h"		//RCC library
#include "stm32f10x_rtc.h"		//RTC library
#include "stm32f10x_gpio.h"		//GPIO library
#include "stm32f10x_usart.h"	//USART communication library
#include "stm32f10x_tim.h"		//Timer library
#include "string.h"				
#include "stdbool.h"

//constants
enum led {
	green1,
	red,
};


//configuration for USART2
extern void InitUsart2(void);
//configuration of LEDs
extern void InitGpio(void);
//configuration Timer 3 Channel 1
extern void TIM3_Config(void);
//send string over uart
void UartPutString(USART_TypeDef *USARTx, char *str);
//turn LEDs on/off
void set_led(enum led led, _Bool on);
//let LED blink for set time
void blink_led(enum led led, int count, int time);
//ampel an/aus 
bool UartInIs(USART_TypeDef * const usart, char character);
//contains all configuration
void Init();


#endif 
