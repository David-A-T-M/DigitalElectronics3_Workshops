/**
 * @file LPC1769_registers.c
 * @brief Implements a hexadecimal counter on a 7-segment display,
 *        incremented in main and decremented by a button interrupt using direct register access.
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

/** Button connected to P2.13. */
#define BTN                 (13)
#define SVN_SGS             (0)

/** Mask for the button connected. */
#define BTN_BIT             BIT_MASK(BTN)
/** Mask for the EINT3 interrupt. */
#define EINT3_BIT           BIT_MASK(3)
/** Mask for a 7 segments display. */
#define SVN_SGS_BITS        BITS_MASK(7, 0)


/** Double bit mask for the button connected. */
#define BTN_PCB             BITS_MASK(2, BTN * 2)
#define BTN_PCB_L           BIT_MASK(BTN * 2)
/** Double bit mask for a 7 segments display. */
#define SVN_SGS_PCB         BITS_MASK(14, 0)

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
void delay(void);

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/** Current index for the digits array. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configInt();

    while(1) {
        LPC_GPIO2->FIOCLR = SVN_SGS_BITS;                   // Turns off all segments.
        LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Sets segments for current digit.

        delay();

        i++;
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL4 &= ~(BTN_PCB);
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;               // P2.13 as EINT3.
    LPC_PINCON->PINMODE4 &= ~(BTN_PCB);             // P2.13 with pull-up.
    LPC_GPIO2->FIODIR &= ~(BTN_BIT);                // P2.13 as input.

    LPC_PINCON->PINSEL4 &= ~(SVN_SGS_PCB);          // P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SVN_SGS_BITS;              // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Start with digit 0.
    i++;
}

void configInt(void) {
    LPC_SC->EXTMODE &= ~(EINT3_BIT);                // EINT3 level-sensitive.
    LPC_SC->EXTPOLAR &= ~(EINT3_BIT);               // EINT3 low-active.

    LPC_SC->EXTINT |= EINT3_BIT;                    // Clear any pending EINT3 interrupt.
    NVIC_ClearPendingIRQ(EINT3_IRQn);               // Clear any pending EINT3 interrupt.
    NVIC_EnableIRQ(EINT3_IRQn);                     // Enable EINT3 interrupt in NVIC.
}

void EINT3_IRQHandler(void) {
    i--;
    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Sets segments for current digit.

    delay();

    LPC_SC->EXTINT |= EINT3_BIT;                    // Clear any pending EINT3 interrupt.
}

void delay(void) {
    for (volatile uint32_t j = 0; j < DELAY; j++)
        for (volatile uint32_t k = 0; k < DELAY; k++);
}
