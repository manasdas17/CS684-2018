//##################################################
// Author: Manas R. Das                            #
// Description: Controlling RGB LED by Pushbuttons #
//Version-0.1                                      #
//##################################################



#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include <time.h>

void SW1ButtonPress(void);
void SW1ButtonRelease(void);
void SW2ButtonPress(void);
void SW2ButtonRelease(void);

uint8_t LED = 2; //for 1byte of data
uint8_t sw2Status = 0; //for 1byte of data
uint32_t delay_500ms = 6700000; // Delay of 0.5s
uint8_t delay = 1;              // Delay multiplier

// Interrupt handler for specified GPIO port and specified pins when buttons pressed
void ButtonPress(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {                //PF4 is interrupt cause
        SW1ButtonPress();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {         //PF0 is interrupt cause
        SW2ButtonPress();
    }
}

// Interrupt handler for specified GPIO port and specified pins when buttons released
void ButtonRelease(void) {
    if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_4) {               //PF4 is interrupt cause
        SW1ButtonRelease();
    } else if (GPIOIntStatus(GPIO_PORTF_BASE, false) & GPIO_PIN_0) {        //PF0 is interrupt cause
        SW2ButtonRelease();
    }
}

void SW2ButtonPress(void) {
    // PF4 was interrupt cause
    GPIOIntRegister(GPIO_PORTF_BASE, ButtonRelease);                   // Register handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_RISING_EDGE);  // Configure PF4 for rising edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);                      // Clear interrupt flag

    // Looping over led light
    if (LED == 8)
    {
        LED = 2;
    }
    else
    {
        LED = LED*2;
    }
}

void SW2ButtonRelease(void) {
    // PF4 was interrupt cause
    GPIOIntRegister(GPIO_PORTF_BASE, ButtonPress);                 // Register handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE); // Configure SW2 for falling edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);                      // Clear interrupt flag
}



void SW1ButtonPress(void) {
    // PF4 was interrupt cause
    GPIOIntRegister(GPIO_PORTF_BASE, ButtonRelease);                   // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_RISING_EDGE);  // Configure SW1 for rising edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);                      // Clear interrupt flag

    // Setting up delays
    if (delay == 4)
    {
        delay = 1;
    }
    else
    {
        delay = delay*2;
    }
}

void SW1ButtonRelease(void) {
    // PF4 was interrupt cause
    sw2Status++;
    GPIOIntRegister(GPIO_PORTF_BASE, ButtonPress);                 // Register our handler function for port F
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE); // Configure PF4 for falling edge trigger
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);                      // Clear interrupt flag
}

/*

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals

* Example Call: setup();

*/
void setup(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);    // Enable port F

    //
    // Unlock PF0 so we can change it to a GPIO input
    // Once we have enabled (unlocked) the commit register then re-lock it
    // to prevent further changes.  PF0 is muxed with NMI thus a special case.
    //
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;

    // Init PF4 and PF0 as input
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);
    // Enable weak pullup resistor for PF4
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
    // Interrupt setup sw1 PF4 & sw2 PF0
    GPIOIntDisable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);   // Disable interrupt for PF4 & PF0 (in case it was enabled)
    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);     // Clear pending interrupts for PF4 & PF0

    GPIOIntRegister(GPIO_PORTF_BASE, ButtonPress);             // Register handler function for port F

    // Configure PF4 & PF0 for falling edge trigger
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4 | GPIO_PIN_0);    // Enable interrupt for PF4 & PF0
}

/*

* Function Name: pin_config()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.

* Example Call: pin_config();

*/

void pin_config(void)
{
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}


int main(void)
{
    setup();
    pin_config();

    while(1)
    {
        // Turn on the LED
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, LED);
        // Delay for a bit
        SysCtlDelay(delay*delay_500ms);
        // Turn off the LED
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
        // Delay for a bit
        SysCtlDelay(delay*delay_500ms);
    }
}
