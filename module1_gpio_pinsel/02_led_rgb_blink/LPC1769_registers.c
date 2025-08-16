/**
 * @file LPC1769_registers.c
 * @brief Program to blink a single color on the onboard RGB LED of the LPC1769 board.
 *
 * This program configures the GPIO pin connected to the red channel of the onboard RGB LED.
 * It toggles the red LED on and off in a loop, creating a blinking effect.
 * The delay is implemented using a blocking nested loop.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Bit mask for the red LED (P0.22). */
#define RED_LED             BIT_MASK(22)
/** Double bit mask for the red LED (P0.22). */
#define RED_LED_DB          BITS_MASK(2, 12)

/** Delay constant for LED timing. */
#define DELAY               2500

/**
 * @brief Configures the GPIO pin for the red LED as output.
 *
 * Sets the pin function to GPIO and configures the direction as output.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    while(1) {
        LPC_GPIO0->FIOCLR = RED_LED;            // Turn LED on.
        delay();

        LPC_GPIO0->FIOSET = RED_LED;            // Turn LED off.
        delay();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_LED_DB);       // P0.22 as GPIO.

    LPC_GPIO0->FIODIR |= RED_LED;               // Set P0.22 as output.

    LPC_GPIO0->FIOSET |= RED_LED;               // Force LED off initially.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
