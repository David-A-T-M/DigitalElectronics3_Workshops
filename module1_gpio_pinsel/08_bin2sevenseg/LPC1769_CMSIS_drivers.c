/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief GPIO and 7-segment display control for LPC1769.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Mask for a 7 segments display. */
#define SVN_SEGS   BITS_MASK(7, 0)
/** Mask for input pins P0.0-P0.3. */
#define INPUT_PINS BITS_MASK(4, 0)

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
        uint32_t value = GPIO_ReadValue(GPIO_PORT_0) & INPUT_PINS;    // Read P0.0-P0.3.
        GPIO_WriteValue(GPIO_PORT_2, digits[value]);                  // Display the value on the 7-segment display.
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};    // PINSEL configuration structure.

    pinCfg.portNum   = PINSEL_PORT_0;
    pinCfg.pinNum    = PINSEL_PIN_0;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, INPUT_PINS);    // P0.0-P0.3 as GPIO with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, SVN_SEGS);    // P2.0-P2.6 as GPIO

    GPIO_SetDir(GPIO_PORT_0, INPUT_PINS, GPIO_INPUT);    // P0.0-P0.3 as input.
    GPIO_SetDir(GPIO_PORT_2, SVN_SEGS, GPIO_OUTPUT);     // P2.0-P2.6 as output.

    GPIO_ClearPins(GPIO_PORT_2, SVN_SEGS);    // Turn off all segments.
}
