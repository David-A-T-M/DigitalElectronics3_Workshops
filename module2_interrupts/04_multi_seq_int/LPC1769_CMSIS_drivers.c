/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Controls an RGB LED with two buttons and interrupt sequences on the LPC1769.
 *
 * This example configures GPIO pins and external interrupts for two buttons (P0.0, P2.11)
 * and an RGB LED (P0.22, P3.25, P3.26). Pressing each button triggers an interrupt that
 * cycles the RGB LED through a predefined color sequence.
 */

#include "lpc17xx_exti.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Button connected to P0.0. */
#define BTN_A     (0)
/** Button connected to P2.11. */
#define BTN_B     (11)
/** Red LED connected to P0.22. */
#define RED_LED   (22)
/** Green LED connected to P3.25. */
#define GREEN_LED (25)
/** Blue LED connected to P3.26. */
#define BLUE_LED  (26)

/** Bit mask for button A (P0.0). */
#define BTN_A_BIT BIT_MASK(BTN_A)
/** Bit mask for button B (P2.11). */
#define BTN_B_BIT BIT_MASK(BTN_B)
/** Bit mask for the red LED (P0.22). */
#define RED_BIT   BIT_MASK(RED_LED)
/** Bit mask for the green LED (P3.25). */
#define GREEN_BIT BIT_MASK(GREEN_LED)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_BIT  BIT_MASK(BLUE_LED)
/** Bit mask for external interrupt 1 (EINT1). */
#define EINT1_BIT BIT_MASK(1)

/** Number of colors in each sequence. */
#define SEQUENCE_LENGTH (3)
/** Delay constant for LED timing. */
#define DELAY           (2500)

/**
 * @brief Color structure to represent RGB colors.
 *
 * Each color is represented by three channels: red, green, and blue.
 * Each channel is a uint8_t value where 0 means off and 1 means on
 */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

/**
 * @brief Configures GPIO pins for RGB LED outputs and button inputs.
 *
 * Sets P0.22 as output for the red LED, P3.25 and P3.26 as outputs for green and blue LEDs.
 * Configures P0.0 as a GPIO input with pull-up for button A.
 * Configures P2.11 as an external interrupt input (EINT1) with pull-up for button B.
 */
void configGPIO(void);

/**
 * @brief Configures external and GPIO interrupts for the buttons.
 *
 * Enables rising edge interrupt for button A (P0.0) using EINT3.
 * Configures EINT1 (P2.11) as edge-sensitive and falling edge active.
 * Sets interrupt priorities, clears pending flags, and enables interrupts in the NVIC.
 */
void configInt(void);

/**
 * @brief Sets the RGB LED to the specified color.
 * @param color Pointer to a Color struct defining the color to display.
 *
 * Turns on or off each LED channel (red, green, blue) according to the color struct.
 */
void setLEDColor(const Color* color);

/**
 * @brief Generates a blocking delay using nested loops.
 */
void delay(void);

const Color RED     = {1, 0, 0};
const Color GREEN   = {0, 1, 0};
const Color BLUE    = {0, 0, 1};
const Color CYAN    = {0, 1, 1};
const Color MAGENTA = {1, 0, 1};
const Color YELLOW  = {1, 1, 0};
const Color WHITE   = {1, 1, 1};

/** Color sequence for button A (P0.0). */
const Color sequence1[SEQUENCE_LENGTH] = {YELLOW, CYAN, MAGENTA};
/** Color sequence for button B (P2.11). */
const Color sequence2[SEQUENCE_LENGTH] = {RED, GREEN, BLUE};

int main(void) {
    configGPIO();
    configInt();

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

    PINSEL_ConfigPin(&pinCfg);                          // Configure P0.0 as GPIO with pull-up.
    GPIO_SetDir(GPIO_PORT_0, BTN_A_BIT, GPIO_INPUT);    // P0.0 as input.

    pinCfg.pinNum = PINSEL_PIN_22;
    PINSEL_ConfigPin(&pinCfg);                         // Configure P0.22 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, RED_BIT, GPIO_OUTPUT);    // P0.22 as output.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, GREEN_BIT | BLUE_BIT);       // P2.25 and P2.26 as GPIO.
    GPIO_SetDir(GPIO_PORT_3, GREEN_BIT | BLUE_BIT, GPIO_OUTPUT);    // P2.25 and P2.26 as output.

    pinCfg.funcNum = PINSEL_FUNC_1;
    PINSEL_ConfigPin(&pinCfg);    // Configure P2.11 as EINT3 with pull-up.
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};    // EXTI configuration structure.
    extiCfg.line          = EXTI_EINT1;
    extiCfg.mode          = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity      = EXTI_FALLING_EDGE;

    EXTI_ConfigEnable(&extiCfg);    // Configure EINT1 for falling edge.

    GPIO_IntCmd(GPIO_PORT_0, BTN_A_BIT, GPIO_INT_RISING);    // Enable rising edge interrupt on P0.0.
    GPIO_ClearInt(GPIO_PORT_0, BTN_A_BIT);                   // Clears the interrupt for P0.0.

    NVIC_SetPriority(EINT1_IRQn, 0);
    NVIC_SetPriority(EINT3_IRQn, 1);

    NVIC_EnableIRQ(EINT3_IRQn);    // Enable EINT3 interrupt in NVIC.
}

void setLEDColor(const Color* color) {
    if (color->r)
        GPIO_ClearPins(GPIO_PORT_0, RED_BIT);    // Turn on red LED.
    else
        GPIO_SetPins(GPIO_PORT_0, RED_BIT);    // Turn off red LED.

    if (color->g)
        GPIO_ClearPins(GPIO_PORT_3, GREEN_BIT);    // Turn on green LED.
    else
        GPIO_SetPins(GPIO_PORT_3, GREEN_BIT);    // Turn off green LED.

    if (color->b)
        GPIO_ClearPins(GPIO_PORT_3, BLUE_BIT);    // Turn on blue LED.
    else
        GPIO_SetPins(GPIO_PORT_3, BLUE_BIT);    // Turn off blue LED.
}

void EINT1_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence2[i]);
        delay();
    }
    EXTI_ClearFlag(EXTI_EINT1);    // Clear EINT1 interrupt flag.
}

void EINT3_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence1[i]);
        delay();
    }
    GPIO_ClearInt(GPIO_PORT_0, BTN_A_BIT);    // Clears the interrupt for P0.0.
}

void delay(void) {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++)
            __NOP();
}
