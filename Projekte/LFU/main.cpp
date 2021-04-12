#include <stm32f10x.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_usart.h>
#include <stm32f10x_tim.h>
#include <misc.h>
#include <string.h>
#include <stdio.h>

// initializes uart 1
void uart1_init(uint32_t baudrate)
{
    USART_InitTypeDef usart;
    USART_ClockInitTypeDef usart_clock;
    GPIO_InitTypeDef gpio;

    // enable clock for GPIOA & GPIOB
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE );

    // USART1 TX (PA9 out PP)
    gpio.GPIO_Pin= GPIO_Pin_9;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
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
    usart.USART_BaudRate = baudrate;
	usart.USART_WordLength = USART_WordLength_8b;	 
	usart.USART_StopBits = USART_StopBits_1;
	usart.USART_Parity = USART_Parity_No;
	usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(USART1, &usart);

    // enable USART1
    USART_Cmd(USART1, ENABLE);
}

// sends a byte through the uart1
void USART1_put_byte(uint8_t data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
    USART_SendData(USART1, data);
}

// sends a string through the uart1
void USART1_put_bytes(const void *data, uint32_t len)
{
    const uint8_t *bytes = (const uint8_t*)data;
    for(uint32_t i=0; i<len; i++)
        USART1_put_byte(bytes[i]);
}

// waits 10us
void wait_10us(void)
{
    uint8_t i, j;
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 10; j++)
            __NOP(); 
    }
}

// its showtime
int main()
{
    // init PB0
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;                // input floating mode
    gpio.GPIO_Speed = GPIO_Speed_50MHz;                     // gpio frequency 50MHz
    gpio.GPIO_Pin   = GPIO_Pin_0;                           // Pin: PB0
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   // Enable pin
    GPIO_Init(GPIOB, &gpio);                                // Init pin

    // init uart
    uart1_init(9600);
    for(;;)
    {
        int periods = 0;    // 10us periods
        // one period
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0) {wait_10us(); periods++;}  // low-period
        while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 1) {wait_10us(); periods++;}  // high-period

        const int time_us = 10 * periods;                   // 1 period = 10us
        const float time_s = ((float)time_us) / 1000000.0f; // 1s = 1000000us
        const int frequency = (int)(1.0f / time_s);         // f = 1 / T

        char str[64];
        snprintf(str, sizeof(str), "Frequency: %dHz\n\r", frequency);   // compile output string
        USART1_put_bytes(str, strlen(str));                             // send output string
    }
}
