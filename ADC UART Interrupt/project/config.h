/*
================================================================
Name: config.h
Author: Maximilian Roll & Martin Platajs
Project: ADC Request

Description:
Header file for initialization code, include and defines

Version: 1.0
================================================================
*/


#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED


/* ----------- Includes -----------*/
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stm32f10x_usart.h>
#include <stm32f10x_adc.h>
#include "stm32f10x_exti.h"


/* ----------- Prototypes -----------*/
static void RCC_Configuration(void);
static void NVIC_Configuration(void);
static void GPIO_Configuration(void);
static void USART_Configuration(void);
static void EXTI_Configuration(void);
static void ADC_Configuration(void);
static void USART_SendString (USART_TypeDef *USARTx, char *str);


/*--------------------------------------------
  Reset and Clock Control, RCC initialization
 *-------------------------------------------*/
void RCC_Configuration(void){
  //enable clock for GPIOA, GPIOB, AFIO, ADC1 and USART1
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | 
                        RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1 |
                         RCC_APB2Periph_USART1, ENABLE);
}


/*----------------------------------------------------------
  Nested Vectored Interrupt Controller, NVIC initialization
 *---------------------------------------------------------*/
void NVIC_Configuration(void){
  //Init NVIC for USART1; RX -> Interrupt (Request handling)
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStructure);

  //Init NVIC for ADC1_2; Conversion complete -> Interrupt (save value)
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/*--------------------------------------------------
  General Purpose Input Output, GPIO initialization
 *-------------------------------------------------*/
void GPIO_Configuration(void){
    GPIO_InitTypeDef GPIO_InitStructure;
     
    // Set PA9 to alternate function push pull (Tx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Set PA10 to input floating (Rx)
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Set PB11 to input floating (external interrupt)
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Set PC4 to analog input (ADC1 channel 14)
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	  GPIO_Init(GPIOC, &GPIO_InitStructure);
}


/*----------------------------------------
  External Interrupt, EXTI initialization
 *---------------------------------------*/
 void EXTI_Configuration(void){
  EXTI_InitTypeDef EXTI_InitStructure;
  //Set external Interrupt Line to PB11
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
  //setup external interrupt for ADC Conversion start
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;   //Mode: event (ADC1)
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //rising edge
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;   //Line 11 (PB11)
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
 } 


/*------------------------------------------------------------------------------
  Universal Synchronous/Asynchronous Receiver Transmitter, USART initialization
 *-----------------------------------------------------------------------------*/
void USART_Configuration(void){
	USART_InitTypeDef USART_InitStructure;
  //Init USART1 to 9600 baud
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_ClockInitTypeDef Usart_ClockInitStructure;
  USART_ClockInit(USART1, &Usart_ClockInitStructure);
    
    USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}


/*----------------------------------------
  Analog-Digital-Converter, ADC initialization
 *---------------------------------------*/
void ADC_Configuration(void){
   ADC_InitTypeDef ADC_InitStructure;
   //Init ADC1 with Continous Conversion Mode and external Trigger
	 ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
   ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //Continous Conversion Mode
   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO;  //external trigger line 11
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   ADC_InitStructure.ADC_NbrOfChannel = 1;
   ADC_Init(ADC1, &ADC_InitStructure);

   ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_28Cycles5);
   ADC_ExternalTrigConvCmd(ADC1, ENABLE);
   ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
   ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);					//ADC Calibration
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
}

/*----------------------------------------------------------------
  Sends a string, character by character, over the specified UART
 *---------------------------------------------------------------*/
void USART_SendString(USART_TypeDef *USARTx, char *str)
{
	while (*str) {
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
		USART_SendData(USARTx, *str++);
	}
}

#endif //CONFIG_H_INCLUDED
