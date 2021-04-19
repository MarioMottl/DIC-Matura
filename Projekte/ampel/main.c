/*
*/
#include "ue4.h"


volatile enum led ampel;


//TIMER 4 Interrupt Routine Handler
//PWM Output Mode, Channel 3
void TIM3_IRQHandler(void)
{ 
	TIM3->SR &=~0x00FF;		//Interrupt pending-bit löschen
	
	ampel = ampel == green1 ? red : green1;
}



void set_led(enum led led, _Bool on)
{
	GPIO_TypeDef * gpio;
	uint16_t pin;
	
	switch (led) {
	case green1:
		gpio = GPIOA;
		pin = GPIO_Pin_8;
		break;
	case green2:
		gpio = GPIOC;
		pin = GPIO_Pin_13;
		break;
	case red:
		gpio = GPIOD;
		pin = GPIO_Pin_2;
		break;
	}
	
	if (on) {
		GPIO_ResetBits(gpio, pin);
	}
	else {
		GPIO_SetBits(gpio, pin);
	}
}

void blink_led(enum led led, int count, int time)
{
	for (int i = 0, s = true; i < 2 * count; ++i, s = !s) {
		set_led(led, s);
		wait_ms(time);
	}
}

bool UartInIs(USART_TypeDef * const usart, char character)
{
	return !((USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET) || USART_ReceiveData(usart) != character);
}

void Init(void)
{
	InitUsart2();		//USART2 Init (Log)
	InitGpio();			//GPIOB1 Init (Externe LEDs) (F1 Ampel)
	//InitExti1();		//EXTI1 Init (Flankenzähler)
	TIM3_Config();	//TIM4 Output Compare Init (PWM) (Countdown)
	UartPutString(USART2, "Initialized\r\n");
	
	set_led(green1, 0);
	set_led(green2, 0);
	set_led(red, 0);
}

int main()
{
	Init();
	
	bool blink = false;
	ampel = green1;
	enum led prev_ampel = -1;
	
	while (1) { //Endlosschleife
		while (!UartInIs(USART2, '1'));
		UartPutString(USART2, "\r\nAmpel gestartet\r\n");
		UartPutString(USART2, "**************************************\r\n");
		
		while (!UartInIs(USART2, '0')) {
			if (prev_ampel != ampel) {
				set_led(green1, 0);
				set_led(red, 0);
				
				if (ampel == green1) {
					UartPutString(USART2, "Ampel gruen\r\n");
					blink = false;
				}
				else if (!blink) {
					UartPutString(USART2, "Ampel blinkt gruen\r\n");
					blink = true;
					blink_led(green1, 4, 250);
					UartPutString(USART2, "Ampel rot\r\n");
				}
				set_led(ampel, 1);
				prev_ampel = ampel;
			}
		}
		UartPutString(USART2, "\r\nAmpel gestoppt\r\n");
		UartPutString(USART2, "**************************************\r\n");
	}
}

