/**
 * @file LPC1769_registers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */
#include "LPC17xx.h"

#define A_MASK   0x0F      // P0.0–P0.3
#define B_MASK   0xF0      // P0.4–P0.7
#define OP_MASK  (1 << 8)  // P0.8
#define LED_MASK 0x0F      // P2.0–P2.3
#define OVF_LED  (1 << 4)  // P2.4


/**
 * @brief Configures GPIO pins P2.0-P2.6 as outputs to control a 7-segment display.
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

/**
 * @brief Array of segment values for hexadecimal digits (0-F).
 */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

int main(void) {
    configGPIO();

    while (1) {
        uint8_t A = LPC_GPIO0->FIOPIN & A_MASK;
        uint8_t B = (LPC_GPIO0->FIOPIN & B_MASK) >> 4;
        if (LPC_GPIO0->FIOPIN & OP_MASK) {
            const uint8_t result = add(A, B);
            LPC_GPIO2->FIOCLR = ~result;
            LPC_GPIO2->FIOSET = result;
        } else {
            const uint8_t result = subtract(A, B);
            LPC_GPIO2->FIOCLR = ~result;
            LPC_GPIO2->FIOSET = result;
        }
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(0x3FFFF); // Set P0.0-P0.7 as GPIO
    LPC_PINCON->PINSEL4 &= ~(0x3FF); // Set P2.0-P2.4 as GPIO

    LPC_GPIO0->FIODIR &= ~(A_MASK | B_MASK | OP_MASK);   // Set P0.0-P0.8 as input
    LPC_GPIO2->FIODIR |= LED_MASK | OVF_LED; // Set P2.0-P2.4 as output

    LPC_GPIO2->FIOCLR = LED_MASK | OVF_LED; // Clear P2.0-P2.4
}

uint8_t add(uint8_t A, uint8_t B) {
    uint8_t result = A + B;

    if (result > 0x0F) {
        LPC_GPIO2->FIOSET = OVF_LED; // Set overflow LED
        result &= 0x0F; // Mask result to fit in 4 bits
    } else {
        LPC_GPIO2->FIOCLR = OVF_LED; // Clear overflow LED
    }
    return result;
}

uint8_t subtract(uint8_t A, uint8_t B) {
    uint8_t result;
    if (B > A) {
        LPC_GPIO2->FIOSET = OVF_LED; // Set overflow LED
        result = B - A;
    } else {
        LPC_GPIO2->FIOCLR = OVF_LED; // Clear overflow LED
        result = A - B; // Normal subtraction
    }
    return result;
}
