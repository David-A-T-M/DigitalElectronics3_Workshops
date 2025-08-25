/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Multitask: LED blink and sequence using SysTick for LPC1769.
 *
 * This file configures the SysTick timer and GPIO to perform two tasks:
 * - Blink the red LED (P0.22) every 500 ms.
 * - Sequence through four LEDs (P2.0-P2.3), advancing every 200 ms.
 * Both tasks are managed using counters in the SysTick interrupt handler.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Red LED is connected to P0.22. */
#define RED_LED             (22)
/** Four LEDs are connected to P2.0-P2.3. */
#define LEDS                (0)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT             BIT_MASK(RED_LED)
/** Bit mask for the four LEDs (P2.0-P2.3). */
#define LEDS_BIT            BITS_MASK(4, LEDS)

/** Sequence time in milliseconds. */
#define SEQ_TIME            (500)
/** Blink time in milliseconds. */
#define BLINK_TIME          (200)
/** SysTick timer interval in milliseconds. */
#define ST_TIME             (100)

/** Number of SysTick interrupts to achieve the desired blink time. */
#define ST_MULT_BLINK       ((BLINK_TIME/ST_TIME) - 1)
/** Number of SysTick interrupts to achieve the desired sequence time. */
#define ST_MULT_SEQ         ((SEQ_TIME/ST_TIME) - 1)

/** Number of LEDs in the sequence. */
#define LEDS_SIZE           (sizeof(leds) / sizeof(leds[0]))

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
 * @param time Time interval in milliseconds between SysTick interrupts.
 *
 * Initializes the SysTick timer using CMSIS drivers, sets the interrupt interval,
 * and enables the SysTick interrupt and counter.
 */
void configSysTick(uint32_t time);

/** Array of LED bit masks for the sequence (P2.0-P2.3). */
const uint8_t leds[] = {0x1, 0x2, 0x4, 0x8};

/** Index for the current LED in the sequence. */
volatile uint32_t i = 0;

int main(void) {
    configGPIO();
    configSysTick(ST_TIME);

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                       // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_22;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                          // P0.22 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, RED_LED, GPIO_OUTPUT);     // P0.22 as output.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, LEDS_BIT);       // P2.0-3 as GPIO.
    GPIO_SetDir(GPIO_PORT_2, LEDS_BIT, GPIO_OUTPUT);    // P2.0-3 as output.

    GPIO_ClearPins(GPIO_PORT_0, RED_BIT);               // Turn off red LED.
    GPIO_ClearPins(GPIO_PORT_2, LEDS_BIT);              // Turn off LEDs.
    GPIO_SetPins(GPIO_PORT_2, leds[0]);                 // Turn on first LED
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);                         // Initialize SysTick with 100 ms interval.
    SYSTICK_IntCmd(ENABLE);                             // Enable SysTick interrupt.
    SYSTICK_Cmd(ENABLE);                                // Enable SysTick timer.

    NVIC_EnableIRQ(SysTick_IRQn);                       // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    static uint8_t blinkCount = ST_MULT_BLINK;
    static uint8_t seqCount = ST_MULT_SEQ;

    if (!blinkCount) {                                  // 500 ms elapsed.
        const uint32_t current = GPIO_ReadValue(GPIO_PORT_0);

        GPIO_SetPins(GPIO_PORT_0, ~current & RED_BIT);
        GPIO_ClearPins(GPIO_PORT_0, current & RED_BIT);

        blinkCount = ST_MULT_BLINK;                     // Reset blink counter.
    }
    if (!seqCount) {                                    // 200 ms elapsed.
        GPIO_ClearPins(GPIO_PORT_2, leds[i % LEDS_SIZE]); // Turn off current LED.
        i++;                                            // Increment LED index.
        GPIO_SetPins(GPIO_PORT_2, leds[i % LEDS_SIZE]); // Turn on next LED.

        seqCount = ST_MULT_SEQ;                         // Reset sequence counter.
    }

    blinkCount--;                                       // Decrement counters.
    seqCount--;
}

