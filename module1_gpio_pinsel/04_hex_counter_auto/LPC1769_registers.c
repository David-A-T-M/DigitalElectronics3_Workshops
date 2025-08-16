/**
 * @file LPC1769_registers.c
 * @brief Controls a 7-segment display using GPIO pins on the LPC1769 board.
 *
 * This program configures GPIO pins P2.0-P2.6 to drive a 7-segment display.
 * It cycles through hexadecimal digits (0-F) by updating the display in a loop.
 * The display segments are controlled via direct register access.
 */

#include "LPC17xx.h"

/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Mask for a 7 segments display. */
#define SVN_SEGS            BITS_MASK(7, 0)
/** Double bit mask for a 7 segments display. */
#define SVN_SEGS_DB         BITS_MASK(14, 0)

/** Number of elements in the digits array. */
#define DIGITS_SIZE         (sizeof(digits) / sizeof(digits[0]))

/** Delay constant for LED timing. */
#define DELAY               2500

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

    uint32_t i = 0;

    while (1) {
        LPC_GPIO2->FIOCLR = SVN_SEGS;                   // Turn off all segments.
        LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Show the current digit.

        i++;
        delay();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL4 &= ~(SVN_SEGS_DB);              // P2.0-P2.6 as GPIO.

    LPC_GPIO2->FIOMASK = ~(SVN_SEGS);                   // Optional: Set mask for protection.

    LPC_GPIO2->FIODIR |= SVN_SEGS;                      // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SEGS;                       // Turn off all segments.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
