#include "includes.h"

// STM32F10x_StdPeriph_Lib_V3.5.0/Project/STM32F10x_StdPeriph_Examples/ADC/ADC1_DMA/main.c

//ADC1 Init
void initadc()
{
    ADC_InitTypeDef adc_init;
    GPIO_InitTypeDef gpio;

    //Peripheral Clock Enable
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_IOPAEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    //ADC1 Channel 9 (PB1, POTI LED/Schalter)
    gpio.GPIO_Mode = GPIO_Mode_AIN;
    gpio.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOB, &gpio);

    //Setup ADC1
    adc_init.ADC_Mode = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode = DISABLE;
    adc_init.ADC_ContinuousConvMode = ENABLE; //Continous Conversion Mode
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel = 1;

    //Initialisierung des ADC's
    ADC_Init(ADC1, &adc_init);

    ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 1, ADC_SampleTime_28Cycles5);
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);

    //ADC1 Calibration
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
}

int main(void)
{
    initadc();
    //Beispielhafte verwendung des ADC's
    int value = ADC_GetConversionValue(ADC1);
    return 0;
}