#include "includes.h"

//STM32F10x_StdPeriph_Examples/USART/Interrupt/main.c

void USART1_put_byte(uint8_t data);
void USART1_put_bytes(const void *data, size_t len);

//Wichtig: Wenn UART Interrupt gesteuert verwendet werden soll
void USART1_IRQHandler(void)
{
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    char input;
    input = USART_ReceiveData(USART1);
    return;
}

void init()
{
    // Definition der Wichtigen Variablen für die UART Definition
    USART_InitTypeDef usart;
    USART_ClockInitTypeDef usart_clock;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    // USART1 RX (PA10 in floating)
    // USART2 RX (PA3)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    // USART1 TX (PA9 out PP)
    // USART2 TX (PA2)
    gpio.GPIO_Pin= GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

     // init USART
    USART_DeInit(USART1);

    // init USART Clock
    usart_clock.USART_Clock = USART_Clock_Disable;
	usart_clock.USART_CPOL = USART_CPOL_Low;
	usart_clock.USART_CPHA = USART_CPHA_2Edge;
	usart_clock.USART_LastBit = USART_LastBit_Disable;
	USART_ClockInit(USART1, &usart_clock);

    // Init USART
    usart.USART_BaudRate = 9600;
	usart.USART_WordLength = USART_WordLength_8b;	 
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &usart);

    // init USART Interrupt
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);

    // enable USART1
    USART_Cmd(USART1, ENABLE);
    return;
}

//puts one byte of data out
void USART1_put_byte(uint8_t data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
    USART_SendData(USART1, data);
}

//puts len bytes out
void USART1_put_bytes(const void *data, size_t len)
{
    const uint8_t *bytes = (const uint8_t*)data;
    for(size_t i=0; i<len; i++)
        USART1_put_byte(bytes[i]);
}