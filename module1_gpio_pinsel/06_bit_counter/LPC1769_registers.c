/**
 * @file LPC1769_registers.c
 * @brief Counts high pins on Port 0 and displays the result in binary on 5 LEDs (P2.0–P2.4).
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Mask for the 5 LEDs connected to P2.0-P2.4. */
#define LEDS_MASK     BITS_MASK(5, 0)
/** PCB mask for the 5 LEDs connected to P2.0-P2.4. */
#define LEDS_MASK_PCB BITS_MASK(10, 0)

/** Mask for the available pins on port 0. */
#define PORT0_AV_MASK (0x7FFF8FFF)

/**
 * @brief Configures the GPIO pins for the LEDs and input port.
 *
 * Sets P0.0-P0.15 and P1.0-P1.31 as GPIO,
 * P2.0-P2.4 as output for LEDs, and P0.0-P0.31 as input.
 * Sets pull-up resistors for P0 and P1.
 * Clears the LEDs at startup.
 */
void configGPIO(void);

/**
 * @brief Counts the number of bits set to 1 in a 32-bit value.
 *
 * @return Number of bits set to 1 in the input value.
 */
uint8_t countOnes(uint32_t value);

int main(void) {
    configGPIO();

    while (1) {
        const uint8_t leds = countOnes(LPC_GPIO0->FIOPIN & PORT0_AV_MASK);

        LPC_GPIO2->FIOCLR = LEDS_MASK;    // Turn off all LEDs.
        LPC_GPIO2->FIOSET = leds;         // Displays the count on the LEDs
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 = 0;                  // P0.0-P0.15 as GPIO.
    LPC_PINCON->PINSEL1 = 0;                  // P0.16-P1.31 as GPIO.
    LPC_PINCON->PINSEL4 &= ~LEDS_MASK_PCB;    // P2.0-P2.4 as GPIO.

    LPC_PINCON->PINMODE0 = 0;    // P0.0-P0.15 with pull-up.
    LPC_PINCON->PINMODE1 = 0;    // P1.0-P1.31 with pull-up.

    LPC_GPIO0->FIODIR = 0;             // P0.0-P0.31 as input.
    LPC_GPIO2->FIODIR |= LEDS_MASK;    // P2.0-P2.4 as output.

    LPC_GPIO2->FIOCLR = LEDS_MASK;    // Turn off all LEDs.
}

uint8_t countOnes(uint32_t value) {
    uint8_t count = 0;

    for (uint8_t i = 0; i < 32; i++)
        if (value & (0x1 << i))
            count++;

    return count;
}
