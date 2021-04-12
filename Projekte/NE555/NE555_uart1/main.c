#include 	"stm32f10x.h"               //standard library 
#include 	"stm32f10x_rcc.h"			//RCC Clock library
#include 	"stm32f10x_gpio.h"			//GPIO I/O library
#include 	"stm32f10x_usart.h"			//USART communication library
#include 	"string.h"					//strcat()
#include 	"uart.h"					//uart

#define countof(a)   (sizeof(a) / sizeof(*(a)))

//Variablen

int falling_edges;  //Fallende Flake von Button für externen Interrupt
static int count;   //Zählt die Ticks bis 1millisek
//Varibale für Input Capture Einheit
__IO uint16_t IC2ReadValue1 = 0, IC2ReadValue2 = 0;
__IO uint16_t CaptureNumber = 0; 
__IO uint32_t Capture = 0; 

//spart schreibeaufwand und erzeugt Übersichtlichkeit	
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

void LED_red(void);     //grünen LEDs ein rot aus
void LED_green(void);   //roten LEDs ein grüne aus
void Get_Password(char *password,int size); //Liefert das Passwort über Polling
void INT_OUT(int zahl); //Gibt eine int zahl über uart aus
void wait_milli(void);  //wartet 1millisek
static void TIM3_Config(void); //Timer 3 Konfigurieren
void wait_sek(int sek); //wartet x Sekunden
float Find_frequenzy(void); //Misst Frequenz des NE555
void Schalter_abfragen(void);   //gibt aus ob schalter betätigt wurde

//funktionen

void initleds(void)
{
    //Port x auf 1 setzten: GPIO_WriteBit(GPIOx, GPIO_Pin_x, Bit_SET);
    //Port x auf 0 setzten: GPIO_WriteBit(GPIOx, GPIO_Pin_x, Bit_RESET);
    GPIO_InitTypeDef pb;    
    pb.GPIO_Mode = GPIO_Mode_Out_PP;        //Mode Push Pull
    pb.GPIO_Speed = GPIO_Speed_50MHz;       //Clockspeed 50MHz
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);   //Clock aktivieren
    pb.GPIO_Pin = LED[0] | LED[1] | LED[2] | LED[3] | LED[4] | LED[5] | LED[6] | LED[7]; //Pins der LEDs übergeben
    GPIO_Init(GPIOB, &pb); //Initialisierung
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
    int counter=0;
        //wartet bis Passwort eingegeben ist
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

void TIM3_IRQHandler(void) 
{ 
    count++;
    if ((TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) != RESET) && 
    (TIM_GetITStatus(TIM3,TIM_IT_Update) != 0)) // Update Interrupt Pending  
    { 
    TIM_ClearFlag(TIM3, TIM_FLAG_Update); // Clr TIM3 Update Int. pending bit 
    } 
    if ((TIM_GetFlagStatus(TIM3,TIM_FLAG_CC3) != RESET) && 
    (TIM_GetITStatus(TIM3,TIM_IT_CC3) != 0)) // Capture Interrupt von Channel3? 
    { 
    TIM_ClearFlag(TIM3, TIM_FLAG_CC3); 
    CaptureNumber = 1; 
    } 
    else if(CaptureNumber == 1) 
    { 
    IC2ReadValue2 = TIM_GetCapture3(TIM3); /* Get Input Capture value for Channel3*/ 
    if (IC2ReadValue2 > IC2ReadValue1) /* Capture computation */ 
    { 
    Capture = (IC2ReadValue2 - IC2ReadValue1); 
    } 
    else 
    { 
    Capture = ((TIM3->ARR - IC2ReadValue1) + IC2ReadValue2); 
    } 
    CaptureNumber = 0; 
    } 
} 
 
static void TIM3_Config(void)
{ 
    GPIO_InitTypeDef gpio; 
    TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure; 
    NVIC_InitTypeDef nvic; 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // GPIOA Clock Enable 
    GPIO_StructInit(&gpio);// Create gpio struct and fill it with defaults 
    gpio.GPIO_Mode = GPIO_Mode_IPU; 
    gpio.GPIO_Pin = GPIO_Pin_0; 
    GPIO_Init(GPIOB, &gpio); 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); // Clock Enable Timer 3 
    /* ----------- Configure Timer 3----------------------*/ 
    TIM_DeInit(TIM3); 
    TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
    TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    /* - T_INT = 13,8ns, Annahme: Presc = 35712 --> Auto Reload Wert = 2000 --> 1s*/ 
    TIM_TimeBase_InitStructure.TIM_Period = 2000; // Max. Zaehlerstand UpCounter 
    TIM_TimeBase_InitStructure.TIM_Prescaler = 35712; // system clock to 2kHz 
    TIM_TimeBaseInit(TIM4, &TIM_TimeBase_InitStructure); 
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_3; 
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 
    TIM_ICInitStructure.TIM_ICFilter = 0xF; /* filter to avoid bouncing */ 
    TIM_ICInit(TIM3, &TIM_ICInitStructure); 
    TIM_ITConfig (TIM3, TIM_IT_Update,ENABLE); // Timer 3 Update Interrupt Enable 
    TIM_ITConfig (TIM3, TIM_IT_CC3,ENABLE); // Enable TIM3 CC3 Interrupt Ch2 

    // Init NVIC for Timer 3 Interrupt 
    nvic.NVIC_IRQChannel = TIM3_IRQn; 
    nvic.NVIC_IRQChannelCmd = ENABLE; 
    nvic.NVIC_IRQChannelPreemptionPriority = 0; 
    nvic.NVIC_IRQChannelSubPriority = 2; 
    NVIC_Init(&nvic); 

    TIM_Cmd(TIM3, ENABLE); //Counter-Enable bit (CEN) Timer 3 setzen 
} 

void LED_red(void)
{
    //roten LEDs ein grünen aus
    GPIO_WriteBit(GPIOB, LED[1]|LED[3]|LED[5], Bit_RESET);
    GPIO_WriteBit(GPIOB, LED[0]|LED[2]|LED[4], Bit_SET);
}

void LED_green(void)
{
    //roten LEDs aus grünen ein
    GPIO_WriteBit(GPIOB, LED[1]|LED[3]|LED[5], Bit_SET);
    GPIO_WriteBit(GPIOB, LED[0]|LED[2]|LED[4], Bit_RESET);
}

void INT_OUT(int zahl)
{
    //Ausgebe einer int Zahl über UART
    char czahl[20];
    sprintf(czahl,"%d",zahl);
    Uart_Put_String(USART1,czahl);
}

void FLOAT_OUT(float zahl)
{
    //Ausgabe einer Floatzahl über UART
    char fzahl[20];
    sprintf(fzahl,"%.1f",zahl);
    Uart_Put_String(USART1,fzahl);
}

void wait_milli(void)
{
    //wartet genau 1 millisek
    for(int i=0;i<7780;i++);
}

void wait_sek(int sek)
{
    //wartet x Sekunden
    for(int i=0;i<1000*sek;i++)
    {
        wait_milli();
    }
}

float Find_frequenzy(void)
{
    count=0;
    int tick_neu=0;
    float frequenz;
    int tick_alt=count;
    wait_sek(1);
    tick_neu=count;
    frequenz=tick_neu-tick_alt;    //MICHI FRAGEN!!! /10 oder nicht
    return frequenz;
}

void EXTI1_IRQHandler(void)
{ 
    EXTI_ClearITPendingBit(EXTI_Line1); 
    //Pending bit EXT1 rücksetzen (Sonst wird die ISR immer wiederholt) 
    falling_edges++; // Fallende Flanke an PA1 erkannt 
    return; 
} 

void Schalter_abfragen(void)
{
    int frequenz;
    falling_edges=0;
    int changed_falling_edges=0;
    frequenz=Find_frequenzy(); //NE555 Frequenz wird von Funktion in Frequenz gespeichert
    wait_sek(10); //USER hat 10 Sekunden Zeit um den Externen Interrupt auszulösen
    if (changed_falling_edges != falling_edges) //Abfrage ob Änderung stattgefunden hat 
    { 
        changed_falling_edges=falling_edges;
        //Ausgabe der Frequenz und ob der Interrupt ausgelöst wurde
        Uart_Put_String(USART1,"\r\nDie Frequenz beträgt: ");
        FLOAT_OUT(frequenz);
        LED_green();
        Uart_Put_String(USART1," Hz\r\nDer Button wurde richtig betätigt");
    }
    else
    {
        //Interrupt nicht ausgelöst keine Frequenz wird angezeigt
        Uart_Put_String(USART1,"flascher Schalter");
    } 
}

static void EXTI1_Config(void) 
{ 
    //Konfiguartion des externen Interrupts
    GPIO_InitTypeDef gpio; 
    EXTI_InitTypeDef EXTI_InitStruct; 
    NVIC_InitTypeDef nvic; 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // GPIOA Clock Enable 

    GPIO_StructInit(&gpio);// Create gpio struct and fill it with defaults 
    gpio.GPIO_Mode = GPIO_Mode_IPU; // Configure PA1 to input Pull UP Mode 
    gpio.GPIO_Pin = GPIO_Pin_1; 
    GPIO_Init(GPIOA, &gpio); 

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); 
    //AFIOEN - Clock enable 
    EXTI_DeInit(); 

    EXTI_StructInit(&EXTI_InitStruct); 
    EXTI_InitStruct.EXTI_Line = EXTI_Line1; 
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; 
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; 
    EXTI_Init (&EXTI_InitStruct); /* save initialisation */ 
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); 
    /* configure EXTI1 Port A */ 
    
    EXTI_ClearITPendingBit(EXTI_Line1); 
    //EXTI_clear_pending: Das Auslösen auf vergangene Vorgänge nach dem enablen 
    //verhindern 
    // Init NVIC for EXTI1 Interrupt 
    nvic.NVIC_IRQChannel = EXTI1_IRQn; 
    nvic.NVIC_IRQChannelCmd = ENABLE; 
    nvic.NVIC_IRQChannelPreemptionPriority = 0; 
    nvic.NVIC_IRQChannelSubPriority = 3; 
    NVIC_Init(&nvic); 
} 

int main(void)
{
    //Initialisierungen
    init_u();
    INIT_UART2();
    initleds();
    TIM3_Config();
    EXTI1_Config();
    char password[6];
    char rightpassword[6]={"strloh"};

    Uart_Put_String(USART1,"\r\nPrgramm startet!\r\n");
    LED_red();
    while (1)
    {
        Uart_Put_String(USART1,"\r\n\nBitte password eingeben:\r\n");
        //Das Passwort wird über Polling eingegeben
        Get_Password(password,countof(password));
        //Abfrage ob das Passwort richtig ist. Externer Interrupt muss innerhalb von 10sec ausgeführt werden
        if(strcmp(password,rightpassword)==0){
            Uart_Put_String(USART1,"\r\npassword richtig\r\n\nNun betätigen Sie den richtigen Schalter, Sie haben 10Sekunden Zeit\r\n");
            LED_red();
            Schalter_abfragen();
        }
        //Passwort falsch eingegeben
		else{
            Uart_Put_String(USART1,"\r\npassword falsch erneute Eingabe\r\n");
            LED_red();
        }
    }
}
