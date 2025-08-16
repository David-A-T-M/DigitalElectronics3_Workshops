/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Program to blink a single color on the onboard RGB LED of the LPC1769 board.
 *
 * This program configures the GPIO pin connected to the red channel of the onboard RGB LED.
 * It toggles the red LED on and off in a loop, creating a blinking effect.
 * The delay is implemented using a blocking nested loop.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))

/** Bit mask for the red LED (P0.22). */
#define RED_LED             BIT_MASK(22)

/** Delay constant for LED timing. */
#define DELAY               2500

/**
 * @brief Configures the GPIO pin for the red LED as output.
 *
 * Sets the pin function to GPIO and configures the direction as output.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    while(1) {
        GPIO_ClearPins(GPIO_PORT_0, RED_LED);       // Turn red LED on.
        delay();

        GPIO_SetPins(GPIO_PORT_0, RED_LED);         // Turn red LED off.
        delay();
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type PinCfg = {0};                   // Initialize the PINSEL configuration structure.

    PinCfg.portNum = PINSEL_PORT_0;
    PinCfg.pinNum = PINSEL_PIN_22;
    PinCfg.funcNum = PINSEL_FUNC_0;
    PinCfg.pinMode = PINSEL_PULLUP;
    PinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&PinCfg);                      // Configure P0.22 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, RED_LED, GPIO_OUTPUT); // Set P0.22 as output.

    GPIO_SetPins(GPIO_PORT_0, RED_LED);             // Force red LED off initially.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
