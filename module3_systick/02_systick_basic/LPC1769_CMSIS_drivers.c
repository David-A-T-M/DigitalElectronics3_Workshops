/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Basic SysTick and GPIO configuration for LPC1769.
 *
 * This file configures the SysTick timer to generate periodic interrupts
 * and sets up the GPIO pin to control the red LED on the LPC1769 board.
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

/** SysTick desired time interval in milliseconds. */
#define ST_TIME (10)

/**
 * @brief Configures P0.22 as a GPIO output for the red LED.
 *
 * Sets the pin function to GPIO, configures the direction as output,
 * and initializes the LED to the off state.
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

    // Alternatively: this function loads (parameter -1) in the LOAD register. Sets priority to 31.
    // SysTick_Config(1000000);                    // Configure SysTick for 10 ms interval.

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
}

void SysTick_Handler(void) {
    const uint32_t current = GPIO_ReadValue(GPIO_PORT_0);

    GPIO_SetPins(GPIO_PORT_0, ~current & RED_BIT);    // Toggle LED state.
    GPIO_SetPins(GPIO_PORT_0, current & RED_BIT);
}
