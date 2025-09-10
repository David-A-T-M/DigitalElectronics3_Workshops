/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Implements an 8-sample moving average calculator using GPIO on LPC1769.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Bit mask for the input pins (P0.0-P0.7). */
#define INPUT_MASK  BITS_MASK(8, 0)
/** Bit mask for the output pins (P2.0-P2.7). */
#define OUTPUT_MASK BITS_MASK(8, 0)

/** Size of the moving average buffer. */
#define BUFFER_SIZE 8
/** Delay constant for LED timing. */
#define DELAY       2500

/**
 * @brief @brief Configures GPIO pins P0.0–P0.7 as inputs for sampling and P2.0–P2.7 as outputs
 * for displaying the moving average result.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    uint32_t i                  = 0;
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint16_t sum                = 0;

    while (1) {
        sum -= buffer[i % BUFFER_SIZE];                                        // Subtract the oldest sample.
        buffer[i % BUFFER_SIZE] = GPIO_ReadValue(GPIO_PORT_0) & INPUT_MASK;    // Read new sample.
        sum += buffer[i % BUFFER_SIZE];                                        // Add the new sample.

        const uint8_t avg = sum / BUFFER_SIZE;    // Calculate the average.

        GPIO_ClearPins(GPIO_PORT_2, OUTPUT_MASK);
        GPIO_SetPins(GPIO_PORT_2, avg);

        i++;
        delay();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};    // PINSEL configuration structure.

    pinCfg.pinNum    = PINSEL_PIN_0;
    pinCfg.portNum   = PINSEL_PORT_0;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, INPUT_MASK);    // P0.0-P0.7 as GPIO inputs with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, OUTPUT_MASK);    // P2.0-P2.7 as GPIO outputs.

    GPIO_SetDir(GPIO_PORT_0, INPUT_MASK, GPIO_INPUT);      // P0.0–P0.7 as input.
    GPIO_SetDir(GPIO_PORT_2, OUTPUT_MASK, GPIO_OUTPUT);    // P2.0–P2.7 as output.

    GPIO_ClearPins(GPIO_PORT_2, OUTPUT_MASK);    // Turn off LEDs.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++)
            __NOP();
}
