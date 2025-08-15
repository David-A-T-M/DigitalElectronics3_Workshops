/**
 * @file LPC1769_registers.c
 * @brief Counts high pins on Port 0 and displays the result in binary on 5 LEDs (P2.0–P2.4).
 */
#include "LPC17xx.h"

/**
 * @def MASK_N_BITS
 * @brief Generic macro that creates a mask with the lowest n bits set to 1.
 * @param n Number of bits to set.
 */
#define MASK_N_BITS(n)   ((n) == 32 ? 0xFFFFFFFFU : ((1U << (n)) - 1))

/**
 * @def LEDS_MASK
 * @brief Mask for the 5 LEDs connected to P2.0-P2.4.
 */
#define LEDS_MASK   (MASK_N_BITS(5))

/**
 * @def DB_LEDS_MASK
 * @brief Mask for the PINSEL/PINMODE registers for the 5 LEDs connected to P2.0-P2.4.
 */
#define DB_LEDS_MASK    (MASK_N_BITS(10))

/**
 * @def PORT0_AV_MASK
 * @brief Mast for the available pins on port 0.
 */
#define PORT0_AV_MASK   (0x7FFF8FFF)

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
 */
uint8_t countOnes(uint32_t value);

int main(void) {
    configGPIO();

    while (1) {
        uint8_t leds = countOnes(LPC_GPIO0->FIOPIN & PORT0_AV_MASK);    // Counts high pins on Port 0.

        LPC_GPIO2->FIOCLR = LEDS_MASK;      // Turn off all LEDs.
        LPC_GPIO2->FIOSET = leds;           // Displays the count on the LEDs
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 = 0;                // Sets P0.0-P0.15 as GPIO.
    LPC_PINCON->PINSEL1 = 0;                // Sets P0.16-P1.31 as GPIO.
    LPC_PINCON->PINSEL4 &= ~DB_LEDS_MASK;   // Sets P2.0-P2.4 as GPIO (clears bits 0-9).

    LPC_PINCON->PINMODE0 = 0;               // Sets P0.0-P0.15 as pull-up.
    LPC_PINCON->PINMODE1 = 0;               // Sets P1.0-P1.31 as pull-up.

    LPC_GPIO0->FIODIR = 0;                  // Sets P0.0-P0.31 as input.
    LPC_GPIO2->FIODIR |= LEDS_MASK;         // Sets P2.0-P2.4 as output.

    LPC_GPIO2->FIOCLR = LEDS_MASK;          // Turn off all LEDs.
}

uint8_t countOnes(uint32_t value) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < 32; i++)
        if (value & (0x1 << i)) count++;
    return count;
}
