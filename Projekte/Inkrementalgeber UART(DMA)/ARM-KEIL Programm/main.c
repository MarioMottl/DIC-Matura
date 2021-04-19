/**
 * \brief			DIC#3 Project - Standard Peripheral Library
 * \file 			main.c
 * \date			10.04.2021
 * \version		1.2
 * \author		Clemens Marx
 * \author		Jakob Pachtrog
 * \copyright HTL - Hollabrunn
 *
 * \details
 * Inkrementalgeber: 	GPIO C8		Event
 *										GPIO C9		Direction
 *
 * UART:							GPIO A9		Tx
 * 										GPIO A10	Rx
 *
 */

/**
 *  \}
 *	\defgroup Includes
 *  \{
 */
#include "stm32f10x.h"                  /// Device header
#include "stm32f10x_conf.h"							/// StdPeriph Driver
#include "stdio.h"
#include "string.h"

/**
 *  \}
 *	\defgroup Defines
 *  \{
 */

#define MAX_VOLUME	20

/**
 *  \}
 *	\defgroup Variables Global varibles
 *  \{
 */
char uartDR = 0;
int volume = 10;
int oldVolume = 0;

/**
 *	\}
 *	\defgroup Variables Incremental Encoder settings
 *  \{
 */

/**
 * \brief           Incremental Encoder - GPIO Port: Event
 */
GPIO_InitTypeDef iE_Event =
{
	.GPIO_Pin = GPIO_Pin_8,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};

/**
 * \brief           Incremental Encoder - GPIO Port: Dircetion
 */
GPIO_InitTypeDef iE_Direction =
{
	.GPIO_Pin = GPIO_Pin_9,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IPU
};

/**
 * \brief           Incremental Encoder - External Interrupt
 */
EXTI_InitTypeDef iE_EXTI_Event =
{
	.EXTI_Line = EXTI_Line8,
	.EXTI_Mode = EXTI_Mode_Interrupt,
	.EXTI_Trigger = EXTI_Trigger_Falling,
	.EXTI_LineCmd = ENABLE
};

/**
 * \brief           Incremental Encoder - NVIC for External Interrupt
 */
NVIC_InitTypeDef iE_NVIC =
{
	.NVIC_IRQChannel = EXTI9_5_IRQn,
	.NVIC_IRQChannelPreemptionPriority = 3,
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

/**
 *	\}
 *	\defgroup Variables vCOM settings
 *  \{
 */

/**
 * \brief           vCOM - GPIO Port: Tx
 */
GPIO_InitTypeDef vCOM_Tx =
{
	.GPIO_Pin = GPIO_Pin_9,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_AF_PP
};

/**
 * \brief           vCOM - GPIO Port: Rx
 */
GPIO_InitTypeDef vCOM_Rx =
{
	.GPIO_Pin = GPIO_Pin_10,
	.GPIO_Speed = GPIO_Speed_50MHz,
	.GPIO_Mode = GPIO_Mode_IN_FLOATING
};

/**
 * \brief           vCOM - Init Struct
 */
USART_InitTypeDef vCOM =
{
	.USART_BaudRate = 115200,
	.USART_WordLength = USART_WordLength_8b,
	.USART_StopBits = USART_StopBits_1,
	.USART_Parity = USART_Parity_No,
	.USART_Mode = (USART_Mode_Tx | USART_Mode_Rx),
	.USART_HardwareFlowControl = USART_HardwareFlowControl_None
};

/**
 * \brief           vCOM - Clk Init Struct
 */
USART_ClockInitTypeDef vCOM_clk =
{
	.USART_Clock = USART_Clock_Disable,
	.USART_CPOL = USART_CPOL_Low,
	.USART_CPHA = USART_CPHA_2Edge,
	.USART_LastBit = USART_LastBit_Disable
};

/**
 * \brief           vCOM - NIVC for USART
 */
NVIC_InitTypeDef vCOM_NVIC =
{
	.NVIC_IRQChannel = USART1_IRQn,
	.NVIC_IRQChannelPreemptionPriority = 3,
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

/**
 * \brief           vCOM - NVIC for DMA
 */
NVIC_InitTypeDef vCOM_DMA_NVIC =
{
	.NVIC_IRQChannel = DMA1_Channel1_IRQn,
	.NVIC_IRQChannelPreemptionPriority = 3,
	.NVIC_IRQChannelSubPriority = 0,
	.NVIC_IRQChannelCmd = ENABLE
};

/**
 * \brief           vCOM - DMA for USART
 */
DMA_InitTypeDef vCOM_DMA =
{
	.DMA_PeripheralBaseAddr = USART1_BASE,
	.DMA_MemoryBaseAddr = (uint32_t)&uartDR,
	.DMA_DIR = DMA_DIR_PeripheralDST,
	.DMA_BufferSize = 32,
	.DMA_PeripheralInc = DMA_PeripheralInc_Disable,
	.DMA_MemoryInc = DMA_MemoryInc_Enable,
	.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,
	.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte,
	.DMA_Mode = DMA_Mode_Circular,
	.DMA_Priority = DMA_Priority_Medium,
	.DMA_M2M = DMA_M2M_Disable
};

/**
 * \}
 */

/**
 * \defgroup Function Interrupt Service Routine
 */

/**
 * \brief           External Interrupt Service Routine
 * \note            If Event-Port (PC8) has a falling flank, this interrupt is triggered.
 *									The Direction of the Incremental Encoder (IE) depands on the state of
 * 									Direction-Port (PC9).
 */
void EXTI9_5_IRQHandler()
{
	// Direction-Port is set -> direction of the IE is left
	if (GPIO_ReadInputDataBit(GPIOC, iE_Direction.GPIO_Pin) == Bit_SET)
	{
		volume--;
		if (volume < 0)
			volume = 0;
	}
	
	// Direction-Port is not set -> direction of the IE is right
	else if (GPIO_ReadInputDataBit(GPIOC, iE_Direction.GPIO_Pin) == Bit_RESET)
	{
		volume++;
		if (volume > MAX_VOLUME)
			volume = MAX_VOLUME;
	}
	
	// Reset the Interrupt
	EXTI_ClearFlag(iE_EXTI_Event.EXTI_Line);
	NVIC_ClearPendingIRQ(EXTI9_5_IRQn);	
}

/**
 * \}
 * \defgroup Function vCOM - Functions
 * \{
 */

/**
 * \brief           Process received data over UART
 * \note            Either process them directly or copy to other bigger buffer
 * \param[in]       data: Data to process
 * \param[in]       len: Length in units of bytes
 */
void usart_process_data(const void* data, size_t len) 
{
    const uint8_t* d = data;
	
    /*
     * This function is called on DMA TC and HT events, aswell as on UART IDLE (if enabled) line event.
     * 
     * For the sake of this example, function does a loop-back data over UART in polling mode.
     * Check ringbuff RX-based example for implementation with TX & RX DMA transfer.
     */
	
    for (; len > 0; --len, ++d) {
        USART_SendData(USART1, *d);
        while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE)) {}
    }
    while (!USART_GetFlagStatus(USART1, USART_FLAG_TC)) {}
}

/**
 * \brief           Send string to USART
 * \param[in]       str: String to send
 */
void usart_send_string(const char* str) 
{
    usart_process_data(str, strlen(str));
}

/**
 * \}
 * \defgroup Function Main Programm
 * \{
 */

/**
 * \brief           Main Programm
 */
int main()
{
// initialice IE
	
	// Enable Peripheral Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// initialice GPIO Port and Pins
	GPIO_Init(GPIOC, &iE_Event);
	GPIO_Init(GPIOC, &iE_Direction);
	
	// initialice External Interrupt for IE-Event
	AFIO->EXTICR[8/4] |= (AFIO_EXTICR3_EXTI8_PC);
	EXTI_Init(&iE_EXTI_Event);
	
	// NVIC Interrupt Initialisieren
	NVIC_Init(&iE_NVIC);
	
	
// UART Init
	
	// GPIO Clock Aktivieren 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// GPIOs Initialisieren
	GPIO_Init(GPIOA, &vCOM_Tx);
	GPIO_Init(GPIOA, &vCOM_Rx);
	
	// DMA Initialisieren
	DMA_Init(DMA1_Channel1, &vCOM_DMA);
	DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1 | DMA1_IT_HT1, ENABLE);
	
	NVIC_Init(&vCOM_DMA_NVIC);
	
	// UART Initialisieren
	USART_ClockInit(USART1, &vCOM_clk);
	USART_Init(USART1, &vCOM);
	
	// NVIC Interrupt Initialisieren
	NVIC_Init(&vCOM_NVIC);
	
	// USART Staren
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	// USART Starten
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
	
	// Send Init Strings
	usart_send_string("is USART DMA functionable?\r\n");
	usart_send_string("YES\r\n");
	
	while(1)
	{
		// if there is a change in the volume
		if (oldVolume != volume)
		{
			char str[16];
			sprintf(str, "Volume:%02d\r\n", volume);
			usart_send_string(str);
			oldVolume = volume;
		}
	}
}

/**
 * \}
 */

