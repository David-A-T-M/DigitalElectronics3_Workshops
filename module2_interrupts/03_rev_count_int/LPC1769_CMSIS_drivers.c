/**
 * @file LPC1769_registers.c
 * @brief Implements a hexadecimal counter on a 7-segment display,
 *        incremented in main and decremented by a button interrupt using drivers.
 *
 * This file configures GPIO pins for a button and a 7-segment display,
 * sets up an interrupt for the button, and handles the display update
 * in the interrupt handler.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Button connected to P2.13. */
#define BTN                 (13)
#define SVN_SGS             (0)

#define BTN_BIT             BIT_MASK(BTN)
/** Mask for the EINT3 interrupt. */
#define EINT3_BIT           BIT_MASK(3)
/** Mask for a 7 segments display. */
#define SVN_SGS_BITS        BITS_MASK(7, 0)

/** Number of elements in the digits array. */
#define DIGITS_SIZE         (sizeof(digits) / sizeof(digits[0]))

/** Delay constant for LED timing. */
#define DELAY               2500

/**
 * @brief Configures GPIO pins for button input and 7-segment display output.
 */
void configGPIO(void);

/**
 * @brief Configures EINT3 interrupt for the button.
 */
void configInt(void);

/**
 * @brief Generates a blocking delay using nested loops.
 */
void delay();

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/** Current index for the digits array. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configInt();

    while(1) {
        GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);          // Clears all segments.
        GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]); // Sets segments for current digit.

        delay();

        i++;
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                           // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_2;
    pinCfg.pinNum = PINSEL_PIN_13;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, SVN_SGS_BITS);       // P2.0-P2.6 as GPIO.

    pinCfg.funcNum = PINSEL_FUNC_1;
    PINSEL_ConfigPin(&pinCfg);                              // P2.13 as EINT3.

    GPIO_SetDir(GPIO_PORT_2, BTN_BIT, GPIO_INPUT);          // P2.13 as input.
    GPIO_SetDir(GPIO_PORT_2, SVN_SGS_BITS, GPIO_OUTPUT);    // P2.0-P2.6 as output.

    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);              // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);     // Start with digit 0.
    i++;
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};                            // EXTI configuration structure.
    extiCfg.line = EXTI_EINT3;                              // EINT3 line.
    extiCfg.mode = EXTI_LEVEL_SENSITIVE;                    // Level-sensitive mode.
    extiCfg.polarity = EXTI_LOW_ACTIVE;                     // Low active polarity.

    EXTI_ConfigEnable(&extiCfg);                            // Configure EINT3.
}

void EINT3_IRQHandler(void) {
    i--;
    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);              // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);     // Start with digit 0.

    delay();

    EXTI_ClearFlag(EXTI_EINT3);                             // Clear the EINT3 interrupt flag.
}

void delay() {
    for (volatile uint32_t j = 0; j < DELAY; j++)
        for (volatile uint32_t k = 0; k < DELAY; k++);
}
