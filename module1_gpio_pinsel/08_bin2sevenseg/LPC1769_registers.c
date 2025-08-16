/**
 * @file LPC1769_registers.c
 * @brief GPIO and 7-segment display control for LPC1769.
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

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
        uint32_t value = GPIO_ReadValue(GPIO_PORT_0) & INPUT_PINS_MASK(0); // Read P0.0-P0.3
        GPIO_WriteValue(GPIO_PORT_2, digits[value]); // Display the value on the 7-segment display
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};
    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, INPUT_PINS_MASK(0));     // Configure P0.0-P0.3 as GPIO with pull-up

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, SEGMENTS_MASK(0));       // Configure P2.0-P2.6 as GPIO

    GPIO_SetDir(GPIO_PORT_0, INPUT_PINS_MASK(0), GPIO_INPUT);   // Set P0.0-P0.3 as input
    GPIO_SetDir(GPIO_PORT_2, SEGMENTS_MASK(0), GPIO_OUTPUT);    // Set P2.0-P2.6 as output

    GPIO_ClearPins(GPIO_PORT_2, SEGMENTS_MASK(0));              // Turn off all segments initially
}
