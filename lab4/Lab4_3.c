#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"
#include <time.h>
#include <inc/hw_gpio.h>
#include "driverlib/ssi.h"
#include "driverlib/uart.h"


uint32_t ui32ADC0Value[4];
uint32_t ui32ADC1Value[4];

volatile uint32_t ui32Avg0;
volatile uint32_t copy0;

volatile uint32_t ui32Avg1;
volatile uint32_t copy1;

volatile uint8_t digit0;
volatile uint8_t digit1;
volatile uint8_t digit2;
volatile uint8_t digit3;
volatile uint8_t digit4;
volatile uint8_t digit5;
volatile uint8_t digit6;
volatile uint8_t digit7;

void uart_char(char data)
{
    UARTCharPut(UART0_BASE, data);
}

int main(void) {
    ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    //ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

    ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6);
    ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC0_BASE, 1);

    ROM_ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH7);
    ROM_ADCSequenceStepConfigure(ADC1_BASE,1,3,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
    ROM_ADCSequenceEnable(ADC1_BASE, 1);

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //GPIOPinConfigure(GPIO_PA0_U0RX);
    //GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    while (1)
    {
        //if (UARTCharsAvail(UART0_BASE)) UARTCharPut(UART0_BASE, UARTCharGet(UART0_BASE));
        ROM_ADCIntClear(ADC0_BASE, 1);
        ROM_ADCProcessorTrigger(ADC0_BASE, 1);
        ROM_ADCIntClear(ADC1_BASE, 1);
        ROM_ADCProcessorTrigger(ADC1_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {
        }
        while(!ADCIntStatus(ADC1_BASE, 1, false))
        {
        }
        ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ROM_ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC1Value);

        ui32Avg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
        copy0=ui32Avg0;
        ui32Avg1 = (ui32ADC1Value[0] + ui32ADC1Value[1] + ui32ADC1Value[2] + ui32ADC1Value[3] + 2)/4;
        copy1=ui32Avg1;

        digit0=copy0%10+48;
        copy0=copy0/10;
        digit1=copy0%10+48;
        copy0=copy0/10;
        digit2=copy0%10+48;
        copy0=copy0/10;
        digit3=copy0%10+48;
        copy0=copy0/10;
        digit4=copy1%10+48;
        copy1=copy1/10;
        digit5=copy1%10+48;
        copy1=copy1/10;
        digit6=copy1%10+48;
        copy1=copy1/10;
        digit7=copy1%10+48;
        copy1=copy1/10+48;

        uart_char(digit0);
        uart_char(digit1);
        uart_char(digit2);
        uart_char(digit3);
        uart_char(',');
        uart_char(digit4);
        uart_char(digit5);
        uart_char(digit6);
        uart_char(digit7);
        uart_char('\n');

    }
}
