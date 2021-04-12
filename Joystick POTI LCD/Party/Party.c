/*
Name: Party.c
Author: Moritz Baldauf, Robert Radu

Description:
The two POTI's should be run by the ADC in Scanmode, 
The joystick should give an message to the LCD if used,
all changes on the POTI's are shown on the LCD in a table

Version: 1.0
*/
#include "Party.h"
#include "armv10_std.h"
#include "stm32f10x_conf.h"

/*Pins for Joystick
Left:PC6
Down:PC7
Up:PC8
Right:PC9
*/
//Initialise Pins for Joystick 
GPIO_InitTypeDef Joyleft =
{
	.GPIO_Pin = GPIO_Pin_6,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};
//Initialise Pins for Joystick 
GPIO_InitTypeDef JoyDown=			
{
	.GPIO_Pin = GPIO_Pin_7,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};
//Initialise Pins for Joystick 
GPIO_InitTypeDef JoyUp =		
{	
	.GPIO_Pin = GPIO_Pin_8,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};
//Initialise Pins for Joystick 
GPIO_InitTypeDef JoyRight =		
{
	.GPIO_Pin = GPIO_Pin_9,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};
//Internal peripheral Trigger
EXTI_InitTypeDef Joyleft_EXTI =		
{
	.EXTI_Line = EXTI_Line6,
	.EXTI_Mode = EXTI_Mode_Interrupt,
	.EXTI_Trigger = EXTI_Trigger_Falling,
	.EXTI_LineCmd = ENABLE
};
//Internal peripheral Trigger
EXTI_InitTypeDef Joydown_EXTI =	
{
	.EXTI_Line = EXTI_Line7,
	.EXTI_Mode = EXTI_Mode_Interrupt,
	.EXTI_Trigger = EXTI_Trigger_Falling,
	.EXTI_LineCmd = ENABLE
};
//Internal peripheral Trigger
EXTI_InitTypeDef Joyup_EXTI =		
{
	.EXTI_Line = EXTI_Line8,
	.EXTI_Mode = EXTI_Mode_Interrupt,
	.EXTI_Trigger = EXTI_Trigger_Falling,
	.EXTI_LineCmd = ENABLE
};
//Internal peripheral Trigger
EXTI_InitTypeDef Joyright_EXTI =		
{
	.EXTI_Line = EXTI_Line9,
	.EXTI_Mode = EXTI_Mode_Interrupt,
	.EXTI_Trigger = EXTI_Trigger_Falling,
	.EXTI_LineCmd = ENABLE
};
//Vector interrupt control for Pins 5-9
NVIC_InitTypeDef Joy_NVIC = 			
{
	.NVIC_IRQChannel = EXTI9_5_IRQn,
	.NVIC_IRQChannelPreemptionPriority = 3,
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

//variable for value from ADC1 channel 9
__IO uint16_t Ch_9 = 0;		
//variable for value from ADC1 channel 14
__IO uint16_t Ch_14 = 0;	
//string buffer for UART transfer
char buffer[100];			

//Protoype for ADC programm
void ch14(void);		

void EXTI9_5_IRQHandler()
{
	//Joystick up
	if(EXTI_GetFlagStatus(Joyup_EXTI.EXTI_Line) == SET)			
	{
		//Print Like on UART and LCD
		lcd_init();
		lcd_set_cursor(0,0);
		sprintf(&buffer[0],"Like\r\n");
		lcd_put_string(&buffer[0]);
		USART_SendString(USART1,buffer);
		EXTI_ClearFlag(Joyup_EXTI.EXTI_Line);
	};
	//Joystick left
	if(EXTI_GetFlagStatus(Joydown_EXTI.EXTI_Line) == SET)		
	{
		//Print Dislike on UART and LCD
		lcd_init();	
		lcd_set_cursor(0,0);
		sprintf(&buffer[0],"Dislike\r\n");
		lcd_put_string(&buffer[0]);
		USART_SendString(USART1,buffer); 
		EXTI_ClearFlag(Joydown_EXTI.EXTI_Line);
	};
	//Joystick right
	if(EXTI_GetFlagStatus(Joyright_EXTI.EXTI_Line) == SET)	
	{
		//Print Skip on UART and LCD
		lcd_init();
		lcd_set_cursor(0,0);
		sprintf(&buffer[0],"Skip\r\n");
		lcd_put_string(&buffer[0]);
		USART_SendString(USART1,buffer);
		EXTI_ClearFlag(Joyright_EXTI.EXTI_Line);
	};
	//Joystick left
	if(EXTI_GetFlagStatus(Joyleft_EXTI.EXTI_Line) == SET)		
	{
		//Print Back on UART and LCD
		lcd_init();	
		lcd_set_cursor(0,0);
		sprintf(&buffer[0],"Back\r\n");
		lcd_put_string(&buffer[0]);
		USART_SendString(USART1,buffer);
		EXTI_ClearFlag(Joyleft_EXTI.EXTI_Line);
	};
	
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);	
}



/*
  Synchronise inputs before 
	processing input-data using 
	a semaphore rendezvous
*/
 
void ch14 () 
{
	int perc = 0;
	int old = 0;
	for(;;) {
		//get data from ADC1 channel14		
		Ch_14 = ADC1ConvertedValues[1];						
		//get data from ADC1 channel9
		Ch_9 = ADC1ConvertedValues[0];						
		
		//calculate combined percentage of both variable resistors
		perc = ((Ch_14 + Ch_9)*100)/8190;					
		
		//Only print if Changes																					
		//Potis are not totally exact so there is a tolerance for changes
		if(!((perc >= old-1)&&(perc <= old+1)))	
		{
			old = perc;
			//prepare string buffer for UART
			sprintf(&buffer[0],"Lautstaerke:%d\r\n",perc);
			//send string via UART
			USART_SendString(USART1,buffer);
			lcd_init();
			lcd_clear();
			//shows procentage on LCD screen
			lcd_bargraphXY(1,0,perc);
		}
  }
}

int main (void) {
	//Enalbe Peripheral Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	//Initilaice GPIO Port and Pins
	GPIO_Init(GPIOC, &Joyleft);
	GPIO_Init(GPIOC, &JoyRight);
	GPIO_Init(GPIOC, &JoyDown);
	GPIO_Init(GPIOC, &JoyUp);

	//Initilaice External Interrupt for IE-Event
	AFIO->EXTICR[6/4] |= (AFIO_EXTICR2_EXTI6_PC);
	AFIO->EXTICR[7/4] |= (AFIO_EXTICR2_EXTI7_PC);
	AFIO->EXTICR[8/4] |= (AFIO_EXTICR3_EXTI8_PC);
	AFIO->EXTICR[9/4] |= (AFIO_EXTICR3_EXTI9_PC);
	EXTI_Init(&Joyup_EXTI);
	EXTI_Init(&Joydown_EXTI);
	EXTI_Init(&Joyright_EXTI);
	EXTI_Init(&Joyleft_EXTI);

	NVIC_Init(&Joy_NVIC);
	
	//load DMA config
	DMA_Config();
	//initialize ADC1
	init_uart();//initialize UART
	ADC1_Init();
	//Clear Console
	USART_SendData(USART1,12);
	USART_SendString(USART1,
	"\r\n***************\r\n    Musikanlage   \r\n***************\r\n");
	SystemCoreClockUpdate();
	ch14();//start programm for potis
	while(1);
}
