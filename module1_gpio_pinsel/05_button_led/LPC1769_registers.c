/**
 * @file LPC1769_registers.c
 * @brief Detects a button press on P0.0 and controls an LED on P2.0 using direct register access.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Mask for the button. */
#define BUTTON_PIN          BIT_MASK(0)
/** Mask for the LED. */
#define LED_PIN             BIT_MASK(0)
/** Double bit mask for the button. */
#define BUTTON_PIN_DB       BITS_MASK(2, 0)
/** Double bit mask for the LED. */
#define LED_PIN_DB          BITS_MASK(2, 0)

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
        if (LPC_GPIO0->FIOPIN & BUTTON_PIN)
            LPC_GPIO2->FIOSET = LED_PIN;                    // Turn on LED.
        else
            LPC_GPIO2->FIOCLR = LED_PIN;                    // Turn off LED.
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(BUTTON_PIN_DB);                // P0.0 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(BUTTON_PIN_DB);               // P0.0 with pull-up.

    LPC_PINCON->PINSEL4 &= ~(LED_PIN_DB);                   // P2.0 as GPIO.

    LPC_GPIO0->FIODIR &= ~(BUTTON_PIN);                     // P0.0 as input.
    LPC_GPIO2->FIODIR |= LED_PIN;                           // P2.0 as output.

    LPC_GPIO2->FIOCLR = LED_PIN;                            // Turn off LED.
}
