#include "includes.h"

// STM32F10x_StdPeriph_Examples/TIM/TIM1_Synchro/main.c

void initTim1(void)
{    
    TIM_OCInitTypeDef tclock = {};
    TIM_TimeBaseInitTypeDef tim;
    
    tim.TIM_Prescaler = 0x0072; // divides 72MHz Clock by 72 => 1MHz
    tim.TIM_CounterMode = TIM_CounterMode_Up;
    tim.TIM_Period = 999; // every 1000 counter valuescause interrupt => 1ms
    tim.TIM_ClockDivision = TIM_CKD_DIV1;
    tim.TIM_RepetitionCounter = 0;

    tclock.TIM_OCMode = TIM_OCMode_PWM2;
    tclock.TIM_OutputState = TIM_OutputState_Enable;
    tclock.TIM_OutputNState = TIM_OutputNState_Enable;
    tclock.TIM_Pulse = 127;
    tclock.TIM_OCPolarity = TIM_OCPolarity_Low;
    tclock.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    tclock.TIM_OCIdleState = TIM_OCIdleState_Set;
    tclock.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &tclock);

    TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Gated);
    TIM_SelectInputTrigger(TIM1, TIM_TS_ITR0);
    
    return;
}