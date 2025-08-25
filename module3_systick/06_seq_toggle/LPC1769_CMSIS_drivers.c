/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief LED sequence with toggle control using SysTick and external interrupt for LPC1769.
 *
 * This file configures the SysTick timer, GPIO, and external interrupt to sequence through
 * eight LEDs (P0.0-P0.7) at 250 ms intervals. The sequence can be started or stopped by
 * pressing the button connected to P2.10 (EINT0).
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_systick.h"
#include "lpc17xx_exti.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** LEDs connected to P0.0-P0.7. */
#define LEDS                (0)
/** Button connected to P2.10 (EINT0). */
#define BTN                 (10)

/** Mask for all LED bits (P0.0 to P0.7). */
#define LEDS_BIT            BITS_MASK(8, LEDS)
/** Mask for the button bit (P2.10). */
#define BTN_BIT             BIT_MASK(BTN)
/** Mask for EINT0 (P2.10). */
#define EINT0_BIT           BIT_MASK(0)

/** Sequence time in milliseconds. */
#define SEQ_TIME            (250)
/** SysTick timer interval in milliseconds. */
#define ST_TIME             (50)

/** Number of SysTick interrupts to achieve the desired sequence time. */
#define ST_MULT_SEQ         ((SEQ_TIME/ST_TIME) - 1)

/** Number of elements in the leds array. */
#define LEDS_SIZE           (sizeof(leds) / sizeof(leds[0]))

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
 * @param time Time interval in milliseconds between SysTick interrupts.
 *
 * Initializes the SysTick timer using CMSIS drivers, sets the interrupt interval,
 * and enables the SysTick interrupt and counter.
 */
void configSysTick(uint32_t time);

/** LEDs array for sequencing. */
const uint8_t leds[] = {0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80};

/** Flag to control the LED sequence (0 = stopped, 1 = running). */
volatile uint8_t flag = 0;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_TIME);

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                           // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, LEDS_BIT);           // P0.0 to P0.7 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, LEDS_BIT, GPIO_OUTPUT);        // P0.0 to P0.7 as outputs.

    pinCfg.portNum = PINSEL_PORT_2;
    pinCfg.pinNum = PINSEL_PIN_10;
    pinCfg.funcNum = PINSEL_FUNC_1;

    PINSEL_ConfigPin(&pinCfg);                              // P2.10 as EINT0 with pull-up.
    GPIO_SetDir(GPIO_PORT_2, BTN_BIT, GPIO_INPUT);          // P2.10 as input.

    GPIO_ClearPins(GPIO_PORT_0, LEDS_BIT);                  // Turn off all LEDs.
    GPIO_SetPins(GPIO_PORT_0, leds[0]);                     // Turn on first LED.
}

void configInt(void) {
    EXTI_CFG_Type extiCfg ={0};
    extiCfg.line = EXTI_EINT0;
    extiCfg.mode = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_RISING_EDGE;

    EXTI_ConfigEnable(&extiCfg);
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);                             // Initialize SysTick.
    SYSTICK_IntCmd(ENABLE);                                 // Enable SysTick interrupt.
    SYSTICK_Cmd(ENABLE);                                    // Enable SysTick timer.

    NVIC_EnableIRQ(SysTick_IRQn);                           // Enable SysTick interrupt in NVIC.
}

void SysTick_Handler(void) {
    static uint32_t i = 0;

    if (!flag) return;                                      // If flag is not set, do nothing.

    static uint8_t seqCount = ST_MULT_SEQ;

    if (!seqCount) {                                        // 200 ms elapsed.
        GPIO_ClearPins(GPIO_PORT_0, leds[i % LEDS_SIZE]);   // Turn off current LED.
        i++;                                                // Increment LED index.
        GPIO_SetPins(GPIO_PORT_0, leds[i % LEDS_SIZE]);     // Turn on next LED.

        seqCount = ST_MULT_SEQ;                             // Reset sequence counter.
    }

    seqCount--;                                             // Decrement counter.
}

void EINT0_IRQHandler(void) {
    flag = !flag;                                           // Toggle flag state.

    EXTI_ClearFlag(EXTI_EINT0);                             // Clear EINT0 flag.
}
