/**
 * @file LPC1769_registers.c
 * @brief GPIO and 7-segment display control for LPC1769.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Mask for a 7 segments display. */
#define SVN_SEGS   BITS_MASK(7, 0)
/** Mask for input pins P0.0-P0.3. */
#define INPUT_PINS BITS_MASK(4, 0)

/** Double bit mask for a 7 segments display. */
#define SVN_SEGS_PCB   BITS_MASK(14, 0)
/** Double bit mask for input pins P0.0-P0.3. */
#define INPUT_PINS_PCB BITS_MASK(8, 0)

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

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int main(void) {
    configGPIO();

    while (1) {
        uint32_t value = LPC_GPIO0->FIOPIN & INPUT_PINS;    // Read P0.0-P0.3

        LPC_GPIO2->FIOCLR = SVN_SEGS;         // Clear all segments.
        LPC_GPIO2->FIOSET = digits[value];    // Display the value on the 7-segment display.
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(INPUT_PINS_PCB);     // P0.0-P0.3 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(INPUT_PINS_PCB);    // P0.0-P0.3 with pull-up.
    LPC_GPIO0->FIODIR &= ~(INPUT_PINS);           // P0.0-P0.3 as input.

    LPC_PINCON->PINSEL4 &= ~(SVN_SEGS_PCB);    // P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SVN_SEGS;             // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SEGS;    // Turns off all segments.
}
