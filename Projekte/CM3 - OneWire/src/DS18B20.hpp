#pragma once

/*
DS18B20 – (One Wire Temperatursensor) 
Thema: DS18B20, UART#1 (Interrupt) 
*/

#include "stm32f10x.h" // Device header 
#include "stm32f10x_rcc.h" // Keil::Device:StdPeriph Drivers:RCC 
#include "stm32f10x_gpio.h" // Keil::Device:StdPeriph Drivers:GPIO 
#include "stm32f10x_usart.h" // Keil::Device:StdPeriph Drivers:USART 
#include "misc.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
typedef uint32_t size_t;

/*
LSB Byte 2^3 2^2 2^1 2^0 2^-1 2^-2 2^-3 2^-4
MSB Byte S S S S S 2^6 2^5 2^4

S... Sign
+125 °C = 0000 0111 1101 0000
-25,0625 °C = 1111 1 110 0110 1111
-55 °C = 1111 1 100 1001 0000
*/
/*
One Wire 
LSB First
Reset = 450us Low
50us later Device Respone 150us
LowByte = Falling Edge -> LOW und dies länger als 10us gehalten dannach High 10us
HighByte = Falling Edge -> LOW nur 10us dannach High


------|-_-----|------ 1
------|-_____-|------ 0


50us Wait
500us Resetpulse


*/

void wait_10us(void);   //waits 10 us
void wait_60us(void);   //waits 60 us
void wait_480us(void);  //waits 480 us

// Bitbanding for IDR of Bus (PB0)
#define DATA_IN (GPIOB->IDR)
//#define DATA_IN *((volatile unsigned long *)(BITBAND_PERI(GPIOB_IDR, 0)))
// Bitbanding for ODR of Bus (PB0)
#define DATA_OUT (GPIOB->ODR)
//#define DATA_OUT *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR, 0)))

void USART1_put_byte(uint8_t data);
void USART1_put_bytes(const void *data, size_t len);

namespace DS18B20
{
    void init();
    uint8_t resetBus();
    void sendBit(uint8_t bit);
    void sendByte(uint8_t data);
    uint8_t readBit(void);
    uint64_t readBytes(void);

    float interpret(uint64_t input);
} // namespace DS18B20


