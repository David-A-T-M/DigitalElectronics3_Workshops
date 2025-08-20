/**
 * @file LPC1769_registers.c
 * @brief Implements GPIO and interrupt configuration for controlling
 *        an RGB LED and handling button input.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

#define RED_LED             (22)
#define GREEN_LED           (25)
#define BLUE_LED            (26)
#define BTN                 (0)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT             BIT_MASK(RED_LED)
/** Bit mask for the green LED (P3.25). */
#define GREEN_BIT           BIT_MASK(GREEN_LED)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_BIT            BIT_MASK(BLUE_LED)
/** Bit mask for the button (P0.0). */
#define BTN_BIT             BIT_MASK(BTN)
/** PCB mask for the red LED (P0.22). */
#define RED_PCB             BITS_MASK(2, (RED_LED - 16) * 2)
/** PCB mask for the green LED (P3.25). */
#define GREEN_PCB           BITS_MASK(2, (GREEN_LED - 16) * 2)
/** PCB mask for the blue LED (P3.26). */
#define BLUE_PCB            BITS_MASK(2, (BLUE_LED - 16) * 2)
/** PCB mask for the button (P0.0). */
#define BTN_PCB             BITS_MASK(2, BTN)

/** Delay constant for LED timing. */
#define DELAY               2500

/** Number of colors in each sequence. */
#define SEQUENCE_LENGTH     8

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
 * @brief Configures GPIO pins for the RGB LED and button.
 *
 * Sets the pin functions, modes, and directions for the red, green, and blue LEDs as outputs,
 * and the button as an input with a pull-up resistor.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt for the button.
 *
 * Enables a falling edge interrupt on the button pin (P2.0) and clears any pending interrupts.
 */
void configInt(void);

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
const Color BLACK   = {0, 0, 0};

/** First color sequence. */
const Color sequence[SEQUENCE_LENGTH] = {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, BLACK};

/** Index for the current color in the sequence. */
volatile uint32_t i = 0;

/** Flag to control the LED sequence. */
volatile uint8_t f = 1;

int main(void) {
    configGPIO();

    while (1) {
        if (f) {
            setLEDColor(&sequence[i % SEQUENCE_LENGTH]);
            delay();
            i++;
        }
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);              // P0.22 as GPIO.
    LPC_PINCON->PINSEL7 &= ~(GREEN_PCB | BLUE_PCB); // P3.25 and P3.26 as GPIO.
    LPC_PINCON->PINSEL4 &= ~(BTN_PCB);              // P2.0 as GPIO.

    LPC_PINCON->PINMODE4 &= ~(BTN_PCB);             // P2.0 pull-up.

    LPC_GPIO0->FIODIR |= RED_BIT;                   // P0.22 as output.
    LPC_GPIO3->FIODIR |= GREEN_BIT | BLUE_BIT;      // P3.25 and P3.26 as output.
    LPC_GPIO2->FIODIR &= ~BTN_BIT;                  // P2.0 as input.

    LPC_GPIO0->FIOSET = RED_BIT;                    // Red LED off.
    LPC_GPIO3->FIOSET = GREEN_BIT | BLUE_BIT;       // Green and blue LEDs off.
}

void setLEDColor(const Color *color) {
    if (color->r)
        LPC_GPIO0->FIOCLR = RED_BIT;                // Turn on red LED.
    else
        LPC_GPIO0->FIOSET = RED_BIT;                // Turn off red LED.

    if (color->g)
        LPC_GPIO3->FIOCLR = GREEN_BIT;              // Turn on green LED.
    else
        LPC_GPIO3->FIOSET = GREEN_BIT;              // Turn off green LED.

    if (color->b)
        LPC_GPIO3->FIOCLR = BLUE_BIT;               // Turn on blue LED.
    else
        LPC_GPIO3->FIOSET = BLUE_BIT;               // Turn off blue LED.
}

void configInt(void) {
    LPC_GPIOINT->IO2IntEnF |= BTN_BIT;              // Enable falling edge interrupt on P2.0.

    LPC_GPIOINT->IO2IntClr = BTN_BIT;               // Clear any pending interrupts on P2.0.
    NVIC_EnableIRQ(EINT3_IRQn);                     // Enable EINT3 interrupt in NVIC.
}

void delay() {
    for (volatile uint32_t j = 0; j < DELAY; j++)
        for (volatile uint32_t k = 0; k < DELAY ; k++);
}

void EINT3_IRQHandler(void) {
    f = !f;                                         // Toggle the flag to pause/resume the LED sequence.

    LPC_GPIOINT->IO2IntClr = BTN_BIT;               // Clear interrupt flag on P2.0.
}
