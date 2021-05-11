#include "stm32f10x.h" // Device header 
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC 
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO 
#include "stm32f10x_usart.h" // Keil::Device:StdPeriph Drivers:USART 
#include "misc.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void uart_init();

void USART_put_byte(char data);
void USART_put_bytes(const void *data, size_t len);

void USART_put_byte(char data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET){}
    USART_SendData(USART1, data);
}

void USART_put_bytes(const void *data, size_t len)
{
    char *bytes = (char*)data;
    for(int i = 0; i<len; i++)
    {
       USART_put_byte(bytes[i]); 
    }
}


void USART1_IRQHandler(void)
{
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    char input;
    input = USART_ReceiveData(USART1);
    return;
}

void uart_init()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

    USART_InitTypeDef usart;
    USART_ClockInitTypeDef usart_clock;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    // nvic
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    nvic.NVIC_IRQChannel = USART1_IRQn;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    nvic.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&nvic);

    //RX
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
    
    //TX
    gpio.GPIO_Pin = GPIO_Pin_9;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    USART_DeInit(USART1);
    usart_clock.USART_Clock = USART_Clock_Disable;
    usart_clock.USART_LastBit = USART_LastBit_Disable;
    usart_clock.USART_CPHA = USART_CPHA_2Edge;
    usart_clock.USART_CPOL = USART_CPOL_Low;
    USART_ClockInit(USART1,&usart_clock);

    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &usart);

    USART_Init(USART1, &usart);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    USART_Cmd(USART1, ENABLE);

}

int main()
{
    uart_init();
    char *huan = "Huan\0";
    while(true){
        USART_put_bytes(huan,strlen(huan));
    }
    return 0;
}
