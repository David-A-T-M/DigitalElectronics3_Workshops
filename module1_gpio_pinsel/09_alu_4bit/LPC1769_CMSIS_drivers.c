/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Bit mask for the A operand (P0.0-P0.3). */
#define A_MASK              BITS_MASK(4, 0)
/** Bit mask for the B operand (P0.4-P0.7). */
#define B_MASK              BITS_MASK(4, 4)
/** Bit mask for the operation selector (P0.8). */
#define OP_MASK             BIT_MASK(8)
/** Bit mask for the result display (P2.0-P2.3). */
#define LED_MASK            BITS_MASK(4, 0)
/** Bit mask for the overflow indicator (P2.4). */
#define OVF_LED             BIT_MASK(4)

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

int main(void) {
    configGPIO();

    while (1) {
        uint8_t A = GPIO_ReadValue(GPIO_PORT_0) & A_MASK;           // Read P0.0-P0.3
        uint8_t B = (GPIO_ReadValue(GPIO_PORT_0) & B_MASK) >> 4;    // Read P0.4-P0.7

        if (GPIO_ReadValue(GPIO_PORT_0) & OP_MASK) {                // Read P0.8
            const uint8_t result = add(A, B);                       // Perform addition.
            GPIO_ClearPins(GPIO_PORT_2, ~result);
            GPIO_SetPins(GPIO_PORT_2, result);
        } else {
            const uint8_t result = subtract(A, B);                  // Perform subtraction.
            GPIO_ClearPins(GPIO_PORT_2, ~result);
            GPIO_SetPins(GPIO_PORT_2, result);
        }
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};

    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, A_MASK | B_MASK | OP_MASK);  // P0.0-P0.8 as GPIO with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, LED_MASK | OVF_LED);         // P2.0-P2.4 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, A_MASK | B_MASK | OP_MASK, GPIO_INPUT);// P0.0-P0.8 as input.
    GPIO_SetDir(GPIO_PORT_2, LED_MASK | OVF_LED, GPIO_OUTPUT);      // P2.0-P2.4 as output.
}

uint8_t add(uint8_t A, uint8_t B) {
    uint8_t result = A + B;                                         // Perform addition.

    if (result > 0x0F) {                                            // Check for overflow.
        GPIO_SetPins(GPIO_PORT_2, OVF_LED);                         // Set overflow LED.
        result &= 0x0F;                                             // Mask to 4 bits.
    } else {
        GPIO_ClearPins(GPIO_PORT_2, OVF_LED);                       // Clear overflow LED.
    }
    return result;
}

uint8_t subtract(uint8_t A, uint8_t B) {
    uint8_t result;
    if (B > A) {                                                    // Check for negative result.
        GPIO_SetPins(GPIO_PORT_2, OVF_LED);                         // Set overflow LED.
        result = B - A;                                             // Absolute value of subtraction.
    } else {
        GPIO_ClearPins(GPIO_PORT_2, OVF_LED);                       // Clear overflow LED.
        result = A - B;                                             // Normal subtraction.
    }
    return result;
}
