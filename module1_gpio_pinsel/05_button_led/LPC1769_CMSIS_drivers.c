/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Detects a button press on P0.0 and controls an LED on P2.0 using driver abstraction.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/**
 * @def BUTTON_PIN
 * @brief Mask for the button connected.
 */
#define BUTTON_PIN      0

/**
 * @def LED_PIN
 * @brief Mask for the LED connected.
 */
#define LED_PIN         0

/**
 * @brief
 */
void configGPIO(void);

int main(void) {
    configGPIO();

    while (1) {
        if (GPIO_ReadValue(GPIO_PORT_0) & (0x1 << BUTTON_PIN))
            GPIO_SetPins(GPIO_PORT_2, (0x1 << LED_PIN));    // Turn on LED.
        else
            GPIO_ClearPins(GPIO_PORT_2, (0x1 << LED_PIN));  // Turn off LED.
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type PinCfg = {0};                           // Initialization of the PINSEL configuration structure.
    PinCfg.portNum = PINSEL_PORT_0;
    PinCfg.pinNum = PINSEL_PIN_0;
    PinCfg.funcNum = PINSEL_FUNC_0;
    PinCfg.pinMode = PINSEL_PULLUP;
    PinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&PinCfg);                              // P0.0 as GPIO with pull-up (Button).

    PinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigPin(&PinCfg);                              // P2.0 as GPIO (LED).

    GPIO_SetDir(GPIO_PORT_0, (0x1 << BUTTON_PIN), GPIO_INPUT);  // P0.0 as input (Button)
    GPIO_SetDir(GPIO_PORT_2, (0x1 << LED_PIN), GPIO_OUTPUT);    // P2.0 as output (LED)

    GPIO_ClearPins(GPIO_PORT_2, (0x1 << LED_PIN));              // Turn off LED at start
}
