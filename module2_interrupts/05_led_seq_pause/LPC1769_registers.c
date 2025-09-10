/**
 * @file LPC1769_registers.c
 * @brief Controls an RGB LED sequence with pause/resume functionality using a button interrupt on the LPC1769.
 *
 * This example configures GPIO pins and an external interrupt for a button (P2.0) and an RGB LED (P0.22, P3.25, P3.26).
 * The main loop cycles the RGB LED through a color sequence. Pressing the button toggles pause/resume of the sequence.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED connected to P0.22. */
#define RED_LED   (22)
/** Green LED connected to P3.25. */
#define GREEN_LED (25)
/** Blue LED connected to P3.26. */
#define BLUE_LED  (26)
/** Button connected to P2.0. */
#define BTN       (0)

/** Bit mask for the red LED (P0.22). */
#define RED_BIT   BIT_MASK(RED_LED)
/** Bit mask for the green LED (P3.25). */
#define GREEN_BIT BIT_MASK(GREEN_LED)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_BIT  BIT_MASK(BLUE_LED)
/** Bit mask for the button (P0.0). */
#define BTN_BIT   BIT_MASK(BTN)

/** PCB mask for the red LED (P0.22). */
#define RED_PCB   BITS_MASK(2, (RED_LED - 16) * 2)
/** PCB mask for the green LED (P3.25). */
#define GREEN_PCB BITS_MASK(2, (GREEN_LED - 16) * 2)
/** PCB mask for the blue LED (P3.26). */
#define BLUE_PCB  BITS_MASK(2, (BLUE_LED - 16) * 2)
/** PCB mask for the button (P0.0). */
#define BTN_PCB   BITS_MASK(2, BTN)

/** Delay constant for LED timing. */
#define DELAY (2500)

/** Number of colors in each sequence. */
#define SEQUENCE_LENGTH (8)

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
 * @brief Configures GPIO pins for RGB LED outputs and button input.
 *
 * Sets P0.22 as output for the red LED, P3.25 and P3.26 as outputs for green and blue LEDs.
 * Configures P2.0 as a GPIO input with pull-up for the button.
 * Initializes all LEDs to the off state.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt for the button on P2.0.
 *
 * Enables a falling edge interrupt for P2.0 and clears any pending interrupt flags.
 * Enables the EINT3 interrupt in the NVIC.
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
void delay();

const Color RED     = {1, 0, 0};
const Color GREEN   = {0, 1, 0};
const Color BLUE    = {0, 0, 1};
const Color CYAN    = {0, 1, 1};
const Color MAGENTA = {1, 0, 1};
const Color YELLOW  = {1, 1, 0};
const Color WHITE   = {1, 1, 1};
const Color BLACK   = {0, 0, 0};

/** Color sequence for the RGB LED. */
const Color sequence[SEQUENCE_LENGTH] = {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, BLACK};

/** Flag to control the LED sequence. */
volatile uint8_t flag = 1;

int main(void) {
    configGPIO();

    uint32_t i = 0;

    while (1) {
        if (flag) {
            setLEDColor(&sequence[i % SEQUENCE_LENGTH]);
            delay();
            i++;
        }
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);                 // P0.22 as GPIO.
    LPC_PINCON->PINSEL7 &= ~(GREEN_PCB | BLUE_PCB);    // P3.25 and P3.26 as GPIO.
    LPC_PINCON->PINSEL4 &= ~(BTN_PCB);                 // P2.0 as GPIO.

    LPC_PINCON->PINMODE4 &= ~(BTN_PCB);    // P2.0 pull-up.

    LPC_GPIO0->FIODIR |= RED_BIT;                 // P0.22 as output.
    LPC_GPIO3->FIODIR |= GREEN_BIT | BLUE_BIT;    // P3.25 and P3.26 as output.
    LPC_GPIO2->FIODIR &= ~BTN_BIT;                // P2.0 as input.

    LPC_GPIO0->FIOSET = RED_BIT;                 // Red LED off.
    LPC_GPIO3->FIOSET = GREEN_BIT | BLUE_BIT;    // Green and blue LEDs off.
}

void setLEDColor(const Color* color) {
    if (color->r)
        LPC_GPIO0->FIOCLR = RED_BIT;    // Turn on red LED.
    else
        LPC_GPIO0->FIOSET = RED_BIT;    // Turn off red LED.

    if (color->g)
        LPC_GPIO3->FIOCLR = GREEN_BIT;    // Turn on green LED.
    else
        LPC_GPIO3->FIOSET = GREEN_BIT;    // Turn off green LED.

    if (color->b)
        LPC_GPIO3->FIOCLR = BLUE_BIT;    // Turn on blue LED.
    else
        LPC_GPIO3->FIOSET = BLUE_BIT;    // Turn off blue LED.
}

void configInt(void) {
    LPC_GPIOINT->IO2IntEnF |= BTN_BIT;    // Enable falling edge interrupt on P2.0.

    LPC_GPIOINT->IO2IntClr = BTN_BIT;    // Clear any pending interrupts on P2.0.
    NVIC_EnableIRQ(EINT3_IRQn);          // Enable EINT3 interrupt in NVIC.
}

void delay() {
    for (volatile uint32_t j = 0; j < DELAY; j++)
        for (volatile uint32_t k = 0; k < DELAY; k++)
            __NOP();
}

void EINT3_IRQHandler(void) {
    flag = !flag;    // Toggle the flag to pause/resume the LED sequence.

    LPC_GPIOINT->IO2IntClr = BTN_BIT;    // Clear interrupt flag on P2.0.
}
