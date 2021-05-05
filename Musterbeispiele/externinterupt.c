#include "includes.h"

// STM32F10x_StdPeriph_Lib_V3.5.0/Project/STM32F10x_StdPeriph_Examples/EXTI/EXTI_Config 

int falling_edges;

void EXTI1_IRQHandler(void)//ISR
{
	EXTI_ClearITPendingBit(EXTI_Line1); //Pending bit EXT1 rücksetzen (Sonst wird die ISR immer wiederholt)
	falling_edges++;    // Fallende Flanke an PA1 erkannt
	return;
}  

void EXTI9_5_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
		// handle EXTI Line 7 IRQ
	}
    else if(EXTI_GetITStatus(EXTI_Line9 != RESET))
    {
        EXTI_ClearITPendingBit(EXTI_Line9);
		// handle EXTI Line 9 IRQ
    }
    return;
}

void EXTI15_10_IRQHandler(); //for EXTI Line 10..15

void EXTI1_Config(void) 
{
	GPIO_InitTypeDef gpio;	
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef nvic;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA Clock Enable
	
	GPIO_StructInit(&gpio);// Create gpio struct and fill it with defaults
	gpio.GPIO_Mode = GPIO_Mode_IPU; // Configure PA1 to input Pull UP Mode
	gpio.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOA, &gpio);
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //AFIOEN  - Clock enable	
	EXTI_DeInit();
    	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); /* Connect EXTI Line 1 to PA.01 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource7); /* Connect EXTI Line 7 to PB.07 pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource9); /* Connect EXTI Line 9 to PC.09 pin */
	
	EXTI_StructInit(&EXTI_InitStruct);
	EXTI_InitStruct.EXTI_Line =  EXTI_Line1; // Initialise EXTI Line 1
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // Exti Triggers ISR
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; // Exti triggers on Falling Input Edge
	EXTI_InitStruct.EXTI_LineCmd = ENABLE; // Exti Line is enabled
	EXTI_Init(&EXTI_InitStruct); /* save initialisation */

	EXTI_InitStruct.EXTI_Line = EXTI_Line7; // init EXTI to line 7
	EXTI_Init(&EXTI_InitStruct);
	EXTI_InitStruct.EXTI_Line = EXTI_Line9; // init EXTI to line 9
	EXTI_Init(&EXTI_InitStruct);
    
	
	EXTI_ClearITPendingBit(EXTI_Line1); //EXTI_clear_pending: Das Auslösen auf vergangene Vorgänge nach	dem enablen verhindern
	//EXTI->IMR |= (0x01 << 1);   // Enable Interrupt EXTI1-Line 
		
	// Init NVIC for EXTI1 Interrupt 
	nvic.NVIC_IRQChannel = EXTI1_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&nvic);   

    // Init NVIC for EXTI9_5 Interrupt / enable EXTI9_5 interrupt -> same interrupt handler for line 5 - 9
    nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_Init(&nvic);
}
