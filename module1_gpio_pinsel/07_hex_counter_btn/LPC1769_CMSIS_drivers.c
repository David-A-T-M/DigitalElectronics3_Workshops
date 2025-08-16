/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Controls a 7-segment display using GPIO pins on the LPC1769 board.
 *        Increments the displayed digit (0-F) on each button press (P0.0).
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/**
 * @def DELAY
 * @brief Delay constant for LED timing.
 */
#define DELAY 2500

/**
 * @def SEGMENTS_MASK(n)
 * @brief Mask for 7 segments of a display starting at bit n.
 */
#define SEGMENTS_MASK(n)   (0x7F << (n))

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
 * @brief Waits for a debounced button press on P0.0.
 * @return 1 if a valid press was detected.
 */
uint8_t debounceButton(void);

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

    uint32_t i = 0;

    while (1) {
        if (debounceButton()) {
            GPIO_WriteValue(GPIO_PORT_2, digits[i % DIGITS_SIZE]);
            i++;
        }
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

    PINSEL_ConfigPin(&pinCfg);                                  // Configure P0.0 as GPIO with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, SEGMENTS_MASK(0));       // Configure P2.0-P2.6 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, (0x1 << BUTTON_PIN), GPIO_INPUT);  // Set P0.0 as input.
    GPIO_SetDir(GPIO_PORT_2, SEGMENTS_MASK(0), GPIO_OUTPUT);    // Set P2.0-P2.6 as output.

    GPIO_ClearPins(GPIO_PORT_2, SEGMENTS_MASK(0));              // Turn off all segments initially.
}

uint8_t debounceButton(void) {
    if (!(GPIO_ReadValue(GPIO_PORT_0) & (0x1 << BUTTON_PIN))) {             // Button pressed.
        delay();                                                            // Debounce delay
        if (!(GPIO_ReadValue(GPIO_PORT_0) & (0x1 << BUTTON_PIN))) {         // Confirm still pressed
            while (!(GPIO_ReadValue(GPIO_PORT_0) & (0x1 << BUTTON_PIN))) {} // Wait for release
            return 1;
        }
    }
    return 0;
}

void delay() {
    for (uint32_t i = 0; i < DELAY; i++)
        for (uint32_t j = 0; j < DELAY; j++);
}
