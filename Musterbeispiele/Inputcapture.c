#include "includes.h"

// STM32F10x_StdPeriph_Lib_V3.5.0/Project/STM32F10x_StdPeriph_Examples/InputCapture/main.c


__IO uint16_t IC4ReadValue_new = 0, IC4ReadValue_prev = 0;
__IO uint16_t CaptureNumber = 0;

bool flanke;

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