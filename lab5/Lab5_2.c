#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "driverlib/fpu.h"
#include "utils/uartstdio.h"



volatile uint32_t ui32JoyUD;
volatile uint32_t ui32JoyLR;
volatile uint32_t center_x;
volatile uint32_t center_y;

void setup(void)       // set crystal freq and enable GPIO pins
{
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    HWREG(GPIO_PORTF_BASE+GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE+GPIO_O_CR) |= GPIO_PIN_0;

    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
    //GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6 );
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_3 |GPIO_PIN_4|GPIO_PIN_6 );
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_3|GPIO_PIN_4,0X18);
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3);
    //GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3);
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 );
    //GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7);*/

}

void adc_init(void){
    //GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    //  enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    //configure ADC sequencer - use ADC 0, sample sequencer 1, want the processor to trigger sequence, use highest priority
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    //Configure ADC Sequencer Steps 0 - 2 on sequencer 1 to sample the temperature sensor
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6 );
    //final sequencer step: need to sample temp sensor(ADC_CTL_TS), configure interrupt flag(ADC_CTL_IE) to be set when sample is done
    //tell ADC logic that this is the last conversion on sequencer (ADC_CTL_END)
    ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 1);
}


void glcd_cmd(unsigned char cmd)
{
    /*clear data lines */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, 0x00);

    /*RS =0*/
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6, 0x00);

    /*Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3, cmd);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, cmd);

    /*Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0x01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0x00);

}



void glcd_init()
{
    SysCtlDelay(134000);
    /*clear RST*/
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5,0x00);
    SysCtlDelay(134000);


    /*Set RST */
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0x20);

    /*Initialise left side of GLCD*/
    /*Set CS1(CS1=1 and CS2=0) */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x00);

    /*Select start line */
    glcd_cmd(0xC0);
    /*Select the page*/
    glcd_cmd(0xB8);
    /*Select the column*/
    glcd_cmd(0x40);
    /*Send glcd on command*/
    glcd_cmd(0x3F);


    /*Initialise left side of GLCD*/
    /*Set CS2(CS1=0 and CS2=1) */
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0x08);

    /*Select start line */
    glcd_cmd(0xC0);
    /*Select the page*/
    glcd_cmd(0xB8);
    /*Select the column*/
    glcd_cmd(0x40);
    /*Send glcd on command*/
    glcd_cmd(0x3F);

}

void glcd_data(unsigned char data)
{
    /*clear data lines */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3, 0x00);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, 0x00);

    /*RS =1*/
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6, 0x40);

    /*Put command on data lines */
    GPIOPinWrite(GPIO_PORTE_BASE,GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2 |GPIO_PIN_3, data);
    GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_4 |GPIO_PIN_5|GPIO_PIN_6 |GPIO_PIN_7, data);

    /*Generate a high to low pulse on enable */
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0x01);
    SysCtlDelay(1340);
    GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_0,0x00);

}


void glcd_setpage (unsigned char page)
{
    /*set CS1(CS1=1 ans CS2=0)right side is selected for column>64 */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3, 0x00);

    /*Select the page*/
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100);

    /*set CS2(CS1=0 ans CS2=1)left side is selected for column<64 */
    GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3, 0x08);

    /*select the page*/
    glcd_cmd(0xB8 | page);
    SysCtlDelay(100);

}

void glcd_setcolumn(unsigned char column)
{

    if(column < 64)
    {
        /*set CS1(CS1=1 ans CS2=0)right side is selected for column>64 */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3, 0x00);

        /*Select column on left side*/
        glcd_cmd(0x40 | column);
        SysCtlDelay(6700);
    }
    else
    {
        /*set CS2(CS1=0 ans CS2=1)left side is selected for column<64 */
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3, 0x08);

        /*select the column on the right*/
        glcd_cmd(0x40 | (column-64) );
        SysCtlDelay(6700);

    }
}

void glcd_cleardisplay()
{
    unsigned char i,j;
    for(i=0;i<8;i++)
    {
        glcd_setpage(i);
        for(j=0;j<128;j++)
        {
            glcd_setcolumn(j);
            glcd_data(0x00);

        }
    }
}

void displaysquare(void){
    glcd_cleardisplay();
    unsigned char i;
    center_x = (ui32JoyLR)/32;
    center_y = (ui32JoyUD)/256;

    if(center_x>127)
        center_x = 127;
    center_x = 127-center_x;
    signed int starting_pt = center_x-4;
    if (starting_pt<0)
        starting_pt = 0;
    unsigned char end_pt = center_x+4;
    if (end_pt>127)
        end_pt = 127;

    if(center_y>15)
        center_y = 15;
    center_y = 15-center_y;

    int rem_center = (center_y%2);
    if( rem_center == 1){
        unsigned char st_pt_y  = (center_y-1)/2;
        unsigned char end_pt_y = (center_y+1)/2;
        glcd_setpage(st_pt_y);
        for(i=starting_pt;i<end_pt;i++)
        {
            glcd_setcolumn(i);
            glcd_data(0xF0);
        }
        glcd_setpage(end_pt_y);
        for(i=starting_pt;i<end_pt;i++)
        {
            glcd_setcolumn(i);
            glcd_data(0x0F);
        }
    }
    else{
        unsigned char st_pt_y  = (center_y)/2;
        glcd_setpage(st_pt_y);
        for(i=starting_pt;i<end_pt;i++)
        {
            glcd_setcolumn(i);
            glcd_data(0xFF);
        }
    }
}

int main(void)
{
    uint32_t ui32ADC0Value[4];
    setup();
    glcd_init();
    glcd_cleardisplay();
    adc_init();
    while(1)
    {
        //clear interrupt flag
        ADCIntClear(ADC0_BASE, 1);
        ADCProcessorTrigger(ADC0_BASE, 1);
        while(!ADCIntStatus(ADC0_BASE, 1, false))
        {

        }
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
        ui32JoyUD = (ui32ADC0Value[0] + ui32ADC0Value[1] + 1)/2;
        ui32JoyLR = (ui32ADC0Value[2] + ui32ADC0Value[3] + 1)/2;
        displaysquare();
    }



/*
    //displaying contents of .h file
    uint32_t j;
    unsigned char p;
    j=0;
    p=0;
    while(p < 8)
    {
        //set the page
        glcd_setpage(p);
        for(i=0;i<128;i++)
        {
            //select the column form 0 to 127
            glcd_setcolumn(i);
            //send hex value of data to GLCD
            glcd_data(a[j]);
            SysCtlDelay(67000);
            j++;
        }
        //increment the page number after previous page is filled
        p++;
    }
    */
return 0;
}
