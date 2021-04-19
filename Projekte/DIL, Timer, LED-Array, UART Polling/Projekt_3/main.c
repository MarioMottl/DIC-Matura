/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2009 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   main.c							                                          */
/* Autor: 		Philipp Hasenzagl / Nicolas Meichenitsch                        */
/* Version: 	V1.00                                                           */
/* Date: 		11/04/2021                                               		      */
/* Description: Beispielprogramm für timer1, Taster am DIL-Adapter,           */
/*						  LED-Array auf der Euro-Platine und UART#1 (polling), 					*/
/*							realisiert als Realbeispiel "Auto mit Klima und Nebellicht"		*/
/******************************************************************************/

//Programm uses 8MHz HSE

#include "stm32f10x.h"
#include "stm32f10x_conf.h"
#include "string.h"

//prototypes
void usart_send_buffer(const char *str, int strlen);
void usart_send_string(const char *str);
void init_TIM();
void waitMS(int ms);

//global variable
int flag=0;
int AC=0;
int Nebel=0;

//timer1 and wait function
int timer_value = 0;

TIM_TimeBaseInitTypeDef tim1 =
{
	.TIM_Prescaler = 0x0008, //divides 8MHz by 8 = 1MHz
	.TIM_CounterMode = TIM_CounterMode_Up, //defines that the timer should count up
	.TIM_Period = 999, //counts every clk-cyle 1 up = 0-999 at 1MHz is 1ms
	.TIM_ClockDivision = TIM_CKD_DIV1, //could be divided again (not used)
	.TIM_RepetitionCounter = 0
};

NVIC_InitTypeDef tim1_nvic =
{
	.NVIC_IRQChannel = TIM1_UP_IRQn,
	.NVIC_IRQChannelPreemptionPriority = 2, //sets priority of the interrupt
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

void TIM1_UP_IRQHandler()
{
	if (TIM_GetFlagStatus(TIM1, TIM_IT_Update)) //when timer's ready do this
	{ 
		timer_value++; //count time_value up (one count takes one ms)
		TIM_ClearFlag(TIM1, TIM_IT_Update); //clear flag
	}
}

void init_TIM()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //EnableClock for timer1
	
	TIM_TimeBaseInit(TIM1, &tim1); //use tim1 configurations from above
	
	NVIC_Init(&tim1_nvic);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void waitMS(int ms)
{
	timer_value = 0; //reset timer_value
	while (timer_value < ms); //blocking process until "time is up"
}



//LED and Button define
GPIO_InitTypeDef LED_Euro_1 = 
{
	.GPIO_Pin = GPIO_Pin_6, //Pin definition
	.GPIO_Speed = GPIO_Speed_50MHz, //Frequency Speed
	.GPIO_Mode = GPIO_Mode_Out_PP // defines as Push/Pull
};

GPIO_InitTypeDef LED_Euro_2 = 
{
	.GPIO_Pin = GPIO_Pin_7, //Pin definition
	.GPIO_Speed = GPIO_Speed_50MHz, //Frequency Speed
	.GPIO_Mode = GPIO_Mode_Out_PP // defines as Push/Pull
};

GPIO_InitTypeDef taster_dill_1 = 
{
	.GPIO_Pin = GPIO_Pin_5,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU // defines as Input
};

GPIO_InitTypeDef taster_dill_2 = 
{
	.GPIO_Pin = GPIO_Pin_13,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU // defines as Input
};


//Interrupt define
EXTI_InitTypeDef exti0 =
{
	.EXTI_Line = EXTI_Line5, //defines which EXTI Line is used (is the same as the PIN-Number from the Input)
	.EXTI_Mode = EXTI_Mode_Interrupt, //sets mode as Interrupt
	.EXTI_Trigger = EXTI_Trigger_Falling, //trigegrs on Falling-Edge
	.EXTI_LineCmd = ENABLE
};

NVIC_InitTypeDef nvic_exti0 =
{
	.NVIC_IRQChannel = EXTI9_5_IRQn, //EXTI 5 is in a combined interrupt request handler therefore EXTI9_5_IRQn
	.NVIC_IRQChannelPreemptionPriority = 3, //sets priority of the interrupt
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

EXTI_InitTypeDef exti1 =
{
	.EXTI_Line = EXTI_Line13, //defines which EXTI Line is used (is the same as the PIN-Number from the Input)
	.EXTI_Mode = EXTI_Mode_Interrupt, //sets mode as Interrupt
	.EXTI_Trigger = EXTI_Trigger_Falling, //trigegrs on Falling-Edge
	.EXTI_LineCmd = ENABLE
};

NVIC_InitTypeDef nvic_exti1 =
{
	.NVIC_IRQChannel = EXTI15_10_IRQn, //EXTI 5 is in a combined interrupt request handler therefore EXTI9_5_IRQn
	.NVIC_IRQChannelPreemptionPriority = 3, //sets priority of the interrupt
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

//Interrupts
void EXTI9_5_IRQHandler()
{
	if (EXTI_GetFlagStatus(exti0.EXTI_Line)) //when Interrupt triggered then (when button pushed then)
	{
		if (Nebel == 0)
		{
			GPIO_SetBits(GPIOB, LED_Euro_1.GPIO_Pin);
			flag |= 1; //sets flag for main programm
			Nebel = 1;
		}
		else
		{
			GPIO_ResetBits(GPIOB, LED_Euro_1.GPIO_Pin);
			Nebel = 0;
			flag |= 2; //sets flag for main programm
		}

		EXTI_ClearFlag(exti0.EXTI_Line); //resets Interrupt flag
	}
}

void EXTI15_10_IRQHandler()
{
	if (EXTI_GetFlagStatus(exti1.EXTI_Line)) //when Interrupt triggered then (when button pushed then)
	{
		if (AC == 0)
		{
			GPIO_SetBits(GPIOB, LED_Euro_2.GPIO_Pin);
			flag |= 4; //sets flag for main programm
			AC = 1;
		}
		else
		{
			GPIO_ResetBits(GPIOB, LED_Euro_2.GPIO_Pin);
			AC = 0;
			flag |= 8; //sets flag for main programm
		}


		EXTI_ClearFlag(exti1.EXTI_Line); //resets Interrupt flag
	}
}

//UART initialize
void init_UART()
{
	USART_InitTypeDef usart_init;
	USART_ClockInitTypeDef usart_clkinit;
	GPIO_InitTypeDef gpio;
	
	// Peripheral Clock Enable
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN, ENABLE);
	
//GPIO Pins init
	//Tx (PA9)
	gpio.GPIO_Pin = GPIO_Pin_9;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpio);
	
	//Rx (PA10)
	gpio.GPIO_Pin = GPIO_Pin_10;
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpio);
	
	//USART Clock init
	usart_clkinit.USART_Clock = USART_Clock_Enable;
	usart_clkinit.USART_CPHA = USART_CPHA_2Edge;
	usart_clkinit.USART_CPOL = USART_CPOL_Low;
	usart_clkinit.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART1, &usart_clkinit);
	
	//USART init
	USART_StructInit(&usart_init);
	usart_init.USART_BaudRate = 115200;
	USART_Init(USART1, &usart_init);
	
	USART_Cmd(USART1, ENABLE);
	usart_send_string("UART initialize successful!\r\n");
	
}

//makes use easier (you only have to input the string initially and not the length too)
void usart_send_string(const char *str)
{
	usart_send_buffer(str, strlen(str));
}

//sends 1 character at a time (polling)
void usart_send_buffer(const char *str, int strlen)
{
	char *data = (char *)str;
	for(; strlen>0; strlen--, data++)
	{
		USART_SendData(USART1, *data);
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE)); //checks Tx USART-status
	}
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
}



int main()
{
	//enabling used GPIOS
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//initallize the Button and LED with their corrosponding GPIO
	GPIO_Init(GPIOB, &LED_Euro_1);
	GPIO_Init(GPIOC, &taster_dill_1);
	GPIO_Init(GPIOB, &LED_Euro_2);
	GPIO_Init(GPIOC, &taster_dill_2);
	
	/*
	Examples for Setting, Resetting and Reading Bits
	GPIO_SetBits(GPIOB, LED_Euro_1.GPIO_Pin);
	GPIO_ResetBits(GPIOB, LED_Euro_1.GPIO_Pin);
	GPIO_WriteBit(GPIOB, LED_Euro_1.GPIO_Pin, Bit_SET);
	GPIO_ReadInputDataBit(GPIO, taster_dill_1.GPIO_Pin);
	
	//toggle LED
	GPIO_WriteBit(GPIOB, LED_Euro_1.GPIO_Pin, ((~GPIO_ReadOutputDataBit(GPIOB, LED_Euro_1.GPIO_Pin))&1));
	*/
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //Enable AFIO for Interrupt
	AFIO->EXTICR[1] |= AFIO_EXTICR2_EXTI5_PC;
	AFIO->EXTICR[13/4] |= AFIO_EXTICR4_EXTI13_PC;
	
	//Initialize Interrupt
	EXTI_Init(&exti0);
	NVIC_Init(&nvic_exti0);
	EXTI_Init(&exti1);
	NVIC_Init(&nvic_exti1);

	init_TIM();
	init_UART();
	usart_send_string("Auto gestartet\r\n");
	while(1)
	{
		if(flag & 1)
		{
			usart_send_string("Nebellicht an\r\n");
			flag &= ~1;
		}
		else if(flag & 2)
		{
			usart_send_string("Nebellicht aus\r\n");
			flag &= ~2;
		}
		else if(flag & 4)
		{
			usart_send_string("Klima an\r\n");
			flag &= ~4;
		}
		else if(flag & 8)
		{
			usart_send_string("Klima aus\r\n");
			flag &= ~8;
		}
		usart_send_string("Auto faehrt\r\n");
		waitMS(1000); //wait a second
	}
}
