/**
 * @file LPC1769_registers.c
 * @brief Detects a button press on P0.0 and controls an LED on P2.0 using direct register access.
 */
#include "LPC17xx.h"

/**
 * @def BUTTON_PIN
 * @brief Mask for the button connected.
 */
#define BUTTON_PIN      0

/**
 * @def LED_PIN
 * @brief Mask for the LED connected.
 */
#define LED_PIN         0

/**
 * @brief
 */
void configGPIO(void);

int main(void) {
    configGPIO();

    while (1) {
        if (LPC_GPIO0->FIOPIN & (0x1 << BUTTON_PIN))
            LPC_GPIO2->FIOSET = (0x1 << LED_PIN);           // Turn on LED
        else
            LPC_GPIO2->FIOCLR = (0x1 << LED_PIN);           // Turn off LED
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(0x3 << (BUTTON_PIN * 2));      // P0.0 as GPIO
    LPC_PINCON->PINMODE0 &= ~(0x3 << (BUTTON_PIN * 2));     // P0.0 pull-up

    LPC_PINCON->PINSEL4 &= ~(0x3 << (LED_PIN * 2));         // P2.0 as GPIO

    LPC_GPIO0->FIODIR &= ~(0x1 << BUTTON_PIN);              // P0.0 as input (button)
    LPC_GPIO2->FIODIR |= (0x1 << LED_PIN);                  // P2.0 as output (LED)

    LPC_GPIO2->FIOCLR = (0x1 << LED_PIN);                   // Turn off LED at start
}
