/**
 * @file LPC1769_registers.c
 * @brief Program to alternate between two color sequences on the onboard RGB LED.
 *
 * This program configures the GPIO pins for the RGB LED on the LPC1769 board.
 * It alternates between two color sequences.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Bit mask for the red LED (P0.22). */
#define RED_LED             BIT_MASK(22)
/** Bit mask for the green LED (P3.25). */
#define GREEN_LED           BIT_MASK(25)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_LED            BIT_MASK(26)
/** Double bit mask for the red LED (P0.22). */
#define RED_LED_DB          BITS_MASK(2, 12)
/** Double bit mask for the green and blue LEDs (P3.25 and P3.26). */
#define GREEN_BLUE_LED_DB   BITS_MASK(4, 18)

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
 * Each channel is a uint8_t value where 0 means off and 1 means on
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
    LPC_PINCON->PINSEL1 &= ~(RED_LED_DB);           // P0.22 as GPIO.
    LPC_PINCON->PINSEL7 &= ~(GREEN_BLUE_LED_DB);    // P3.25 and P3.26 as GPIO.

    LPC_GPIO0->FIODIR |= RED_LED;                   // P0.22 as output.
    LPC_GPIO3->FIODIR |= GREEN_LED | BLUE_LED;      // P3.25 and P3.26 as output.

    LPC_GPIO0->FIOSET = RED_LED;                    // Red LED off.
    LPC_GPIO3->FIOSET = GREEN_LED | BLUE_LED;       // Green and blue LEDs off.
}

void setLEDColor(const Color *color) {
    if (color->r)
        LPC_GPIO0->FIOCLR = RED_LED;                // Turn on red LED.
    else
        LPC_GPIO0->FIOSET = RED_LED;                // Turn off red LED.

    if (color->g)
        LPC_GPIO3->FIOCLR = GREEN_LED;              // Turn on green LED.
    else
        LPC_GPIO3->FIOSET = GREEN_LED;              // Turn off green LED.

    if (color->b)
        LPC_GPIO3->FIOCLR = BLUE_LED;               // Turn on blue LED.
    else
        LPC_GPIO3->FIOSET = BLUE_LED;               // Turn off blue LED.
}

void delay() {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j <DELAY ; j++);
}
