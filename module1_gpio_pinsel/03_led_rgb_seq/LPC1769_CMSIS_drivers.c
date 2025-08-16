/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Program to alternate between two color sequences on the onboard RGB LED.
 *
 * This program configures the GPIO pins for the RGB LED on the LPC1769 board.
 * It alternates between two color sequences.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))

/** Bit mask for the red LED (P0.22). */
#define RED_LED             BIT_MASK(22)
/** Bit mask for the green LED (P3.25). */
#define GREEN_LED           BIT_MASK(25)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_LED            BIT_MASK(26)

/** Delay constant for LED timing. */
#define DELAY               2500

/** Number of times to repeat each sequence before switching. */
#define CYCLE_REPEATS       10
/** Number of color sequences defined. */
#define NUM_SEQUENCES       2
/** Number of colors in each sequence. */
#define SEQUENCE_LENGTH     3

/**
 * @brief Color structure to represent RGB colors.
 *
 * Each color is represented by three channels: red, green, and blue.
 * Each channel is a uint8_t value where 0 means off and 1 means on.
 */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

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

    while (1) {
        for (uint8_t i = 0; i < CYCLE_REPEATS; i++) {
            for (uint8_t j = 0; j < SEQUENCE_LENGTH; j++) {
                setLEDColor(&sequence1[j]);
                delay();
            }
        }
        for (uint8_t i = 0; i < CYCLE_REPEATS; i++) {
            for (uint8_t j = 0; j < SEQUENCE_LENGTH; j++) {
                setLEDColor(&sequence2[j]);
                delay();
            }
        }
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                       // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_22;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_TRISTATE;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigPin(&pinCfg);                          // P0.22 as GPIO.

    pinCfg.portNum = PINSEL_PORT_3;
    pinCfg.pinNum = PINSEL_PIN_25;
    PINSEL_ConfigPin(&pinCfg);                          // P3.25 as GPIO.

    pinCfg.pinNum = PINSEL_PIN_26;
    PINSEL_ConfigPin(&pinCfg);                          // P3.26 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, RED_LED, GPIO_OUTPUT);                 // P0.22 as output.
    GPIO_SetDir(GPIO_PORT_3, GREEN_LED | BLUE_LED, GPIO_OUTPUT);    // P3.25 and P3.26 as output.

    GPIO_SetPins(GPIO_PORT_0, RED_LED);                 // Red LED off.
    GPIO_SetPins(GPIO_PORT_3, GREEN_LED | BLUE_LED);    // Green and blue LEDs off.
}

void setLEDColor(const Color *color) {
    if (color->r)
        GPIO_ClearPins(GPIO_PORT_0, RED_LED);           // Turn on red LED.
    else
        GPIO_SetPins(GPIO_PORT_0, RED_LED);             // Turn off red LED.

    if (color->g)
        GPIO_ClearPins(GPIO_PORT_3, GREEN_LED);         // Turn on green LED.
    else
        GPIO_SetPins(GPIO_PORT_3, GREEN_LED);           // Turn off green LED.

    if (color->b)
        GPIO_ClearPins(GPIO_PORT_3, BLUE_LED);          // Turn on blue LED.
    else
        GPIO_SetPins(GPIO_PORT_3, BLUE_LED);            // Turn off blue LED.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j <DELAY ; j++);
}
