/**
 * @file LPC1769_registers.c
 * @brief Detects a button press on P0.0 and controls an LED on P2.0 using direct register access.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Button connected to P0.0. */
#define BTN (0)
/** LED connected to P2.0. */
#define LED (0)

/** Mask for the button. */
#define BTN_BIT BIT_MASK(BTN)
/** Mask for the LED. */
#define LED_BIT BIT_MASK(LED)

/** PCB mask for the button. */
#define BTN_PCB BITS_MASK(2, BTN * 2)
/** PCB mask for the LED. */
#define LED_PCB BITS_MASK(2, LED * 2)

/**
 * @brief Configures GPIO pins for button input (P0.0) and LED output (P2.0).
 *
 * Sets up the pin function, mode, and direction for the button and LED.
 * Ensures the LED is turned off initially.
 */
void configGPIO(void);

int main(void) {
    configGPIO();

    while (1) {
        if (LPC_GPIO0->FIOPIN & BTN_BIT)    // Read button state.
            LPC_GPIO2->FIOSET = LED_BIT;    // Turn on LED.
        else
            LPC_GPIO2->FIOCLR = LED_BIT;    // Turn off LED.
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(BTN_PCB);     // P0.0 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(BTN_PCB);    // P0.0 with pull-up.

    LPC_PINCON->PINSEL4 &= ~(LED_PCB);    // P2.0 as GPIO.

    LPC_GPIO0->FIODIR &= ~(BTN_BIT);    // P0.0 as input.
    LPC_GPIO2->FIODIR |= LED_BIT;       // P2.0 as output.

    LPC_GPIO2->FIOCLR = LED_BIT;    // Turn off LED.
}
