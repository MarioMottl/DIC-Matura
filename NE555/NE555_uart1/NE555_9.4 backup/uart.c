/*	uart.c
* 	Julian Strunz
*	2020/2021
*
* Function:
* Alle UART Funktionen werden in diesem File beschrieben
* sowohl die Initialisierung als auch die Ausgabe
*/

/* -------------------------- Includes -------------------------------*/ 
#include "uart.h"


/* -------------------------- Functions -------------------------------*/


//send char to USARTx
//parameter:
//USART_TypeDef *USARTx....selected USART interface
//char ch....character to transmit
void Uart_Put_Char(USART_TypeDef *USARTx, char ch) 
{   
	while (!(USARTx->SR & USART_SR_TXE)); //Data Transmit Register leer?   
	USARTx->DR = (ch&0xFF); //byteweise reinschieben 
}

//send string to USARTx
//parameter:
//USART_TypeDef *USARTx....selected USART interface
//char *str....string to transmit
void Uart_Put_String(USART_TypeDef *USARTx, char *str) 
{   
	while (*str)  
	{     
		Uart_Put_Char (USARTx, *str++);  
		//USART_SendData (USARTx, *str++);
	} 
} 

///configuration for USART2
//
//USART2:
//Rx Pin 		....	PA3
//Tx Pin		.... 	PA2
//
//baudrate		....	9600
//word length	....	8 bit
//parity bits	.... 	none
//stop bit		....	1
void INIT_UART2(void)
{
	GPIO_InitTypeDef gpio;  
	USART_ClockInitTypeDef usartclock; 
	USART_InitTypeDef usart; 
	NVIC_InitTypeDef nvic;
	//uint32_t sysclock;
	
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
	usartclock.USART_CPHA = USART_CPHA_2Edge;
	USART_ClockInit(USART2, &usartclock); 
	
	
	//create usart struct and init USART2 to 115200 baud  
	USART_StructInit(&usart);  
	usart.USART_BaudRate = 9600;
	usart.USART_WordLength = USART_WordLength_8b; 
	usart.USART_StopBits = USART_StopBits_1; 
	usart.USART_Parity = USART_Parity_No ; 
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 
	USART_Init(USART2, &usart); 
 

	//init NVIC for USART2 RXNE   
	nvic.NVIC_IRQChannel = USART2_IRQn;  
	nvic.NVIC_IRQChannelCmd = ENABLE; 
	nvic.NVIC_IRQChannelPreemptionPriority = 0;  
	nvic.NVIC_IRQChannelSubPriority = 2;  
	NVIC_Init(&nvic);  
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	//enable USART2  
	USART_Cmd(USART2, ENABLE); 
}
