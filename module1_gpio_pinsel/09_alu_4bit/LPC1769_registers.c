/**
 * @file LPC1769_registers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Bit mask for the A operand (P0.0-P0.3). */
#define A_MASK   BITS_MASK(4, 0)
/** Bit mask for the B operand (P0.4-P0.7). */
#define B_MASK   BITS_MASK(4, 4)
/** Bit mask for the operation selector (P0.8). */
#define OP_MASK  BIT_MASK(8)
/** Bit mask for the result display (P2.0-P2.3). */
#define LED_MASK BITS_MASK(4, 0)
/** Bit mask for the overflow indicator (P2.4). */
#define OVF_LED  BIT_MASK(4)

/** Double bit mask for the A operand (P0.0-P0.3). */
#define A_MASK_DB   BITS_MASK(8, 0)
/** Double bit mask for the B operand (P0.4-P0.7). */
#define B_MASK_DB   BITS_MASK(8, 8)
/** Double bit mask for the operation selector (P0.8). */
#define OP_MASK_DB  BITS_MASK(2, 16)
/** Double bit mask for the result display (P2.0-P2.3). */
#define LED_MASK_DB BITS_MASK(8, 0)
/** Double bit mask for the overflow indicator (P2.4). */
#define OVF_LED_DB  BITS_MASK(2, 8)

/**
 *  @brief Configures GPIO pins P2.0-P2.3 as outputs for the 4-bit ALU result display using LEDs,
 *  and P2.4 as output for the overflow indicator LED.
 */
void configGPIO(void);

/**
 * @brief Adds two 4-bit values and sets the overflow LED if the result exceeds 4 bits.
 * @param A First 4-bit operand.
 * @param B Second 4-bit operand.
 * @return 4-bit result of A + B.
 */
uint8_t add(uint8_t A, uint8_t B);

/**
 * @brief Subtracts two 4-bit values and sets the overflow LED if the result is negative.
 * @param A Minuend (first 4-bit operand).
 * @param B Subtrahend (second 4-bit operand).
 * @return 4-bit absolute value of the subtraction.
 */
uint8_t subtract(uint8_t A, uint8_t B);

int main(void) {
    configGPIO();

    while (1) {
        const uint8_t A = LPC_GPIO0->FIOPIN & A_MASK;           // Read P0.0-P0.3
        const uint8_t B = (LPC_GPIO0->FIOPIN & B_MASK) >> 4;    // Read P0.4-P0.7

        if (LPC_GPIO0->FIOPIN & OP_MASK) {       // Read P0.8
            const uint8_t result = add(A, B);    // Perform addition.
            LPC_GPIO2->FIOCLR    = LED_MASK;
            LPC_GPIO2->FIOSET    = result;
        } else {
            const uint8_t result = subtract(A, B);    // Perform subtraction.
            LPC_GPIO2->FIOCLR    = LED_MASK;
            LPC_GPIO2->FIOSET    = result;
        }
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(A_MASK_DB | B_MASK_DB | OP_MASK_DB);     // P0.0-P0.8 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(A_MASK_DB | B_MASK_DB | OP_MASK_DB);    // P0.0-P0.8 as pull-up.

    LPC_PINCON->PINSEL4 &= ~(LED_MASK_DB | OVF_LED_DB);    // P2.0-P2.4 as GPIO.

    LPC_GPIO0->FIODIR &= ~(A_MASK | B_MASK | OP_MASK);    // P0.0-P0.8 as input.
    LPC_GPIO2->FIODIR |= LED_MASK | OVF_LED;              // P2.0-P2.4 as output.

    LPC_GPIO2->FIOCLR = LED_MASK | OVF_LED;    // Turn off all LEDs.
}

uint8_t add(uint8_t A, uint8_t B) {
    uint8_t result = A + B;    // Perform addition.

    if (result > 0x0F) {                // Check for overflow.
        LPC_GPIO2->FIOSET = OVF_LED;    // Set overflow LED.
        result &= 0x0F;                 // Mask to 4 bits.
    } else {
        LPC_GPIO2->FIOCLR = OVF_LED;    // Clear overflow LED.
    }
    return result;
}

uint8_t subtract(uint8_t A, uint8_t B) {
    uint8_t result;
    if (B > A) {                        // Check for negative result.
        LPC_GPIO2->FIOSET = OVF_LED;    // Set overflow LED.
        result            = B - A;      // Absolute value of subtraction.
    } else {
        LPC_GPIO2->FIOCLR = OVF_LED;    // Clear overflow LED.
        result            = A - B;      // Normal subtraction.
    }
    return result;
}
