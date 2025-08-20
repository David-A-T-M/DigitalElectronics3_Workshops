/**
 * @file LPC1769_registers.c
 * @brief Implements a hexadecimal counter on a 7-segment display,
 *        incremented by a button interrupt using direct register access.
 *
 * This file configures GPIO pins for a button and a 7-segment display,
 * sets up an interrupt for the button, and handles the display update
 * in the interrupt handler.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

#define BTN                 (0)
#define SVN_SGS             (0)

/** Mask for the button connected. */
#define BTN_BIT             BIT_MASK(BTN)
/** Mask for a 7 segments display. */
#define SVN_SGS_BITS        BITS_MASK(7, SVN_SGS)

/** PCB for the button connected. */
#define BTN_PCB             BITS_MASK(2, BTN * 2)
/** PCB mask for a 7 segments display. */
#define SVN_SGS_PCB         BITS_MASK(14, SVN_SGS * 2)

/** Number of elements in the digits array. */
#define DIGITS_SIZE         (sizeof(digits) / sizeof(digits[0]))

/**
 * @brief Configures GPIO pins for button input and 7-segment display output.
 */
void configGPIO(void);

/**
 * @brief Configures GPIO interrupt for the button.
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

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(BTN_PCB);              // P0.0 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(BTN_PCB);             // P0.0 with pull-up.
    LPC_GPIO0->FIODIR &= ~(BTN_BIT);                // P0.0 as input.

    LPC_PINCON->PINSEL4 &= ~(SVN_SGS_PCB);          // P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SVN_SGS_BITS;              // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Start with digit 0.
    i++;
}

void configInt(void) {
    LPC_GPIOINT->IO0IntEnR |= BTN_BIT;              // Enable rising edge interrupt on P0.0.

    NVIC_EnableIRQ(EINT3_IRQn);                     // Enable EINT3 interrupt in NVIC.
}

void EINT3_IRQHandler(void) {
    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Sets segments for current digit.
    i++;

    LPC_GPIOINT->IO0IntClr = BTN_BIT;               // Clears the interrupt flag for P0.0.
}
