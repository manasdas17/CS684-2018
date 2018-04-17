/*
 * main.c
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.c"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.c"
#include "driverlib/gpio.h"


#define LED_PERIPH SYSCTL_PERIPH_GPIOF


void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(LED_PERIPH);
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
//  HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//  HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= GPIO_PIN_0;

}


int main(void)
{

    setup();
    int sw2Status=0;


    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPadConfigSet(GPIO_PORTF_BASE ,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    uint32_t value=0;
    uint32_t sw2Value=0;
    uint8_t state=0;

    while(1){


        value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4);
        sw2Value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0);
        if((sw2Value && GPIO_PIN_0)==0){
            sw2Status++;
        }
        if( (value && GPIO_PIN_4)==0 && (state==0) ){
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 2);
            SysCtlDelay(2000000);
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, 0);
            state=2;
            sw2Status++;

        }
        else if( (value && GPIO_PIN_4)==0 && (state==2) ) {
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 8);
            SysCtlDelay(2000000);
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3, 0);
            state=4;
            sw2Status++;


        }
        else if( (value && GPIO_PIN_4)==0 && (state==4) ){
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2, 4);
            SysCtlDelay(2000000);
            GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_2, 0);
            state=8;
            sw2Status++;

        }
        else if( state==8 ){
                    state=0;
        }
//      else {
//          state=0;
//      }
    }

}

