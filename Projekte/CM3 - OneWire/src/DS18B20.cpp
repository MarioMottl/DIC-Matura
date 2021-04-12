#include "DS18B20.hpp"

extern "C" void USART1_IRQHandler(void)
{
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    char input;
    input = USART_ReceiveData(USART1);
    // TODO: process USART input
    return;
}

void DS18B20::init()
{
    USART_InitTypeDef usart;
    USART_ClockInitTypeDef usart_clock;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    // enable clock for GPIOA & GPIOB
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE );

    // one wire pin PB0
    gpio.GPIO_Pin = GPIO_Pin_0;
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio);

    // USART1 RX (PA10 in floating)
    gpio.GPIO_Pin = GPIO_Pin_10;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    // USART1 TX (PA9 out PP)
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
}

float DS18B20::interpret(uint64_t input)
{
    uint16_t buf, in;
    float result = 0;
    in = 0; // endianess (byte order) problem
    in += input & 0x00ff >> 0;
    in += input & 0xff00 >> 8;
    buf = in & 0x07ff; // mask out sign bits
    result = buf / (16.0); // divide to let LSb represent 2^-4
    result *= ( (in & 0xf800) != 0) ? -1 : 1; // interpret sign bit
    return result;
}

void USART1_put_byte(uint8_t data)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}
    USART_SendData(USART1, data);
}
void USART1_put_bytes(const void *data, size_t len)
{
    const uint8_t *bytes = (const uint8_t*)data;
    for(size_t i=0; i<len; i++)
        USART1_put_byte(bytes[i]);
}

void wait_10us(void)		//waits 10 us
{
	uint8_t i, j;
	for (i = 0; i < 5; i++)
	{
		for (j = 0; j < 10; j++)
		{
			__NOP();
		}
	}
}

void wait_60us(void)		//waits 60 us
{
	uint8_t i, j;
	for (i = 0; i < 8; i++)
	{
		for (j = 0; j < 55; j++)
		{
			__NOP();
		}
	}
}
 
void wait_480us(void)		//waits 480 us
{
	uint8_t i, j;
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 106; j++)
		{
			__NOP();
		}
	}
}

uint8_t DS18B20::resetBus (void)
{
	uint8_t DevResponse = 0;
	
	DATA_OUT = 0;				//RESET Flanke as in One-Wire protocol
	wait_480us(); 	

	DATA_OUT = 1;
	wait_60us();
	
	DevResponse = DATA_IN^0x01;
	
	wait_480us();				//wating for presence condition

	return DevResponse;	//true or false
}


void DS18B20::sendBit(uint8_t bit)
{
		if(bit == 1)
	{
		DATA_OUT = 0;	//Write '1' bit
		wait_10us();
		DATA_OUT = 1;	//Releases the bus
		wait_60us();	//Complete the time slot and 10us recovery
	}
	else
	{
		DATA_OUT = 0;	//Write '0' bit
		wait_60us();
		DATA_OUT = 1; //Releases the bus
		wait_10us();
	}
}


void DS18B20::sendByte(uint8_t data)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		sendBit(data & 0x01); //Send LSB
		data >>= 1;           //Shift right for next Bit
	}
}

//Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
uint8_t DS18B20::readBit(void)
{
	uint8_t value = 0;
	
	DATA_OUT = 0;
	wait_10us();
	DATA_OUT = 1;						//Releases the bus
	wait_10us();
	//value = DATA_IN & 0x01;	//Sample the bit value from the slave
    value = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
	wait_60us();						//Complete the time slot and 10us recovery
	
	return value;
}

// Read 1-Wire data byte and return it
uint64_t DS18B20::readBytes(void)
{
	uint64_t idByte = 0;

	// read LSB to MSB
	for (uint8_t i = 0; i < 64; i++)
	{
		idByte >>= 1;		//shift the result to get it ready for the next bit
		if (readBit())
		{
			//if result is one, then set MS bit
			idByte |= 0x8000000000000000;
		}
	}
	return idByte;
}
