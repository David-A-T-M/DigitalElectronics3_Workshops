/**
 * @file LPC1769_registers.c
 * @brief Demonstrates configuring GPIO and external interrupts on the LPC1769.
 *
 * This example sets up a red LED on P0.22 and a button on P2.10.
 * Pressing the button triggers an external interrupt (EINT0) that toggles the LED.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Red LED connected to P0.22. */
#define RED_LED (22)
/** Button connected to P2.10. */
#define BTN     (10)

/** Mask for the red LED (P0.22). */
#define RED_BIT   BIT_MASK(RED_LED)
/** Mask for the button connected. */
#define BTN_BIT   BIT_MASK(BTN)
/** Mask for the EINT0 interrupt. */
#define EINT0_BIT BIT_MASK(0)

/** PCB mask for the red LED (P0.22). */
#define RED_PCB   BITS_MASK(2, (RED_LED - 16) * 2)
/** PCB mask for the button connected. */
#define BTN_PCB   BITS_MASK(2, BTN * 2)
/** PCB lower bit mask for the button connected. */
#define BTN_PCB_L BIT_MASK(BTN * 2)

/**
 * @brief Initializes GPIO pins for the red LED and button.
 *
 * Configures P0.22 as a GPIO output for the red LED and P2.10 as an input with pull-up for the button.
 * Sets P2.10 to function as an external interrupt (EINT0) source.
 * Ensures the LED is initially turned off.
 */
void configGPIO(void);

/**
 * @brief Sets up the external interrupt for the button on P2.10 (EINT0).
 *
 * Configures EINT0 to trigger on a falling edge, clears any pending interrupt flags,
 * and enables the interrupt in the NVIC.
 */
void configInt(void);

int main(void) {
    configGPIO();
    configInt();

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);    // P0.22 as GPIO.
    LPC_GPIO0->FIODIR |= RED_BIT;         // P0.22 as output.

    LPC_PINCON->PINSEL4 &= ~(BTN_PCB);
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;      // P2.10 as EINT0.
    LPC_PINCON->PINMODE4 &= ~(BTN_PCB);    // P2.10 pull-up resistor.
    LPC_GPIO2->FIODIR &= ~(BTN_BIT);       // P2.10 as input.

    LPC_GPIO0->FIOCLR = RED_BIT;    // Turn off the red LED.
}

void configInt(void) {
    LPC_SC->EXTMODE |= EINT0_BIT;      // EINT0 as edge-sensitive.
    LPC_SC->EXTPOLAR &= ~EINT0_BIT;    // EINT0 falling edge.

    LPC_SC->EXTINT |= EINT0_BIT;         // Clear pending EINT0.
    NVIC_ClearPendingIRQ(EINT0_IRQn);    // Clear pending EINT0 in NVIC.
    NVIC_EnableIRQ(EINT0_IRQn);          // Enable EINT0 interrupt in NV
}

void EINT0_IRQHandler(void) {
    const uint32_t current = LPC_GPIO2->FIOPIN;    // Read port 2.

    LPC_GPIO0->FIOSET = ~current & RED_BIT;    // Toggle LED state.
    LPC_GPIO0->FIOCLR = current & RED_BIT;

    LPC_SC->EXTINT = EINT0_BIT;    // Clear EINT0 interrupt flag.
}
