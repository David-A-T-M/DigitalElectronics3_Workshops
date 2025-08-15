/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Program to alternate between two color sequences on the onboard RGB LED.
 *
 * This program configures the GPIO pins for the RGB LED on the LPC1769 board.
 * It alternates between two color sequences.
 */

#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/**
 * @def RED_LED
 * @brief Bit mask for the red LED (P0.22).
 */
#define RED_LED      (1<<22)

/**
 * @def GREEN_LED
 * @brief Bit mask for the green LED (P3.25).
 */
#define GREEN_LED    (1<<25)

/**
 * @def BLUE_LED
 * @brief Bit mask for the blue LED (P3.26).
 */
#define BLUE_LED     (1<<26)

/**
 * @def DELAY
 * @brief Delay constant for LED timing.
 */
#define DELAY       2500

/**
 * @def CYCLE_REPEATS
 * @brief Number of times to repeat each sequence before switching.
 */
#define CYCLE_REPEATS 10

/**
 * @def NUM_SEQUENCES
 * @brief Number of color sequences defined.
 */
#define NUM_SEQUENCES 2

/**
 * @def SEQUENCE_LENGTH
 * @brief Number of colors in each sequence.
 */
#define SEQUENCE_LENGTH 3

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

const Color RED     = {1, 0, 0};
const Color GREEN   = {0, 1, 0};
const Color BLUE    = {0, 0, 1};
const Color CYAN    = {0, 1, 1};
const Color MAGENTA = {1, 0, 1};
const Color YELLOW  = {1, 1, 0};
const Color WHITE   = {1, 1, 1};

/**
 * @brief Array of colors for the first sequence.
 * Each color is represented by a bit mask corresponding to the LED channels.
 * The colors are: Red, Green, Blue.
 */
const Color sequence1[SEQUENCE_LENGTH] = {RED, GREEN, BLUE};

/**
 * @brief Array of colors for the second sequence.
 * Each color is represented by a bit mask corresponding to the LED channels.
 * The colors are: Yellow, Cyan, Magenta.
 */
const Color sequence2[SEQUENCE_LENGTH] = {YELLOW, CYAN, MAGENTA};

/**
 * @brief Configures the RGB LED.
 * Sets the pin function to GPIO and configures the direction as output
 * for each color in the RGB LED.
 */
void configGPIO(void);

/**
 * @brief Sets the RGB LED to the specified color.
 * @param color Pointer to a Color struct defining the color to set.
 */
void setLEDColor(const Color *color);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    while (1) {
        // Repeats CYCLE_REPEATS times the first sequence.
        for (uint8_t i = 0; i < CYCLE_REPEATS; i++) {
            // Cycles through each color in the first sequence.
            for (uint8_t j = 0; j < SEQUENCE_LENGTH; j++) {
                setLEDColor(&sequence1[j]);
                delay();
            }
        }
        // Repeats CYCLE_REPEATS times the second sequence.
        for (uint8_t i = 0; i < CYCLE_REPEATS; i++) {
            // Cycles through each color in the second sequence.
            for (uint8_t j = 0; j < SEQUENCE_LENGTH; j++) {
                setLEDColor(&sequence2[j]);
                delay();
            }
        }
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                       // Initialization of the PINSEL configuration structure.
    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_22;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_TRISTATE;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                          // Configure P0.22 as GPIO.

    pinCfg.portNum = PINSEL_PORT_3;
    pinCfg.pinNum = PINSEL_PIN_25;
    PINSEL_ConfigPin(&pinCfg);                          // Configure P3.25 as GPIO.

    pinCfg.pinNum = PINSEL_PIN_26;
    PINSEL_ConfigPin(&pinCfg);                          // Configure P3.26 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, RED_LED, GPIO_OUTPUT);                 // Set P0.22 as output.
    GPIO_SetDir(GPIO_PORT_3, GREEN_LED | BLUE_LED, GPIO_OUTPUT);    // Set P3.25 and P3.26 as output.

    GPIO_SetPins(GPIO_PORT_0, RED_LED);                 // Force red LED off initially.
    GPIO_SetPins(GPIO_PORT_3, GREEN_LED | BLUE_LED);    // Force green and blue LEDs off initially.
}

void setLEDColor(const Color *color) {
    if (color->r)
        GPIO_ClearPins(GPIO_PORT_0, RED_LED);           // Clear P0.22 to turn on red LED
    else
        GPIO_SetPins(GPIO_PORT_0, RED_LED);             // Set P0.22 to turn off red LED

    if (color->g)
        GPIO_ClearPins(GPIO_PORT_3, GREEN_LED);         // Clear P3.25 to turn on green LED
    else
        GPIO_SetPins(GPIO_PORT_3, GREEN_LED);           // Set P3.25 to turn off green LED

    if (color->b)
        GPIO_ClearPins(GPIO_PORT_3, BLUE_LED);          // Clear P3.26 to turn on blue LED
    else
        GPIO_SetPins(GPIO_PORT_3, BLUE_LED);            // Set P3.26 to turn off blue LED
}

void delay() {
    for (uint32_t i = 0; i < DELAY; i++)
        for (uint32_t j = 0; j <DELAY ; j++);
}
