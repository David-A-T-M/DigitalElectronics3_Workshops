/**
 * @file LPC1769_registers.c
 * @brief Program to blink a single color on the onboard RGB LED of the LPC1769 board.
 *
 * This program configures the GPIO pin connected to the red channel of the onboard RGB LED.
 * It toggles the red LED on and off in a loop, creating a blinking effect.
 * The delay is implemented using a blocking nested loop.
 */
#include "LPC17xx.h"

/**
 * @def RED_LED
 * @brief Bit mask for the red LED (P0.22).
 */
#define RED_LED      (1<<22)

/**
 * @def DELAY
 * @brief Delay constant for LED timing.
 */
#define DELAY       2500

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
        LPC_GPIO0->FIOCLR = RED_LED;            // Turn red LED on.
        delay();

        LPC_GPIO0->FIOSET = RED_LED;            // Turn red LED off.
        delay();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(0x3<<12);          // P0.22 as GPIO.

    LPC_GPIO0->FIODIR |= RED_LED;               // Set P0.22 as output.

    LPC_GPIO0->FIOSET |= RED_LED;               // Force red LED off initially.
}

void delay() {
    for(uint32_t i=0; i<DELAY; i++)
        for(uint32_t j=0; j<DELAY; j++);
}
