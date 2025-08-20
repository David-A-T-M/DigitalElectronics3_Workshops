/**
 * @file LPC1769_registers.c
 * @brief GPIO and interrupt configuration for LPC1769 RGB LED and button control.
 *
 * This file contains functions to configure GPIO pins, external interrupts,
 * and to control an RGB LED using the LPC1769 microcontroller.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

#define BTN_A               (0)
#define BTN_B               (11)
#define RED_LED             (22)
#define GREEN_LED           (25)
#define BLUE_LED            (26)

#define BTN_A_BIT           BIT_MASK(BTN_A)
#define BTN_B_BIT           BIT_MASK(BTN_B)
#define RED_BIT             BIT_MASK(RED_LED)
#define GREEN_BIT           BIT_MASK(GREEN_LED)
#define BLUE_BIT            BIT_MASK(BLUE_LED)
#define EINT1_BIT           BIT_MASK(1)

#define SEQUENCE_LENGTH     (3)
#define DELAY               (2500)

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
 * @brief Configures the GPIO pins for LEDs and buttons.
 */
void configGPIO(void);

/**
 * @brief Configures gpio and external interrupts for buttons.
 */
void configInt(void);

/**
 * @brief Sets the RGB LED to the specified color.
 * @param color Pointer to a Color struct defining the color to set.
 */
void setLEDColor(const Color *color);

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

/** First color sequence. */
const Color sequence1[SEQUENCE_LENGTH] = {RED, GREEN, BLUE};
/** Second color sequence. */
const Color sequence2[SEQUENCE_LENGTH] = {YELLOW, CYAN, MAGENTA};

int main(void) {
    configGPIO();
    configInt();

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                       // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                          // Configure P0.0 as GPIO with pull-up.
    GPIO_SetDir(GPIO_PORT_0, BTN_A_BIT, GPIO_INPUT);    // P0.0 as input.

    pinCfg.pinNum = PINSEL_PIN_22;
    PINSEL_ConfigPin(&pinCfg);                          // Configure P0.22 as GPIO.
    GPIO_SetDir(GPIO_PORT_0, RED_BIT, GPIO_OUTPUT);     // P0.22 as output.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, GREEN_BIT | BLUE_BIT);       // P2.25 and P2.26 as GPIO.
    GPIO_SetDir(GPIO_PORT_3, GREEN_BIT | BLUE_BIT, GPIO_OUTPUT);    // P2.25 and P2.26 as output.

    pinCfg.funcNum = PINSEL_FUNC_1;
    PINSEL_ConfigPin(&pinCfg);                          // Configure P2.11 as EINT3 with pull-up.
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};                // EXTI configuration structure.
    extiCfg.line = EXTI_EINT1;
    extiCfg.mode = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_FALLING_EDGE;

    EXTI_ConfigEnable(&extiCfg);                // Configure EINT1 for falling edge.

    GPIO_IntCmd(GPIO_PORT_0, BTN_A_BIT, GPIO_INT_RISING); // Enable rising edge interrupt on P0.0.
    GPIO_ClearInt(GPIO_PORT_0, BTN_A_BIT);                // Clears the interrupt for P0.0.

    NVIC_SetPriority(EINT1_IRQn, 0);
    NVIC_SetPriority(EINT3_IRQn, 1);

    NVIC_EnableIRQ(EINT3_IRQn);                         // Enable EINT3 interrupt in NVIC.
}

void setLEDColor(const Color *color) {
    if (color->r)
        GPIO_ClearPins(GPIO_PORT_0, RED_BIT);           // Turn on red LED.
    else
        GPIO_SetPins(GPIO_PORT_0, RED_BIT);             // Turn off red LED.

    if (color->g)
        GPIO_ClearPins(GPIO_PORT_3, GREEN_BIT);         // Turn on green LED.
    else
        GPIO_SetPins(GPIO_PORT_3, GREEN_BIT);           // Turn off green LED.

    if (color->b)
        GPIO_ClearPins(GPIO_PORT_3, BLUE_BIT);          // Turn on blue LED.
    else
        GPIO_SetPins(GPIO_PORT_3, BLUE_BIT);            // Turn off blue LED.
}

void EINT1_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence1[i]);
        delay();
    }
    EXTI_ClearFlag(EXTI_EINT1);                         // Clear EINT1 interrupt flag.
}

void EINT3_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence2[i]);
        delay();
    }
    GPIO_ClearInt(GPIO_PORT_0, BTN_A_BIT);              // Clears the interrupt for P0.0.
}

void delay(void) {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
