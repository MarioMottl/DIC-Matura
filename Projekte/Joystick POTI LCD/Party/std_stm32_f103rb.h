/******************************************************************************/
/*   (C) Copyright HTL - HOLLABRUNN  2009-2010 All rights reserved. AUSTRIA   */ 
/*                                                                            */ 
/* File Name:   std_stm32_f103rb.h                                            */
/* Autor: 		Josef Reisinger/Michael Mötz                                  */
/* Version: 	V1.00                                                         */
/* Date: 		28/10/2010                                                    */
/* Description: Standard Library für ARM Corttex M3                           */
/******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F103RB_STD_H
#define __STM32F103RB_STD_H

/* Includes ------------------------------------------------------------------*/
#include <stm32f10x.h>
#include <stdio.h>

/* Einzel-Portpins im Bitbandbereich festlegen                                */

#define GPIOA_IDR GPIOA_BASE + 2*sizeof(uint32_t)	// Calc peripheral address GPIOA IDR
#define GPIOA_ODR GPIOA_BASE + 3*sizeof(uint32_t)	// Calc peripheral address GPIOA ODR
#define GPIOB_IDR GPIOB_BASE + 2*sizeof(uint32_t)	// Calc peripheral address GPIOB IDR
#define GPIOB_ODR GPIOB_BASE + 3*sizeof(uint32_t)	// Calc peripheral address GPIOB ODR
#define GPIOC_IDR GPIOC_BASE + 2*sizeof(uint32_t)	// Calc peripheral address GPIOC IDR
#define GPIOC_ODR GPIOC_BASE + 3*sizeof(uint32_t)	// Calc peripheral address GPIOC ODR
#define GPIOD_ODR GPIOD_BASE + 3*sizeof(uint32_t)	// Calc peripheral address GPIOD ODR


// Calc Bit Band Adress from peripheral address: a = peripheral address b = Bit number
#define BITBAND_PERI(a,b) ((PERIPH_BB_BASE + (a-PERIPH_BASE)*32 + (b*4)))  

#define SW0  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,0)))	// PA0
#define SW1  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,1)))	// PA1
#define SW2  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,2)))	// PA2
#define SW3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,3)))	// PA3
#define SW4  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,4)))	// PA4
#define SW5  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,5)))	// PA5
#define SW6  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,6)))	// PA6
#define SW7  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,7)))	// PA7

#define LED0  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,8)))	// PB8
#define LED1  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,9)))	// PB9
#define LED2  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,10)))	// PB10
#define LED3  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,11)))	// PB11
#define LED4  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,12)))	// PB12
#define LED5  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,13)))	// PB13
#define LED6  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,14)))	// PB14
#define LED7  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,15)))	// PB15

#define INC_BTN  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,7)))	// PC7
#define INC_1  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,8)))		// PC8
#define INC_2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,9)))		// PC9

#define JOY_BTN  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_IDR,5)))	// PB5
#define JOY_UP  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,8)))	// PC8
#define JOY_LEFT  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,6)))	// PC6
#define JOY_DOWN  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,7)))	// PC7
#define JOY_RIGHT  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,9)))	// PC9

#define B0_OUT  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_ODR,0)))	//PB0
#define B0_IN  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_IDR,0)))		//PB0

#define KB_C1  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_ODR,7)))		//PC7		   
#define KB_C2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_ODR,8)))		//PC8
#define KB_C3  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_ODR,9)))		//PC9
#define KB_R1  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,13)))	//PC13
#define KB_R2  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_IDR,1)))		//PB1
#define KB_R3  *((volatile unsigned long *)(BITBAND_PERI(GPIOB_IDR,5)))		//PB5
#define KB_R4  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,6)))		//PC6

#define DIL_LED_1  *((volatile unsigned long *)(BITBAND_PERI(GPIOD_ODR,2)))		//PD2
#define DIL_LED_2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_ODR,13)))	//PC13
#define DIL_LED_3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_ODR,8)))		//PA8

#define DIL_TASTER_1  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,5)))	//PC5
#define DIL_TASTER_2  *((volatile unsigned long *)(BITBAND_PERI(GPIOC_IDR,13)))	//PC13 
#define DIL_TASTER_3  *((volatile unsigned long *)(BITBAND_PERI(GPIOA_IDR,8)))	//PA8


/*---------------------------------- Definitions -----------------------------*/
#define LineLen     16                  /* Width of LCD (in characters)       */
#define NumLines     2                  /* Hight of LCD (in lines)            */

#ifdef STM32F103RB_STD_MOD
#define EXPORT
#else
#define EXPORT extern
#endif 

/* Exported functions ------------------------------------------------------- */

EXPORT void dil_taster_init(void);
EXPORT void dil_led_init(void);

EXPORT void keyboard_init(void);
EXPORT void joy_inc_init(void);

EXPORT void B0_init_out(void);
EXPORT void B0_init_in(void);
 
EXPORT char get_switches(void);
EXPORT void set_leds (char value);

EXPORT void init_leds_switches(void);

/**/ 

EXPORT void wait_ms(int ms);
EXPORT void wait_10us(void);

EXPORT void nib2asc(char *nib);
EXPORT void asc2nib(char *asc);
EXPORT int hex2bcd(char hex);

EXPORT void uart_init(unsigned long baudrate);
EXPORT char uart_get_char(void); 
EXPORT void uart_put_char(char ch);
EXPORT void uart_put_hex(char c); 
EXPORT void uart_put_string(char *string);
EXPORT void uart_clear(void);
EXPORT void uart_setpos(char x,char y);

EXPORT void lcd_init (void);
EXPORT void lcd_write_cmd (unsigned char c);
EXPORT void lcd_write_data (unsigned char c);
EXPORT void lcd_put_char (char c);
EXPORT void lcd_put_hex(char c); 
EXPORT void lcd_set_cursor (int line_nr, int column_nr);
EXPORT void lcd_clear (void);
EXPORT void lcd_put_string (char *string);
EXPORT void lcd_bargraph (int value, int size);
EXPORT void lcd_bargraphXY (int pos_y, int pos_x, int value);

EXPORT unsigned short int adc1_convert(unsigned char channel);

#undef  EXPORT
#endif /* __STM32F103RB_STD_H */

/******************* (C) HTL - HOLLABRUNN  2009-2010 *****END OF FILE****/
