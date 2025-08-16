/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

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
        uint8_t A = GPIO_ReadValue(GPIO_PORT_0) & A_MASK;
        uint8_t B = (GPIO_ReadValue(GPIO_PORT_0) & B_MASK) >> 4;
        if (GPIO_ReadValue(GPIO_PORT_0) & OP_MASK) {
            const uint8_t result = add(A, B);
            GPIO_ClearPins(GPIO_PORT_2, ~result);
            GPIO_SetPins(GPIO_PORT_2, result);
        } else {
            const uint8_t result = subtract(A, B);
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

    PINSEL_ConfigMultiplePins(&pinCfg, A_MASK | B_MASK | OP_MASK); // Configure P0.0-P0.8

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, LED_MASK | OVF_LED); // Configure P2.0-P2.4

    GPIO_SetDir(GPIO_PORT_0, A_MASK | B_MASK | OP_MASK, GPIO_INPUT); // Set P0.0-P0.8 as input
    GPIO_SetDir(GPIO_PORT_2, LED_MASK | OVF_LED, GPIO_OUTPUT); // Set P2.0-P2.4 as output
}

uint8_t add(uint8_t A, uint8_t B) {
    uint8_t result = A + B;

    if (result > 0x0F) {
        GPIO_SetPins(GPIO_PORT_2, OVF_LED); // Set overflow LED
        result &= 0x0F; // Mask result to fit in 4 bits
    } else {
        GPIO_ClearPins(GPIO_PORT_2, OVF_LED); // Clear overflow LED
    }
    return result;
}

uint8_t subtract(uint8_t A, uint8_t B) {
    uint8_t result;
    if (B > A) {
        GPIO_SetPins(GPIO_PORT_2, OVF_LED); // Set overflow LED
        result = B - A;
    } else {
        GPIO_ClearPins(GPIO_PORT_2, OVF_LED); // Clear overflow LED
        result = A - B; // Normal subtraction
    }
    return result;
}
