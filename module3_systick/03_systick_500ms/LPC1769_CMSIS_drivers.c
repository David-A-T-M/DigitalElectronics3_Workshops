/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief SysTick-based LED blinking with configurable interval for LPC1769.
 *
 * This file provides functions to configure the SysTick timer and GPIO
 * to toggle the red LED (P0.22) at a 500 ms interval.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_systick.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED is connected to P0.22. */
#define RED_LED (22)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT BIT_MASK(RED_LED)
/** PCB mask for the red LED (P0.22). */
#define RED_PCB BITS_MASK(2, (RED_LED - 16) * 2)

/** Blink time in milliseconds. */
#define BLINK_TIME (500)
/** SysTick timer interval in milliseconds. */
#define ST_TIME    (100)

/** Number of SysTick interrupts to achieve the desired blink time. */
#define ST_MULT ((BLINK_TIME / ST_TIME) - 1)

/**
 * @brief Configures the GPIO pin for the red LED as output.
 *
 * Sets P0.22 as a GPIO output and initializes the LED to the off state.
 */
void configGPIO(void);

/**
 * @brief Configures the SysTick timer to generate periodic interrupts.
 *
 * @param time Time interval in milliseconds between SysTick interrupts.
 *
 * Initializes the SysTick timer using CMSIS drivers, sets the interrupt interval,
 * and enables the SysTick interrupt and counter.
 */
void configSysTick(uint32_t time);

int main(void) {
    configGPIO();
    configSysTick(ST_TIME);

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};

    pinCfg.portNum   = PINSEL_PORT_0;
    pinCfg.pinNum    = PINSEL_PIN_22;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                         // P0.22 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, RED_BIT, GPIO_OUTPUT);    // P0.22 as output.

    GPIO_SetPins(GPIO_PORT_0, RED_BIT);    // Turn LED off.
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);    // Initialize SysTick with 10 ms interval.
    SYSTICK_IntCmd(ENABLE);        // Enable SysTick interrupt.
    SYSTICK_Cmd(ENABLE);           // Enable SysTick counter.

    NVIC_EnableIRQ(SysTick_IRQn);    // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    static uint8_t intCount = ST_MULT;

    if (intCount) {
        intCount--;    // Decrement interrupt counter.

        return;
    }

    const uint32_t current = GPIO_ReadValue(GPIO_PORT_0);

    GPIO_SetPins(GPIO_PORT_0, ~current & RED_BIT);
    GPIO_SetPins(GPIO_PORT_0, current & RED_BIT);

    intCount = ST_MULT;    // Reset interrupt counter.
}
