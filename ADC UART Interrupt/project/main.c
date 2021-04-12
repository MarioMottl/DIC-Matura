/*
================================================================
Name: main.c
Author: Maximilian Roll & Martin Platajs
Project: ADC Request

Description:
This File handels USART-requests. The main feature ist
to return the ADC-value of a variable resistor.

Version: 1.0
================================================================
*/

/* ----------------------------- Includes ------------------------*/

#include "config.h"

/* ----------------------------- Varibales -----------------------*/

static char recieved;		//variable for value recieved from USART
static uint16_t value = 0;	//variable for value from ADC1 channel 14
char buffer[20];			//buffor for response messages


/*------------------------------------------------------------------
  Interrupt service routine for USART RX, checks for USART-requests
 *-----------------------------------------------------------------*/
void USART1_IRQHandler()	
{
	recieved = USART_ReceiveData(USART1);
    if(recieved == 'y' && ADC_GetFlagStatus(ADC1,ADC_FLAG_STRT) == RESET){
			sprintf(buffer,"\r\nADC Conversion not triggered yet!\r\nADC-Value: %d\r\n",value);
			USART_SendString(USART1, buffer);
		}
		else{
		switch(recieved){
		
		case 'y':
			sprintf(buffer,"\r\nADC-Value: %d\r\n",value);
			USART_SendString(USART1, buffer);
			break;
    
		case 'c':
			USART_SendData(USART1,12);
			break;
		
		case 'm':
			sprintf(buffer,"\r\n-> Press y to get ADC Value\r\n-> Press c to clear Console\r\n-> Press m to display above mentioned Commands\r\n");
			USART_SendString(USART1, buffer);
			break;
		}
	}
}


/*--------------------------------------------------------------------------
  Interrupt service routine for ADC1 (and ADC2), saves ADC-value to "value"
 *-------------------------------------------------------------------------*/
void ADC1_2_IRQHandler()	//interrupt service routine for ADC1 (and ADC2)
{
	value = ADC_GetConversionValue(ADC1);
}


/*------------------------------------------------------------------
  Main; initialize peripherals; send welcome text to USART1
 *-----------------------------------------------------------------*/
int main(){

    RCC_Configuration();	//load RCC Configuration
    NVIC_Configuration();	//load NVIC Configuration
    GPIO_Configuration();	//load GPIO Configuration
	USART_Configuration();	//load USART Configuration
	EXTI_Configuration();	//load EXTI Configuration
	ADC_Configuration();	//load ADC Configuration

	USART_SendData(USART1,12);	//clear console
	USART_SendString(USART1,"\r\n*****************\r\n*  ADC-Request  *\r\n*****************\r\n\r\n");	//welcome text
	USART_SendString(USART1,"press x to start\r\n\r\n");	//press x for further execution
	while(recieved != 'x'){}								//wait for x to be pressed
	USART_SendString(USART1,"-> Make sure ADC is triggered!\r\n");		// remove LED/Switches circuit board
	USART_SendString(USART1,"-> Press y to get ADC Value\r\n");			// for further information see the protocol
	USART_SendString(USART1,"-> Press c to clear Console\r\n");
	USART_SendString(USART1,"-> Press m to display above mentioned Commands\r\n");

	while(1);
}
