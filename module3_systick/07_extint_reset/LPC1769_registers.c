/**
 * @file LPC1769_registers.c
 * @brief External interrupt counter with auto-reset using SysTick for LPC1769.
 *
 * This file configures GPIO, external interrupt (EINT1), and the SysTick timer to:
 * - Increment a counter (displayed on LEDs P0.0-P0.3) each time a button on P2.11 is pressed (EINT1).
 * - Automatically reset the counter to zero every 2 seconds using the SysTick timer.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Four LEDs are connected to P0.0-P0.3. */
#define LEDS                (0)
/** Button is connected to P2.11 (EINT1). */
#define BTN                 (11)

/** Bit mask for the four LEDs (P0.0-P0.3). */
#define LEDS_BITS           BITS_MASK(4, LEDS)
/** Bit mask for the button (P2.11). */
#define BTN_BIT             BIT_MASK(BTN)
/** External interrupt 1 bit mask. */
#define EINT1_BIT           BIT_MASK(1)

/** PCB mask for the four LEDs (P0.0-P0.3). */
#define LEDS_PCB            BITS_MASK(8, LEDS * 2)
/** PCB mask for the button (P2.11). */
#define BTN_PCB             BITS_MASK(2, BTN * 2)
/** PCB lower bit mask for the button (P2.11). */
#define BTN_PCB_L           BIT_MASK(BTN * 2)

/** Reset time in milliseconds. */
#define RESET_TIME          (2000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME             (100)

/** SysTick load value for the desired time interval. */
#define ST_LOAD             ((ST_TIME * 100000) - 1)
/** Number of SysTick interrupts to achieve the desired reset time. */
#define ST_MULT_RESET       ((RESET_TIME/ST_TIME) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE           BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT          BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE        BIT_MASK(2)

/**
 * @brief Configures GPIO pins for the LED display and button input.
 *
 * Sets P0.0-P0.3 as outputs for the LEDs and P2.11 as input for the button (EINT1).
 * Initializes all LEDs to the off state.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt (EINT1) for the button.
 *
 * Sets up EINT1 on P2.11 to trigger on a rising edge and enables its interrupt in the NVIC.
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

/** Counter to track button presses, displayed on LEDs. */
volatile uint8_t count = 0;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_LOAD);

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~LEDS_PCB;               // P0.0 to P0.3 as GPIO.
    LPC_GPIO0->FIODIR |= LEDS_BITS;                 // P0.0 to P0.3 as outputs.

    LPC_PINCON->PINSEL4 &= ~BTN_PCB;
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;               // P2.11 as GPIO.
    LPC_PINCON->PINMODE4 &= ~BTN_PCB;               // P2.11 pull-up.
    LPC_GPIO2->FIODIR &= ~BTN_BIT;                  // P2.11 as input.

    LPC_GPIO0->FIOCLR = LEDS_BITS;                  // Turn off all LEDs.
}

void configInt(void) {
    LPC_SC->EXTMODE &= ~EINT1_BIT;                  // EINT1 edge sensitive.
    LPC_SC->EXTPOLAR |= EINT1_BIT;                  // EINT1 rising edge.

    LPC_SC->EXTINT = EINT1_BIT;                    // Clear flag.
    NVIC_ClearPendingIRQ(EINT1_IRQn);               // Clear pending interrupt.
    NVIC_EnableIRQ(EINT1_IRQn);                     // Enable EINT1 interrupt in NVIC.
}

void configSysTick(uint32_t ticks) {
    SysTick->LOAD = ticks;                          // Load value for 100 ms interval.
    SysTick->VAL = 0;                               // Clear current value and interrupt flag.
    SysTick->CTRL = ST_ENABLE |                     // Enable SysTick interrupt.
                    ST_TICKINT |                    // Enable SysTick exception request.
                    ST_CLKSOURCE;                   // Use processor clock.

    NVIC_EnableIRQ(SysTick_IRQn);                   // Enable SysTick interrupt in NVIC.
}

void EINT1_IRQHandler(void) {
    LPC_GPIO0->FIOSET = ++count;                    // Increment count and display it on LEDs.

    LPC_SC->EXTINT |= EINT1_BIT;                    // Clear flag.
}

void SysTick_Handler(void) {
    static uint8_t reset = ST_MULT_RESET;

    if (!reset) {                                   // 2 s elapsed.
        count = 0;                                  // Reset count.

        reset = ST_MULT_RESET;                      // Reset sequence counter.
    }

    reset--;
}
