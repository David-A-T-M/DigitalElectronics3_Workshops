/**
 * @file LPC1769_registers.c
 * @brief GPIO and interrupt configuration for LPC1769 RGB LED and button control.
 *
 * This file contains functions to configure GPIO pins, external interrupts,
 * and to control an RGB LED using the LPC1769 microcontroller.
 */

#include "LPC17xx.h"

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

#define BTN_A_PCB           BITS_MASK(2, BTN_A * 2)
#define BTN_B_PCB           BITS_MASK(2, BTN_B * 2)
#define RED_PCB             BITS_MASK(2, (RED_LED - 16) * 2)
#define GREEN_PCB           BITS_MASK(2, (GREEN_LED - 16) * 2)
#define BLUE_PCB            BITS_MASK(2, (BLUE_LED - 16) * 2)

#define BTN_B_PCB_L         BIT_MASK(BTN_B * 2)

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
        setLEDColor(&sequence1[i]);
        delay();
    }
    LPC_SC->EXTINT = EINT1_BIT;                     // Clear any pending EINT1 interrupt.
}

void EINT3_IRQHandler(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        setLEDColor(&sequence2[i]);
        delay();
    }
    LPC_GPIOINT->IO0IntClr = BTN_A_BIT;             // Clears the interrupt for P0.0.
}

void delay(void) {
    for (volatile uint32_t i = 0; i < DELAY; i++)
        for (volatile uint32_t j = 0; j < DELAY; j++);
}
