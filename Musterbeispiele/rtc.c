#include "includes.h"

__IO uint32_t TimeDisplay = 0;

void RTC_IRQHandler(void)
{
    if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
    {
        /* Clear the RTC Second interrupt */
        RTC_ClearITPendingBit(RTC_IT_SEC);

        /* Toggle LED1 */
        STM_EVAL_LEDToggle(LED1);

        /* Enable time update */
        TimeDisplay = 1;

        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
}

void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the RTC Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

void RTC_Configuration(void)
{
    /* Enable PWR and BKP clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    /* Allow access to BKP Domain */
    PWR_BackupAccessCmd(ENABLE);
    /* Reset Backup Domain */
    BKP_DeInit();
    /* Enable LSE */
    RCC_LSEConfig(RCC_LSE_ON);
    /* Wait till LSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {}
    /* Select LSE as RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    /* Enable RTC Clock */
    RCC_RTCCLKCmd(ENABLE);
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Enable the RTC Second */
    RTC_ITConfig(R, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Set RTC prescaler: set RTC period to 1sec */
    RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

/**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @param  None
  * @retval Current time RTC counter value
  */
uint32_t Time_Regulate(void)
{
    uint32_t Tmp_HH = 0xFF, Tmp_MM = 0xFF, Tmp_SS = 0xFF;

    printf("\r\n==============Time Settings=====================================");
    printf("\r\n  Please Set Hours");

    while (Tmp_HH == 0xFF)
    {
    Tmp_HH = USART_Scanf(23);
    }
    printf(":  %d", Tmp_HH);
    printf("\r\n  Please Set Minutes");
    while (Tmp_MM == 0xFF)
    {
    Tmp_MM = USART_Scanf(59);
    }
    printf(":  %d", Tmp_MM);
    printf("\r\n  Please Set Seconds");
    while (Tmp_SS == 0xFF)
    {
    Tmp_SS = USART_Scanf(59);
    }
    printf(":  %d", Tmp_SS);

    /* Return the value to store in RTC counter register */
    return((Tmp_HH*3600 + Tmp_MM*60 + Tmp_SS));
}

/**
  * @brief  Adjusts time.
  * @param  None
  * @retval None
  */
void Time_Adjust(void)
{
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Change the current time */
    RTC_SetCounter(Time_Regulate());
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
}

uint8_t USART_Scanf(uint32_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};

    while (index < 2)
    {
    /* Loop until RXNE = 1 */
    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_RXNE) == RESET)
    {}
    tmp[index++] = (USART_ReceiveData(EVAL_COM1));
    if ((tmp[index - 1] < 0x30) || (tmp[index - 1] > 0x39))
    {
        printf("\n\rPlease enter valid number between 0 and 9");
        index--;
    }
    }
    /* Calculate the Corresponding value */
    index = (tmp[1] - 0x30) + ((tmp[0] - 0x30) * 10);
    /* Checks */
    if (index > value)
    {
    printf("\n\rPlease enter valid number between 0 and %d", value);
    return 0xFF;
    }
    return index;
}

void Time_Show(void)
{
    printf("\n\r");

    /* Infinite loop */
    while (1)
    {
        /* If 1s has been elapsed */
        if (TimeDisplay == 1)
        {
            /* Display current time */
            Time_Display(RTC_GetCounter());
            TimeDisplay = 0;
        }
    }
}

void Time_Display(uint32_t TimeVar)
{
    uint32_t THH = 0, TMM = 0, TSS = 0;
    
    /* Reset RTC Counter when Time is 23:59:59 */
    if (RTC_GetCounter() == 0x0001517F)
    {
        RTC_SetCounter(0x0);
        /* Wait until last write operation on RTC registers has finished */
        RTC_WaitForLastTask();
    }
    
    /* Compute  hours */
    THH = TimeVar / 3600;
    /* Compute minutes */
    TMM = (TimeVar % 3600) / 60;
    /* Compute seconds */
    TSS = (TimeVar % 3600) % 60;

    printf("Time: %0.2d:%0.2d:%0.2d\r", THH, TMM, TSS);
}