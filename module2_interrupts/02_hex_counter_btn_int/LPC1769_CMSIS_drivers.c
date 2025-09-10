/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Implements a hexadecimal counter on a 7-segment display using button interrupts.
 *
 * This example configures GPIO pins for a button (P0.0) and a 7-segment display (P2.0-P2.6)
 * on the LPC1769 microcontroller. Pressing the button triggers a GPIO interrupt that increments
 * the displayed hexadecimal digit (0-F) on the 7-segment display.
 */

#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Button connected to P0.0. */
#define BTN     (0)
/** 7-segment display connected to P2.0-P2.6. */
#define SVN_SGS (0)

/** Mask for the button connected. */
#define BTN_BIT      BIT_MASK(BTN)
/** Mask for the seven segments display. */
#define SVN_SGS_BITS BITS_MASK(7, SVN_SGS)

/** Number of elements in the digits array. */
#define DIGITS_SIZE (sizeof(digits) / sizeof(digits[0]))

/**
 * @brief Configures GPIO pins for button input and 7-segment display output.
 *
 * Sets P0.0 as a GPIO input with pull-up for the button.
 * Sets P2.0-P2.6 as GPIO outputs for the 7-segment display.
 * Initializes the display to show the first digit.
 */
void configGPIO(void);

/**
 * @brief Configures GPIO interrupt for the button on P0.0.
 *
 * Enables rising edge interrupt for P0.0 and enables the EINT3 interrupt in the NVIC.
 */
void configInt(void);

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/** Current index for the digits array. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configInt();

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};    // PINSEL configuration structure.

    pinCfg.portNum   = PINSEL_PORT_0;
    pinCfg.pinNum    = PINSEL_PIN_0;
    pinCfg.funcNum   = PINSEL_FUNC_0;
    pinCfg.pinMode   = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);    // Configure P0.0 as GPIO with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;    // Configure P2.0-P2.6.

    PINSEL_ConfigMultiplePins(&pinCfg, SVN_SGS_BITS);    // P2.0-P2.6 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, BTN_BIT, GPIO_INPUT);          // P0.0 as input.
    GPIO_SetDir(GPIO_PORT_2, SVN_SGS_BITS, GPIO_OUTPUT);    // P2.0-P2.6 as output.

    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);             // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);    // Start with digit 0.
    i++;
}

void configInt(void) {
    GPIO_IntCmd(GPIO_PORT_0, BTN_BIT, GPIO_INT_RISING);    // Enable rising edge interrupt on P0.0.

    NVIC_EnableIRQ(EINT3_IRQn);    // Enable EINT3 interrupt in NVIC.
}

void EINT3_IRQHandler(void) {
    GPIO_ClearPins(GPIO_PORT_2, SVN_SGS_BITS);             // Turns off all segments.
    GPIO_SetPins(GPIO_PORT_2, digits[i % DIGITS_SIZE]);    // Sets segments for current digit.
    i++;

    GPIO_ClearInt(GPIO_PORT_0, BTN_BIT);    // Clears the interrupt for P0.0.
}
