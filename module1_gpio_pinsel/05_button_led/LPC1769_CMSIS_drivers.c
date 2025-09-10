/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Detects a button press on P0.0 and controls an LED on P2.0 using driver abstraction.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x) (0x1 << (x))

/** Button connected to P0.0. */
#define BTN (0)
/** LED connected to P2.0. */
#define LED (0)

/** Mask for the button. */
#define BTN_BIT BIT_MASK(BTN)
/** Mask for the LED. */
#define LED_BIT BIT_MASK(LED)

/**
 * @brief Configures GPIO pins for button input (P0.0) and LED output (P2.0).
 *
 * Sets up the pin function, mode, and direction for the button and LED.
 * Ensures the LED is turned off initially.
 */
void configGPIO(void);

int main(void) {
    configGPIO();

    while (1) {
        if (GPIO_ReadValue(GPIO_PORT_0) & BTN_BIT)
            GPIO_SetPins(GPIO_PORT_2, LED_BIT);    // Turn on LED.
        else
            GPIO_ClearPins(GPIO_PORT_2, LED_BIT);    // Turn off LED.
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type PinCfg = {0};    // PINSEL configuration structure.

    PinCfg.portNum   = PINSEL_PORT_0;
    PinCfg.pinNum    = PINSEL_PIN_0;
    PinCfg.funcNum   = PINSEL_FUNC_0;
    PinCfg.pinMode   = PINSEL_PULLUP;
    PinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&PinCfg);    // P0.0 as GPIO with pull-up.

    PinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigPin(&PinCfg);    // P2.0 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, BTN_BIT, GPIO_INPUT);     // P0.0 as input.
    GPIO_SetDir(GPIO_PORT_2, LED_BIT, GPIO_OUTPUT);    // P2.0 as output.

    GPIO_ClearPins(GPIO_PORT_2, LED_BIT);    // Turn off LED.
}
