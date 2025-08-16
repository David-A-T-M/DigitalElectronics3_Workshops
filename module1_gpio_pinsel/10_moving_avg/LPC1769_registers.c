/**
 * @file LPC1769_registers.c
 * @brief Implements an 8-sample moving average calculator using GPIO on LPC1769.
 */

#include "LPC17xx.h"

/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Bit mask for the input pins (P0.0-P0.7). */
#define INPUT_MASK          BITS_MASK(8, 0)
/** Bit mask for the output pins (P2.0-P2.7). */
#define OUTPUT_MASK         BITS_MASK(8, 0)
/** Double bit mask for the input pins (P0.0-P0.7). */
#define INPUT_MASK_DB       BITS_MASK(16, 0)
/** Double bit mask for the output pins (P2.0-P2.7). */
#define OUTPUT_MASK_DB      BITS_MASK(16, 0)

/** Size of the moving average buffer. */
#define BUFFER_SIZE         8
/** Delay constant for LED timing. */
#define DELAY               2500

/**
 * @brief Configures GPIO pins P2.0-P2.6 as outputs to control a 7-segment display.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    uint32_t i = 0;
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint16_t sum = 0;

    while (1) {
        sum -= buffer[i % BUFFER_SIZE];             // Subtract the oldest sample.
        buffer[i % BUFFER_SIZE] = LPC_GPIO0->FIOPIN & INPUT_MASK;   // Read new sample.
        sum += buffer[i % BUFFER_SIZE];             // Add the new sample.

        const uint8_t avg = sum / BUFFER_SIZE;      // Calculate the average.

        LPC_GPIO2->FIOCLR = OUTPUT_MASK;
        LPC_GPIO2->FIOSET = avg;

        i++;
        delay();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(INPUT_MASK_DB);        // P0.0-P0.7 as GPIO.
    LPC_PINCON->PINSEL4 &= ~(OUTPUT_MASK_DB);       // P2.0-P2.7 as GPIO.

    LPC_GPIO0->FIODIR &= ~INPUT_MASK;               // P0.0–P0.7 as input.
    LPC_GPIO2->FIODIR |= OUTPUT_MASK;               // P2.0–P2.7 as output.

    LPC_GPIO2->FIOCLR = OUTPUT_MASK;                // Turn off LEDs.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
