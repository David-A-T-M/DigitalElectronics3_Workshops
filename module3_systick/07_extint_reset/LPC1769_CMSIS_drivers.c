/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief External interrupt counter with auto-reset using SysTick for LPC1769.
 *
 * This file configures GPIO, external interrupt (EINT1), and the SysTick timer to:
 * - Increment a counter (displayed on LEDs P0.0-P0.3) each time a button on P2.11 is pressed (EINT1).
 * - Automatically reset the counter to zero every 2 seconds using the SysTick timer.
 */

#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_systick.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Four LEDs are connected to P0.0-P0.3. */
#define LEDS (0)
/** Button is connected to P2.11 (EINT1). */
#define BTN  (11)

/** Bit mask for the four LEDs (P0.0-P0.3). */
#define LEDS_BITS BITS_MASK(4, LEDS)
/** Bit mask for the button (P2.11). */
#define BTN_BIT   BIT_MASK(BTN)
/** External interrupt 1 bit mask. */
#define EINT1_BIT BIT_MASK(1)

/** Reset time in milliseconds. */
#define RESET_TIME (2000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME    (100)

/** Number of SysTick interrupts to achieve the desired reset time. */
#define ST_MULT_RESET ((RESET_TIME / ST_TIME) - 1)

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
 * @param time Time interval in milliseconds between SysTick interrupts.
 *
 * Initializes the SysTick timer using CMSIS drivers, sets the interrupt interval,
 * and enables the SysTick interrupt and counter.
 */
void configSysTick(uint32_t time);

/** Counter to track button presses, displayed on LEDs. */
volatile uint8_t count = 0;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_TIME);

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

    PINSEL_ConfigMultiplePins(&pinCfg, LEDS_BITS);       // Configure P0.0 to P0.3 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, LEDS_BITS, GPIO_OUTPUT);    // P0.0 to P0.3 as outputs.

    pinCfg.portNum = PINSEL_PORT_2;
    pinCfg.pinNum  = PINSEL_PIN_11;
    pinCfg.funcNum = PINSEL_FUNC_1;

    PINSEL_ConfigPin(&pinCfg);                        // P2.11 as EINT1 with pull-up.
    GPIO_SetDir(GPIO_PORT_2, BTN_BIT, GPIO_INPUT);    // P2.11 as input.

    GPIO_ClearPins(GPIO_PORT_0, LEDS_BITS);    // Turn off all LEDs.
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};

    extiCfg.line     = EXTI_EINT1;
    extiCfg.mode     = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_RISING_EDGE;

    EXTI_ConfigEnable(&extiCfg);    // Configure and enable EINT1.
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);    // Initialize SysTick.
    SYSTICK_IntCmd(ENABLE);        // Enable SysTick interrupt.
    SYSTICK_Cmd(ENABLE);           // Enable SysTick timer.
}

void EINT1_IRQHandler(void) {
    GPIO_SetPins(GPIO_PORT_0, ++count);    // Increment count and display it on LEDs.

    EXTI_ClearFlag(EXTI_EINT1);    // Clear EINT1 flag.
}

void SysTick_Handler(void) {
    static uint8_t reset = ST_MULT_RESET;

    if (!reset) {     // 2 s elapsed.
        count = 0;    // Reset count.

        reset = ST_MULT_RESET;    // Reset sequence counter.
    }

    reset--;
}
