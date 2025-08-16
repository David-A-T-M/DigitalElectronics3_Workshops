/**
 * @file LPC1769_registers.c
 * @brief GPIO and 7-segment display control for LPC1769.
 */
#include "LPC17xx.h"

/**
 * @def SEGMENTS_MASK(n)
 * @brief Mask for 7 segments of a display starting at bit n.
 * @param n The starting bit position for the segments.
 */
#define SEGMENTS_MASK(n)   (0x7F << (n))

/**
 *
 * @param n
 */
#define INPUT_PINS_MASK(n) (0xF << (n))

/**
 * @def BUTTON_PIN
 * @brief Mask for the button connected.
 */
#define BUTTON_PIN 0

/**
 * @def DIGITS_SIZE
 * @brief Number of elements in the digits array.
 */
#define DIGITS_SIZE (sizeof(digits) / sizeof(digits[0]))

/**
 * @brief Configures GPIO pins P2.0-P2.6 as outputs to control a 7-segment display.
 *
 * Sets the pin function to GPIO and sets the direction to output.
 * Turns off all segments initially.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 */
void delay();

/**
 * @brief Array of segment values for hexadecimal digits (0-F).
 */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int main(void) {
    configGPIO();

    while (1) {
        uint32_t value = LPC_GPIO0->FIOPIN & INPUT_PINS_MASK(0); // Read P0.0-P0.3
        LPC_GPIO2->FIOCLR = SEGMENTS_MASK(0);                 // Clear all segments
        LPC_GPIO2->FIOSET = digits[value];
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(0xFF << BUTTON_PIN);           // P0.0-P0.3 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(0xFF << BUTTON_PIN);          // P0.0-P0.3 with pull-up.
    LPC_GPIO0->FIODIR &= ~(0xF << BUTTON_PIN);            // P0.0 as input.

    LPC_PINCON->PINSEL4 &= ~(0x3FFF);                       // Sets P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SEGMENTS_MASK(0);                // Sets P2.0-P2.6 as output.
    LPC_GPIO2->FIOCLR = SEGMENTS_MASK(0);                 // Turns off all segments.
}
