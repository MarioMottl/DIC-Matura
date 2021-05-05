#include "includes.h"

void init_uart(void)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;
    USART_ClockInitTypeDef usartclock;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
    
    // USART1 RX (PA10 in floating)
    // USART2 RX (PA3)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    // USART1 TX (PA9 out PP)
    // USART2 TX (PA2)
    gpio.GPIO_Pin= GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    usartclock.USART_Clock = USART_Clock_Enable;
    usartclock.USART_LastBit = USART_LastBit_Disable;
    usartclock.USART_CPOL = USART_CPOL_Low;
    usartclock.USART_CPHA = USART_CPHA_1Edge;
    USART_ClockInit(USART1, &usartclock);

    usart.USART_BaudRate = 9600;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &usart);

    USART_Cmd(USART1, ENABLE);

}
