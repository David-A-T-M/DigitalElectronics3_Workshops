/**
 * @file LPC1769_registers.c
 * @brief Multitask: LED blink and sequence using SysTick for LPC1769.
 *
 * This file configures the SysTick timer and GPIO to perform two tasks:
 * - Blink the red LED (P0.22) every 500 ms.
 * - Sequence through four LEDs (P2.0-P2.3), advancing every 200 ms.
 * Both tasks are managed using counters in the SysTick interrupt handler.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED is connected to P0.22. */
#define RED_LED (22)
/** Four LEDs are connected to P2.0-P2.3. */
#define LEDS    (0)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT  BIT_MASK(RED_LED)
/** Bit mask for the four LEDs (P2.0-P2.3). */
#define LEDS_BIT BITS_MASK(4, LEDS)

/** PCB mask for the red LED (P0.22). */
#define RED_PCB  BITS_MASK(2, (RED_LED - 16) * 2)
/** PCB mask for the four LEDs (P2.0-P2.3). */
#define LEDS_PCB BITS_MASK(2, LEDS * 2)

/** Sequence time in milliseconds. */
#define SEQ_TIME   (500)
/** Blink time in milliseconds. */
#define BLINK_TIME (200)
/** SysTick timer interval in milliseconds. */
#define ST_TIME    (100)

/** SysTick load value for the desired time interval. */
#define ST_LOAD       ((ST_TIME * 100000) - 1)
/** Number of SysTick interrupts to achieve the desired blink time. */
#define ST_MULT_BLINK ((BLINK_TIME / ST_TIME) - 1)
/** Number of SysTick interrupts to achieve the desired sequence time. */
#define ST_MULT_SEQ   ((SEQ_TIME / ST_TIME) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE     BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT    BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE  BIT_MASK(2)

/** Number of LEDs in the sequence. */
#define LEDS_SIZE (sizeof(leds) / sizeof(leds[0]))

/**
 * @brief Configures GPIO pins for the red LED and LED sequence as outputs.
 *
 * Sets P0.22 as output for the red LED and P2.0-P2.3 as outputs for the LED sequence.
 * Initializes all LEDs to the off state, then turns on the first sequence LED.
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

/** Array of LED bit masks for the sequence (P2.0-P2.3). */
const uint8_t leds[] = {0x1, 0x2, 0x4, 0x8};

/** Index for the current LED in the sequence. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configSysTick(ST_LOAD);

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~RED_PCB;    // P0.22 as GPIO.
    LPC_GPIO0->FIODIR |= RED_BIT;       // P0.22 as output.

    LPC_PINCON->PINSEL4 &= ~LEDS_PCB;    // P2.0-3 as GPIO.
    LPC_GPIO2->FIODIR |= LEDS_BIT;       // P2.0-3 as output.

    LPC_GPIO0->FIOCLR |= RED_BIT;     // Turn off red LED.
    LPC_GPIO2->FIOCLR |= LEDS_BIT;    // Turn off LEDs.
    LPC_GPIO2->FIOSET |= leds[0];     // Turn on first LED.
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
    static uint8_t blinkCount = ST_MULT_BLINK;
    static uint8_t seqCount   = ST_MULT_SEQ;

    if (!blinkCount) {    // 500 ms elapsed.
        const uint32_t current = LPC_GPIO0->FIOPIN;

        LPC_GPIO0->FIOSET = ~current & RED_BIT;    // Toggle LED state.
        LPC_GPIO0->FIOCLR = current & RED_BIT;

        blinkCount = ST_MULT_BLINK;    // Reset blink counter.
    }
    if (!seqCount) {                                // 200 ms elapsed.
        LPC_GPIO2->FIOCLR = leds[i % LEDS_SIZE];    // Turn off current LED.
        i++;                                        // Increment LED index.
        LPC_GPIO2->FIOSET = leds[i % LEDS_SIZE];    // Turn on next LED.

        seqCount = ST_MULT_SEQ;    // Reset sequence counter.
    }

    blinkCount--;    // Decrement counters.
    seqCount--;
}
