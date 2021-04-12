/*adc.h
*   Julian Strunz
*	2020/2021
*
* Function:
* In diesem File findet man alle Includes und Datentypen die in uart.c benötigt werden
*/

#ifndef UART_H_INCLUDED
#define UART_H_INCLUDED

#include 	"stm32f10x.h"               //standard library 
#include 	"stm32f10x_rcc.h"			//RCC Clock library
#include 	"stm32f10x_gpio.h"			//GPIO I/O library
#include 	"stm32f10x_usart.h"			//USART communication library
#include 	"string.h"					//strcat()
#include 	"stdio.h" 					//strcat()

/* ------------------------- Prototypes -------------------------------*/ 

//uart initialisieren
void INIT_UART2(void);
//send char via selected USART port
void Uart_Put_Char(USART_TypeDef *USARTx, char ch);
//send string via selected USART port
void Uart_Put_String(USART_TypeDef *USARTx, char *string) ;

#endif
