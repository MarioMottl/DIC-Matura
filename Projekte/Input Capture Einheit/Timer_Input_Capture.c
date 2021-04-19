/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2015 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   Timer_Input_Capture.c                                         */
/* Autor: 		  Simon Brenninger, David Kopper                                */
/* Version: 	  V1.00                                                         */
/* Date: 		    10/04/2021                                                    */
/* Description: demoprogramm for CM3 Peripheral Library with topics:					*/
/* 							Input Capture Einheit Timer4, LED /Schalterplatine						*/
/*							and UART#1 (Polling) 																				  */
/*              							                                                */
/******************************************************************************/
/* History: 	V1.00  creation										          										*/
/******************************************************************************/
#include <stm32f10x.h>
#include <armv10_std.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*---------------------------- Function Prototypes ---------------------------*/
static void USART_Send_String(char *str, int strlen);
static void USART1_conf(void);
static void TIM4_Config(void);


/*----------------------------- Globale Variablen -----------------------------*/
__IO uint16_t IC4ReadValue_new = 0, IC4ReadValue_prev = 0;
__IO uint16_t CaptureNumber = 0;

bool flanke;

/*****************************************************************************/
/*                     TIM4 Interrupt Service Routine                        */
/*****************************************************************************/
void TIM4_IRQHandler(void)
{	
	if ((TIM_GetFlagStatus(TIM4,TIM_FLAG_Update) != RESET) && (TIM_GetITStatus(TIM4,TIM_IT_Update) != 0))
	{	// Update Interrupt Pending ?
		TIM_ClearFlag(TIM4, TIM_FLAG_Update); /* Clear TIM4 Update Int. pending bit */
	}
	if ((TIM_GetFlagStatus(TIM4,TIM_FLAG_CC4) != RESET) && (TIM_GetITStatus(TIM4,TIM_IT_CC4) != 0))
	{	// Capture Interrupt von Ch4?
		TIM_ClearFlag(TIM4, TIM_FLAG_CC4); /* Clear TIM4 Capture Int. pending bit Ch4*/

		if(CaptureNumber == 0)
		{
			IC4ReadValue_new = TIM_GetCapture4(TIM4);/* Get Input Capture value for Ch4*/
			CaptureNumber = 1;
		}
		else if(CaptureNumber == 1)
		{
			IC4ReadValue_prev = IC4ReadValue_new;
			IC4ReadValue_new = TIM_GetCapture4(TIM4); /* Get Input Capture value for Ch4*/
		}
		flanke=true;
	}
}
/*****************************************************************************/
/*                           Send String over UART1                          */
/*****************************************************************************/
static void USART_Send_String(char *str, int strlen)
{
	// Sends a string, character for character, over the UART1
	for(int i=0; i<strlen; i++)
	{
		USART_SendData(USART1, str[i]);
		while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	}
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
}

/*****************************************************************************/
/*                             Configure USART1                              */
/*****************************************************************************/
static void USART1_conf()
{
	GPIO_InitTypeDef gpio;
	USART_ClockInitTypeDef usartclock;
	USART_InitTypeDef usart;

	
	SystemCoreClockUpdate();
	USART_DeInit(USART1);
	
	// Enable all GPIO and USART clocks needed for USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
	
	// Create gpio structure
	GPIO_StructInit(&gpio);
	
	// PA9: alternate function push pull (Tx)
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &gpio);
	
	// PA10: input floating (Rx)
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &gpio);
	
	// Init USART1 Clock
	USART_ClockStructInit(&usartclock);
	usartclock.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInit(USART1, &usartclock);
	
	// Create usart structure
	USART_StructInit(&usart);
	usart.USART_BaudRate = 115200;	//baudrate set to 115200
	USART_Init(USART1, &usart);
	
	// Enable USART1
	USART_Cmd(USART1, ENABLE);
}

/*****************************************************************************/
/*                             Configure Timer 4                             */
/*****************************************************************************/
static void TIM4_Config(void)
{
	GPIO_InitTypeDef gpio;	
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef nvic;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOB Clock Enable
	
	GPIO_StructInit(&gpio);	// Create gpio structure
	gpio.GPIO_Mode = GPIO_Mode_IPU; // PB9(=LED1) --> Timer 4 Channel 4 in Mode Input Pull UP 
	gpio.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &gpio);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); // Clock Enable Timer 4
	
	/* ----------- Configure Timer 4----------------------*/
	TIM_DeInit(TIM4);
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	/* ---------- T_INT = 125ns, Annahme: Presc = 1 --> Auto Reload Wert = 65535 --> 16,4ms*/
	//Auto-Reload Wert = Maximaler Zaehlerstand des Upcounters
	TIM_TimeBase_InitStructure.TIM_Period = 0xFFFF;	
	//Value Prescaler (reduce system clock rate to 2kHz)
	TIM_TimeBase_InitStructure.TIM_Prescaler = 1; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure);

	/* ---------- Configure TIM4 CH4 (PB9) as input capture channel------------*/
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;
	/* capture only when an rising edge is detected*/ 
 	TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
 	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
 	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
 	TIM_ICInitStructure.TIM_ICFilter = 0x0; /* filter to avoid bouncing */
	TIM_ICInit(TIM4, &TIM_ICInitStructure);
	
	// Timer 4 Update Interrupt Enable  
	TIM_ITConfig (TIM4, TIM_IT_Update,ENABLE); 
	// Timer 4 Input Capture Interrupt Channel 4 (CC4) Enable
	TIM_ITConfig (TIM4, TIM_IT_CC4,ENABLE);    
	
	memset(&nvic, 0, sizeof(nvic));
	// Init NVIC for Timer 4 Interrupt 
	nvic.NVIC_IRQChannel = TIM4_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;

	nvic.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&nvic);
	
	TIM_Cmd(TIM4, ENABLE); //Counter-Enable bit (CEN), set Timer 4
}

/******************************************************************************/
/*                                MAIN function                               */
/******************************************************************************/
int main (void) 
{
	GPIO_InitTypeDef gpio;
	char buffer[100];					//buffer for UART output
	int diff;									//difference of the counter between two rising edges
	int time;									//time difference between two rising edges
	
	uint16_t prevValue_SW0 = Bit_SET;						//previous value Schalter SW0
	
	TIM4_Config();						//Timer 4 Channel 4, configurate as Input Capture
	USART1_conf();						//USART 1 configuration
	
	//Write to USART1
	sprintf(buffer, "Uebung 3: Input Capture c 2021\n\r");
	USART_Send_String(buffer, strlen(buffer));
	
	memset(&gpio, 0, sizeof(gpio));
	GPIO_StructInit(&gpio);																// Create gpio structure
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	// Clock Enable Port A
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	// Clock Enable Port B
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;			//PB8 (LED0) as Output Push Pull
	gpio.GPIO_Pin = GPIO_Pin_8;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &gpio);
		
	gpio.GPIO_Mode = GPIO_Mode_IPU;					//PA0 (SW0) as Input Pull Up
	gpio.GPIO_Pin = GPIO_Pin_0;
	gpio.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
		
	while(1)
	{
		/*	on Pin PA0 (SW0) rising edge ?*/
		if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_RESET) && (prevValue_SW0 == Bit_SET))
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_8);
		sprintf(buffer, "Schalter eingeschaltet\n\r\n\r");
			USART_Send_String(buffer, strlen(buffer));				//protocol on UART1
			prevValue_SW0 = Bit_RESET;
		}
		/*	on Pin PA0 (SW0) falling edge ?*/
		else if((GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == Bit_SET) && (prevValue_SW0 == Bit_RESET))
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_8);
			sprintf(buffer, "Schalter ausgeschaltet\n\r");
			USART_Send_String(buffer, strlen(buffer));				//protocol on UART1
			prevValue_SW0 = Bit_SET;
		}
		if(flanke==true)
		{
			if (IC4ReadValue_new > IC4ReadValue_prev)
			{
				diff = IC4ReadValue_new - IC4ReadValue_prev;
			}
			else
			{
				diff = ((TIM4->ARR - IC4ReadValue_prev) + IC4ReadValue_new);
			}
			time = diff/16.4;				//calculation of the time difference
			sprintf(buffer, "Time: %dms\n\r", time);
			USART_Send_String(buffer, strlen(buffer));					//protocol on UART1
			flanke=false;					//only execute ones
		}
	}
}
