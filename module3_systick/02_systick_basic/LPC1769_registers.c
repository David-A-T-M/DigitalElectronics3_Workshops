/**
 * @file LPC1769_registers.c
 * @brief Basic SysTick and GPIO configuration for LPC1769.
 *
 * This file configures the SysTick timer to generate periodic interrupts
 * and sets up the GPIO pin to control the red LED on the LPC1769 board.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED is connected to P0.22. */
#define RED_LED (22)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT BIT_MASK(RED_LED)
/** PCB mask for the red LED (P0.22). */
#define RED_PCB BITS_MASK(2, (RED_LED - 16) * 2)

/** SysTick desired time interval in milliseconds. */
#define ST_TIME (10)

/** SysTick load value for the desired time interval. */
#define ST_LOAD      ((ST_TIME * 100000) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE    BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT   BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE BIT_MASK(2)

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
 * @param ticks Number of clock cycles between SysTick interrupts.
 *
 * Loads the specified value, clears the current counter, and enables
 * the SysTick timer and its interrupt.
 */
void configSysTick(uint32_t ticks);

int main(void) {
    configGPIO();
    configSysTick(ST_LOAD);

    // Alternatively: this function loads (parameter -1) in the LOAD register. Sets priority to 31.
    // SysTick_Config(1000000);                    // Configure SysTick for 10 ms interval.

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);    // P0.22 as GPIO.

    LPC_GPIO0->FIODIR |= RED_BIT;    // Set P0.22 as output.

    LPC_GPIO0->FIOSET = RED_BIT;    // Turn LED off.
}

void configSysTick(uint32_t ticks) {
    SysTick->LOAD = ticks;           // Load value for 10 ms interval.
    SysTick->VAL  = 0;               // Clear current value and interrupt flag.
    SysTick->CTRL = ST_ENABLE |      // Enable SysTick interrupt.
                    ST_TICKINT |     // Enable SysTick exception request.
                    ST_CLKSOURCE;    // Use processor clock.

    NVIC_EnableIRQ(SysTick_IRQn);    // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    const uint32_t current = LPC_GPIO0->FIOPIN;

    LPC_GPIO0->FIOSET = ~current & RED_BIT;    // Toggle LED state.
    LPC_GPIO0->FIOCLR = current & RED_BIT;
}
