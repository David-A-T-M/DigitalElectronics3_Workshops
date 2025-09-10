/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Demonstrates configuring GPIO and external interrupts on the LPC1769.
 *
 * This example sets up a red LED on P0.22 and a button on P2.10.
 * Pressing the button triggers an external interrupt (EINT0) that toggles the LED.
 */

#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED connected to P0.22. */
#define RED_LED (22)
/** Button connected to P2.10. */
#define BTN     (10)

/** Mask for the red LED (P0.22). */
#define RED_BIT   BIT_MASK(RED_LED)
/** Mask for the button connected. */
#define BTN_BIT   BIT_MASK(BTN)
/** Mask for the EINT0 interrupt. */
#define EINT0_BIT BIT_MASK(0)

/**
 * @brief Initializes GPIO pins for the red LED and button.
 *
 * Configures P0.22 as a GPIO output for the red LED and P2.10 as an input with pull-up for the button.
 * Sets P2.10 to function as an external interrupt (EINT0) source.
 * Ensures the LED is initially turned off.
 */
void configGPIO(void);

/**
 * @brief Sets up the external interrupt for the button on P2.10 (EINT0).
 *
 * Configures EINT0 to trigger on a falling edge, clears any pending interrupt flags,
 * and enables the interrupt in the NVIC.
 */
void configInt(void);

int main(void) {
    configGPIO();
    configInt();

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};

    pinCfg.portNum   = PINSEL_PORT_0;
    pinCfg.pinNum    = PINSEL_PIN_0;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                         // Configure P0.22 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, RED_BIT, GPIO_OUTPUT);    // Set P0.22 as output.

    pinCfg.portNum = PINSEL_PORT_2;
    pinCfg.pinNum  = PINSEL_PIN_10;
    pinCfg.funcNum = PINSEL_FUNC_1;

    PINSEL_ConfigPin(&pinCfg);                        // Configure P2.10 as EINT0 with pull-up.
    GPIO_SetDir(GPIO_PORT_2, BTN_BIT, GPIO_INPUT);    // Set P2.10 as input.

    GPIO_ClearPins(GPIO_PORT_0, RED_BIT);    // Turn off the red LED.
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};
    extiCfg.line          = EXTI_EINT0;
    extiCfg.mode          = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity      = EXTI_FALLING_EDGE;

    EXTI_ConfigEnable(&extiCfg);    // Configure EINT0 for falling edge.
}

void EINT0_IRQHandler(void) {
    const uint32_t current = GPIO_ReadValue(GPIO_PORT_2);

    GPIO_SetPins(GPIO_PORT_0, ~current & RED_BIT);    // Toggle LED state.
    GPIO_ClearPins(GPIO_PORT_0, current & RED_BIT);

    EXTI_ClearFlag(EXTI_EINT0);    // Clear EINT0 interrupt flag.
}
