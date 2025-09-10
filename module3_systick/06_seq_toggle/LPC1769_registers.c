/**
 * @file LPC1769_registers.c
 * @brief LED sequence with toggle control using SysTick and external interrupt for LPC1769.
 *
 * This file configures the SysTick timer, GPIO, and external interrupt to sequence through
 * eight LEDs (P0.0-P0.7) at 250 ms intervals. The sequence can be started or stopped by
 * pressing the button connected to P2.10 (EINT0).
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** LEDs connected to P0.0-P0.7. */
#define LEDS (0)
/** Button connected to P2.10 (EINT0). */
#define BTN  (10)

/** Mask for all LED bits (P0.0 to P0.7). */
#define LEDS_BIT  BITS_MASK(8, LEDS)
/** Mask for the button bit (P2.10). */
#define BTN_BIT   BIT_MASK(BTN)
/** Mask for EINT0 (P2.10). */
#define EINT0_BIT BIT_MASK(0)

/** PCB mask for all LED bits (P0.0 to P0.7). */
#define LEDS_PCB  BITS_MASK(16, LEDS * 2)
/** PCB mask for the button bit (P2.10). */
#define BTN_PCB   BITS_MASK(2, BTN * 2)
/** PCB lower bit mask for the button (P2.10). */
#define BTN_PCB_L BIT_MASK(BTN * 2)

/** Sequence time in milliseconds. */
#define SEQ_TIME (250)
/** SysTick timer interval in milliseconds. */
#define ST_TIME  (50)

/** SysTick load value for the desired time interval. */
#define ST_LOAD      ((ST_TIME * 100000) - 1)
/** Number of SysTick interrupts to achieve the desired sequence time. */
#define ST_MULT_SEQ  ((SEQ_TIME / ST_TIME) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE    BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT   BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE BIT_MASK(2)

/** Number of elements in the leds array. */
#define LEDS_SIZE (sizeof(leds) / sizeof(leds[0]))

/**
 * @brief Configures GPIO pins for the LED sequence and button input.
 *
 * Sets P0.0-P0.7 as outputs for the LEDs and P2.10 as input for the button (EINT0).
 * Initializes all LEDs to the off state, then turns on the first LED.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt (EINT0) for the button.
 *
 * Sets up EINT0 on P2.10 to trigger on a rising edge and enables its interrupt in the NVIC.
 */
void configInt(void);

/**
 * @brief Configures the SysTick timer to generate periodic interrupts.
 *
 * @param ticks Number of clock cycles between SysTick interrupts.
 *
 * Loads the specified value, clears the current counter, and enables
 * the SysTick timer and its interrupt.
 */
void configSysTick(uint32_t ticks);

/** LEDs array for sequencing. */
const uint8_t leds[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

/** Flag to control the LED sequence (0 = stopped, 1 = running). */
volatile uint8_t flag = 0;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_LOAD);

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~LEDS_PCB;    // P0.0 to P0.7 as GPIO.
    LPC_GPIO0->FIODIR |= LEDS_BIT;       // P0.0 to P0.7 as outputs.

    LPC_PINCON->PINSEL4 &= ~BTN_PCB;
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;    // P2.10 as EINT0.
    LPC_PINCON->PINMODE4 &= ~BTN_PCB;    // P2.10 pull-up resistor.
    LPC_GPIO2->FIODIR &= ~BTN_BIT;       // P2.10 as input.

    LPC_GPIO0->FIOCLR |= LEDS_BIT;    // Turn off all LEDs.
    LPC_GPIO0->FIOSET |= leds[0];     // Turn on first LED.
}

void configInt(void) {
    LPC_SC->EXTMODE &= ~EINT0_BIT;    // EINT0 edge sensitive.
    LPC_SC->EXTPOLAR |= EINT0_BIT;    // EINT0 rising edge.

    LPC_SC->EXTINT |= EINT0_BIT;         // Clear flag.
    NVIC_ClearPendingIRQ(EINT0_IRQn);    // Clear pending interrupt.
    NVIC_EnableIRQ(EINT0_IRQn);          // Enable EINT0 interrupt in NVIC.
}

void configSysTick(uint32_t ticks) {
    SysTick->LOAD = ticks;           // Load value for 100 ms interval.
    SysTick->VAL  = 0;               // Clear current value and interrupt flag.
    SysTick->CTRL = ST_ENABLE |      // Enable SysTick interrupt.
                    ST_TICKINT |     // Enable SysTick exception request.
                    ST_CLKSOURCE;    // Use processor clock.

    NVIC_EnableIRQ(SysTick_IRQn);    // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    static uint32_t i = 0;

    if (!flag)
        return;    // If flag is not set, do nothing.

    static uint8_t seqCount = ST_MULT_SEQ;

    if (!seqCount) {                                // 200 ms elapsed.
        LPC_GPIO0->FIOCLR = leds[i % LEDS_SIZE];    // Turn off current LED.
        i++;                                        // Increment LED index.
        LPC_GPIO0->FIOSET = leds[i % LEDS_SIZE];    // Turn on next LED.

        seqCount = ST_MULT_SEQ;    // Reset sequence counter.
    }

    seqCount--;    // Decrement counter.
}

void EINT0_IRQHandler(void) {
    flag = !flag;    // Toggle flag state.

    LPC_SC->EXTINT |= EINT0_BIT;    // Clear EINT0 flag.
}
