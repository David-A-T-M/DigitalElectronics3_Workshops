/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Hexadecimal counter on 7-segment display using SysTick for LPC1769.
 *
 * This file configures the SysTick timer and GPIO to display a hexadecimal
 * counter (0-F) on a 7-segment display, incrementing every second.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_systick.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Seven segments display connected to P2.0-P2.6. */
#define SVN_SGS (0)

/** Mask for a 7 segments display. */
#define SVN_SGS_BITS BITS_MASK(7, SVN_SGS)

/** Number of elements in the digits array. */
#define DIGITS_SIZE (sizeof(digits) / sizeof(digits[0]))

/** Blink time in milliseconds. */
#define COUNT_TIME (1000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME    (100)

/** Number of SysTick interrupts to achieve the desired count time. */
#define ST_MULT ((COUNT_TIME / ST_TIME) - 1)

/**
 * @brief Configures GPIO pins for 7-segment display output.
 *
 * Sets P2.0-P2.6 as GPIO outputs and initializes the display to show 0.
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

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/** Current index for the digits array. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configSysTick(ST_TIME);

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};    // PINSEL configuration structure.

    pinCfg.portNum   = PINSEL_PORT_2;
    pinCfg.pinNum    = PINSEL_PIN_0;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, SVN_SGS_BITS);       // P2.0-P2.6 as GPIO.
    GPIO_SetDir(GPIO_PORT_2, SVN_SGS_BITS, GPIO_OUTPUT);    // P2.0-P2.6 as output.

    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);             // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);    // Start with digit 0.
    i++;
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);    // Initialize SysTick interval.
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

    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);             // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);    // Sets segments for current digit.

    intCount = ST_MULT;    // Reset interrupt counter.
    i++;
}
