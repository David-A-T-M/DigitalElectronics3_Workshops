/**
 * @file LPC1769_registers.c
 * @brief Controls an RGB LED with two buttons and interrupt sequences on the LPC1769.
 *
 * This example configures GPIO pins and external interrupts for two buttons (P0.0, P2.11)
 * and an RGB LED (P0.22, P3.25, P3.26). Pressing each button triggers an interrupt that
 * cycles the RGB LED through a predefined color sequence.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Button connected to P0.0. */
#define BTN_A               (0)
/** Button connected to P2.11. */
#define BTN_B               (11)
/** Red LED connected to P0.22. */
#define RED_LED             (22)
/** Green LED connected to P3.25. */
#define GREEN_LED           (25)
/** Blue LED connected to P3.26. */
#define BLUE_LED            (26)

/** Bit mask for button A (P0.0). */
#define BTN_A_BIT           BIT_MASK(BTN_A)
/** Bit mask for button B (P2.11). */
#define BTN_B_BIT           BIT_MASK(BTN_B)
/** Bit mask for the red LED (P0.22). */
#define RED_BIT             BIT_MASK(RED_LED)
/** Bit mask for the green LED (P3.25). */
#define GREEN_BIT           BIT_MASK(GREEN_LED)
/** Bit mask for the blue LED (P3.26). */
#define BLUE_BIT            BIT_MASK(BLUE_LED)
/** Bit mask for external interrupt 1 (EINT1). */
#define EINT1_BIT           BIT_MASK(1)

/** PCB mask for button A (P0.0). */
#define BTN_A_PCB           BITS_MASK(2, BTN_A * 2)
/** PCB mask for button B (P2.11). */
#define BTN_B_PCB           BITS_MASK(2, BTN_B * 2)
/** PCB mask for the red LED (P0.22). */
#define RED_PCB             BITS_MASK(2, (RED_LED - 16) * 2)
/** PCB mask for the green LED (P3.25). */
#define GREEN_PCB           BITS_MASK(2, (GREEN_LED - 16) * 2)
/** PCB mask for the blue LED (P3.26). */
#define BLUE_PCB            BITS_MASK(2, (BLUE_LED - 16) * 2)
/** PCB lower bit mask for button B (P2.11). */
#define BTN_B_PCB_L         BIT_MASK(BTN_B * 2)

/** Number of colors in each sequence. */
#define SEQUENCE_LENGTH     (3)
/** Delay constant for LED timing. */
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

/** Color sequence for button A (P0.0). */
const Color sequence1[SEQUENCE_LENGTH] = {YELLOW, CYAN, MAGENTA};
/** Color sequence for button B (P2.11). */
const Color sequence2[SEQUENCE_LENGTH] = {RED, GREEN, BLUE};

int main(void) {
    configGPIO();
    configInt();

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);              // P0.22 as GPIO.
    LPC_GPIO0->FIODIR |= RED_BIT;                   // P0.22 as output.

    LPC_PINCON->PINSEL7 &= ~(GREEN_PCB | BLUE_PCB); // P2.25 and P2.26 as GPIO.
    LPC_GPIO3->FIODIR |= (GREEN_BIT | BLUE_BIT);    // P2.25 and P2.26 as output.

    LPC_PINCON->PINSEL0 &= ~(BTN_A_PCB);            // P0.0 as GPIO.
    LPC_PINCON->PINMODE0 &= ~(BTN_A_PCB);           // P0.0 with pull-up.
    LPC_GPIO0->FIODIR &= ~(BTN_A_BIT);              // P0.0 as input.

    LPC_PINCON->PINSEL4 &= ~(BTN_B_PCB);
    LPC_PINCON->PINSEL4 |= BTN_B_PCB_L;             // P2.11 as EINT3.
    LPC_PINCON->PINMODE4 &= ~(BTN_B_PCB);           // P2.11 with pull-up.

}

void configInt(void) {
    LPC_GPIOINT->IO0IntEnR |= BTN_A_BIT;            // Enable rising edge interrupt on P0.0.

    LPC_SC->EXTMODE |= EINT1_BIT;                   // EINT1 edge-sensitive.
    LPC_SC->EXTPOLAR &= ~(EINT1_BIT);               // EINT1 falling edge active.

    NVIC_SetPriority(EINT1_IRQn, 0);
    NVIC_SetPriority(EINT3_IRQn, 1);

    LPC_SC->EXTINT |= EINT1_BIT;                    // Clear EINT1 flag.
    NVIC_ClearPendingIRQ(EINT1_IRQn);               // Clear pending EINT1 interrupt.
    NVIC_EnableIRQ(EINT1_IRQn);                     // Enable EINT1 interrupt in NVIC.

    LPC_GPIOINT->IO0IntClr = BTN_A_BIT;             // Clear pin flag.
    NVIC_EnableIRQ(EINT3_IRQn);                     // Enable EINT3 interrupt in NVIC.
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

void EINT1_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence2[i]);
        delay();
    }
    LPC_SC->EXTINT = EINT1_BIT;                     // Clear any pending EINT1 interrupt.
}

void EINT3_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence1[i]);
        delay();
    }
    LPC_GPIOINT->IO0IntClr = BTN_A_BIT;             // Clears the interrupt for P0.0.
}

void delay(void) {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
