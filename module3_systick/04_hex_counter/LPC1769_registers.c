/**
 * @file LPC1769_registers.c
 * @brief Hexadecimal counter on 7-segment display using SysTick for LPC1769.
 *
 * This file configures the SysTick timer and GPIO to display a hexadecimal
 * counter (0-F) on a 7-segment display, incrementing every second.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Seven segments display connected to P2.0-P2.6. */
#define SVN_SGS             (0)

/** Mask for a 7 segments display. */
#define SVN_SGS_BITS        BITS_MASK(7, SVN_SGS)

/** PCB mask for a 7 segments display. */
#define SVN_SGS_PCB         BITS_MASK(14, SVN_SGS * 2)

/** Number of elements in the digits array. */
#define DIGITS_SIZE         (sizeof(digits) / sizeof(digits[0]))

/** Blink time in milliseconds. */
#define COUNT_TIME          (1000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME             (100)

/** SysTick load value for the desired time interval. */
#define ST_LOAD             ((ST_TIME * 100000) - 1)
/** Number of SysTick interrupts to achieve the desired count time. */
#define ST_MULT             ((COUNT_TIME/ST_TIME) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE           BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT          BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE        BIT_MASK(2)

/**
 * @brief Configures GPIO pins for 7-segment display output.
 *
 * Sets P2.0-P2.6 as GPIO outputs and initializes the display to show 0.
 */
void configGPIO(void);

/**
 * @brief Configures the SysTick timer to generate periodic interrupts.
 *
 * @param ticks Number of clock cycles between SysTick interrupts.
 *
 * Loads the specified value, clears the current counter, and enables
 * the SysTick timer and its interrupt.
 */
void configSysTick(uint32_t ticks);

/** Values for hexadecimal digits (0-F). */
const uint32_t digits[] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
                           0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

/** Current index for the digits array. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configSysTick(ST_LOAD);

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL4 &= ~(SVN_SGS_PCB);          // P2.0-P2.6 as GPIO.
    LPC_GPIO2->FIODIR |= SVN_SGS_BITS;              // P2.0-P2.6 as output.

    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Start with digit 0.
    i++;
}

void configSysTick(uint32_t ticks) {
    SysTick->LOAD = ticks;                          // Load value for interval.
    SysTick->VAL = 0;                               // Clear current value and interrupt flag.
    SysTick->CTRL = ST_ENABLE |                     // Enable SysTick interrupt.
                    ST_TICKINT |                    // Enable SysTick exception request.
                    ST_CLKSOURCE;                   // Use processor clock.

    NVIC_EnableIRQ(SysTick_IRQn);                   // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    static uint8_t intCount = ST_MULT;

    if (intCount) {
        intCount--;                                 // Decrement interrupt counter.

        return;
    }

    LPC_GPIO2->FIOCLR = SVN_SGS_BITS;               // Turns off all segments.
    LPC_GPIO2->FIOSET = digits[i % DIGITS_SIZE];    // Sets segments for current digit.

    intCount = ST_MULT;                             // Reset interrupt counter.
    i++;
}
