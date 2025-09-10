/**
 * @file LPC1769_registers.c
 * @brief Controls a 7-segment display using GPIO pins on the LPC1769 board.
 *        Increments the displayed digit (0-F) on each button press (P0.0).
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Button connected to P0.0. */
#define BTN (0)

/** Mask for the button connected. */
#define BTN_BIT  BIT_MASK(BTN)
/** Mask for a 7 segments display. */
#define SVN_SEGS BITS_MASK(7, 0)

/** PCB mask for the button connected. */
#define BTN_PCB      BITS_MASK(2, BTN * 2)
/** PCB mask for a 7 segments display. */
#define SVN_SEGS_PCB BITS_MASK(14, 0)

/** Number of elements in the digits array. */
#define DIGITS_SIZE (sizeof(digits) / sizeof(digits[0]))

/** Delay constant for LED timing. */
#define DELAY 2500

/**
 * @brief Configures GPIO pins P2.0-P2.6 as outputs to control a 7-segment display.
 *
 * Sets the pin function to GPIO and sets the direction to output.
 * Turns off all segments initially.
 */
void configGPIO(void);

/**
 * @brief Waits for a debounced button press on P0.0.
 * @return 1 if a valid press was detected.
 */
uint8_t debounceButton(void);

/**
 * @brief Generates a blocking delay using nested loops.
 */
void delay();

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int main(void) {
    configGPIO();

    uint32_t i = 1;

    while (1) {
        if (debounceButton()) {
            LPC_GPIO2->FIOCLR = SVN_SEGS;                   // Turns off all segments.
            LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Sets segments for current digit.
            i++;
        }
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(BTN_PCB);     // P0.0 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(BTN_PCB);    // P0.0 with pull-up.
    LPC_GPIO0->FIODIR &= ~(BTN_BIT);       // P0.0 as input.

    LPC_PINCON->PINSEL4 &= ~(SVN_SEGS_PCB);    // P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SVN_SEGS;             // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SEGS;     // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[0];    // Start with digit 0.
}

uint8_t debounceButton(void) {
    if (!(LPC_GPIO0->FIOPIN & BTN_BIT)) {    // Button pressed.
        delay();                             // Debounce delay

        if (!(LPC_GPIO0->FIOPIN & BTN_BIT)) {            // Confirm still pressed
            while (!(LPC_GPIO0->FIOPIN & BTN_BIT)) {}    // Wait for release
            return 1;
        }
    }
    return 0;
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++)
            __NOP();
}
