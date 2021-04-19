#include "ue4.h"

void InitGpio(void)
{
	GPIO_InitTypeDef gpio;
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	gpio.GPIO_Pin = GPIO_Pin_8;
  GPIO_Init(GPIOA, &gpio);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	gpio.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOD, &gpio);
}

void TIM3_Config(void)
{
	GPIO_InitTypeDef gpio;
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	NVIC_InitTypeDef nvic;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOB Clock Enable
	GPIO_StructInit(&gpio);// Create gpio struct and fill it with defaults
	gpio.GPIO_Mode = GPIO_Mode_AF_PP; // PB8(=LED0, TIM4_CH3) in AF -Push Pull
	gpio.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &gpio);
	
	//Tx = 1s; T_INT = 125ns, ARR: 0xFFF => PSC: 0x78
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // Clock Enable Timer 4
	TIM_DeInit(TIM3);
	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	//Auto-Reload Wert (ARR) = Maximaler Zaehlerstand des Upcounters
	TIM_TimeBase_InitStructure.TIM_Period = 0xFFFF;		
	//Prescaler (Taktverminderung)
	int Tx = 2;
	double T_INT = 0.000000125;
	TIM_TimeBase_InitStructure.TIM_Prescaler = Tx / (T_INT * 0xFFFF) - 1;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBase_InitStructure);
	
	/* Init Output Compare Channels */
	/*TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStruct.TIM_Pulse = TIM_TimeBase_InitStructure.TIM_Period / 2;*/
	
	/* Compare Value*/
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(TIM3,&TIM_OCInitStruct); /* save initialisation */
	TIM_ITConfig (TIM3, TIM_IT_Update,ENABLE); // Timer 4 Update Interrupt Enable
	
	// Init NVIC for Timer 1 Update Interrupt
	nvic.NVIC_IRQChannel = TIM3_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
	TIM_Cmd(TIM3, ENABLE); //Counter-Enable bit (CEN) Timer 4 setzen
}


///configuration for USART2
//
//USART2:
//Rx Pin 			....	PA3
//Tx Pin			.... 	PA2
//
//baudrate		....	115200
//word length	....	8 bit
//parity bits	.... 	none
//stop bit		....	1
void InitUsart2(void)
{
	GPIO_InitTypeDef gpio;  
	USART_ClockInitTypeDef usartclock; 
	USART_InitTypeDef usart;
	
	SystemCoreClockUpdate();
	USART_DeInit(USART2);
	
	//enable all GPIO and USART clocks needed for USART2  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
	
	GPIO_StructInit(&gpio);
	
	//set PA2 to alternate function push pull (Tx)  
	gpio.GPIO_Mode = GPIO_Mode_AF_PP;  
	gpio.GPIO_Pin = GPIO_Pin_2;  
	gpio.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio); 
	
	//set PA3 to input floating (Rx)  
	gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	gpio.GPIO_Pin = GPIO_Pin_3;  
	GPIO_Init(GPIOA, &gpio); 
	
	//init USART2 clock  
	USART_ClockStructInit(&usartclock);  
//	memset(&usartclock, 0, sizeof(usartclock));
	usartclock.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInit(USART2, &usartclock); 
	
	
	//create usart struct and init USART2 to 115200 baud  
	USART_StructInit(&usart);  
	usart.USART_BaudRate = 115200;
	usart.USART_WordLength = USART_WordLength_8b; 
	usart.USART_StopBits = USART_StopBits_1; 
	usart.USART_Parity = USART_Parity_No ; 
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USART2, &usart); 
 

	//enable USART2  
	USART_Cmd(USART2, ENABLE); 
	//sysclock = SystemCoreClock;
}

//send string to USARTx
//parameter:
//USART_TypeDef *USARTx....selected USART interface
//char *str....string to transmit
void UartPutString(USART_TypeDef *USARTx, char *str) 
{  
	while (*str)  
	{     
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		//while (!(USARTx->SR & USART_SR_TXE));
		//uart_put_char(USARTx, *str++);  
		USART_SendData(USARTx, *str++);  
	} 
} 
