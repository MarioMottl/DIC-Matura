#include 	"stm32f10x.h"               //standard library 
#include 	"stm32f10x_rcc.h"			//RCC Clock library
#include 	"stm32f10x_gpio.h"			//GPIO I/O library
#include 	"stm32f10x_usart.h"			//USART communication library
#include 	"string.h"					//strcat()
#include 	"uart.h"					//uart

#define countof(a)   (sizeof(a) / sizeof(*(a)))

//static int sek;
	
uint16_t const LED[8] = { 
    GPIO_Pin_15,
    GPIO_Pin_14,
    GPIO_Pin_13,
    GPIO_Pin_12,
    GPIO_Pin_11,
    GPIO_Pin_10,
    GPIO_Pin_9,
    GPIO_Pin_8,
    };

//includes
void LED_red(void);
void LED_green(void);
void Get_Password(char *password,int size);
void INT_OUT(int zahl);


//funktionen
void initleds(void)
{
    //Port x auf 1 setzten: GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET);
    //Port x auf 0 setzten: GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_RESET);
    GPIO_InitTypeDef pb;
    pb.GPIO_Mode = GPIO_Mode_Out_PP;
    pb.GPIO_Speed = GPIO_Speed_50MHz;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    pb.GPIO_Pin = LED[0] | LED[1] | LED[2] | LED[3] | LED[4] | LED[5] | LED[6] | LED[7];
    GPIO_Init(GPIOB, &pb);
}

void init_u(void)
{
    GPIO_InitTypeDef gpio;	
    USART_ClockInitTypeDef usartclock;
    USART_InitTypeDef usart;
    
    // Enable all GPIO and USART clocks needed for USART1
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    
    // Create gpio struct and fill it with defaults
    GPIO_StructInit(&gpio);
    
    // Set PA9 to alternate function push pull (Tx)
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = GPIO_Pin_9;
    GPIO_Init(GPIOA, &gpio);
    
    // Set PA10 to input floating (Rx)
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOA, &gpio);

    // Create usart struct and init USART1 to 115 200 baud
    USART_StructInit(&usart);
    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No ;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  
    USART_Init(USART1, &usart);

    // Init USART1 clock
    USART_ClockStructInit(&usartclock);
    USART_ClockInit(USART1, &usartclock);	

  // Enable USART1
    USART_Cmd(USART1, ENABLE);
}

void Get_Password(char *password,int size)
{
    /* Loop until the USARTz Receive Data Register is not empty */
    int counter=0;
        while(counter<size)
        {
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            {}
            while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
            {}
			password[counter] = USART_ReceiveData(USART1);
            Uart_Put_Char(USART1,password[counter]);
            counter++;
        }
}

//void TIM3_UP_IRQHandler(void) { 
////Timer 1, löst alle 1000ms aus 
////Interrupt pending bit löschen (Verhinderung eines nochmaligen Interrupt-auslösens) 
// TIM3->SR &=~0x01; 
// sek++; 
// } 
//
//static void TIM3_Config(void) { 
// /*---------------------- Timer 1 konfigurieren -----------------------*/ 
// RCC->APB2ENR |= 0x0800; //Timer 1 Clock enable 
// TIM3->SMCR = 0x0000; //Timer 1 Clock Selection: CK_INT wird verwendet 
// TIM3->CR1 = 0x0000; //Auswahl des Timer Modus: Upcounter 
// /* T_INT = 126,26ns, Annahme: */ 
// /* Presc = 150 -> Auto Reload Wert=52801(=0xCE41)-> 1s Update Event*/ 
// TIM3->PSC = 150; //Wert des prescalers (Taktverminderung) 
// TIM3->ARR = 0xCE41; //Auto-Reload Wert = Maximaler Zaehlerstand des Upcounters 
// TIM3->RCR = 0; //Repetition Counter deaktivieren 
// /*--------------------- Interrupt Timer 1 konfigurieren ---------------------*/ 
// TIM3->DIER = 0x01; //enable Interrupt bei einem UEV (Überlauf / Unterlauf) 
// NVIC_init(TIM3_UP_IRQn,2); //Enable Timer 1 Update Interrupt, Priority 2 
// /*-------------------------- Timer 1 Starten -------------------------------*/ 
// TIM3->CR1 |= 0x0001; //Counter-Enable bit setzen 
// } 
//
//static void NVIC_init(char position, char priority) 
//{ 
// NVIC->ICPR[position >> 0x05] |= (0x01 << (position & 0x1F)); 
// //Interrupt Clear Pendig Register: Verhindert, dass der 
// // Interrupt auslöst sobald er enabled wird 
// NVIC->IP[position]=(priority<<4); //Interrupt priority register: 
// //Setzen der Interrupt Priorität 
// NVIC->ISER[position >> 0x05] |= (0x01 << (position & 0x1F)); 
// //Interrupt Set Enable Register: Enable interrupt
//} 


void LED_red(void)
{
    GPIO_WriteBit(GPIOB, LED[1]|LED[3]|LED[5]|LED[7], Bit_RESET);
    GPIO_WriteBit(GPIOB, LED[0]|LED[2]|LED[4]|LED[6], Bit_SET);
}

void LED_green(void)
{
    GPIO_WriteBit(GPIOB, LED[1]|LED[3]|LED[5]|LED[7], Bit_SET);
    GPIO_WriteBit(GPIOB, LED[0]|LED[2]|LED[4]|LED[6], Bit_RESET);
}

void INT_OUT(int zahl)
{
    char czahl='0'+zahl;
    Uart_Put_Char(USART1,czahl);
}

int main(void)
{
    init_u();
    INIT_UART2();
    initleds();
    char password[6];
    char rightpassword[6]={"saftat"};
    Uart_Put_String(USART1,"\r\nPrgramm startet\r\n\n");
    while (1)
    {
        Uart_Put_String(USART1,"Bitte password eingeben:\r\n");
        //5sec einlesen
        Get_Password(password,countof(password));
        if(strcmp(password,rightpassword)==0){
            Uart_Put_String(USART1,"\r\npassword richtig\r\n");
            LED_green();
        }
		else{
            Uart_Put_String(USART1,"\r\npassword falsch erneute Eingabe\r\n");
            LED_red();
        }
    }
}
