/**
 * @file LPC1769_registers.c
 * @brief Configures GPIO and external interrupt for toggling the red LED using a button on the LPC1769.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

#define BTN                 (10)
#define RED_LED             (22)

#define BTN_BIT             BIT_MASK(BTN)
#define RED_BIT             BIT_MASK(RED_LED)
#define EINT0_BIT           BIT_MASK(0)

#define BTN_PCB             BITS_MASK(2, BTN * 2)
#define RED_PCB             BITS_MASK(2, (RED_LED - 16) * 2)

#define BTN_PCB_L           BIT_MASK(BTN * 2)

/**
 * @brief Configures GPIO pins for the red LED and button.
 *
 * Sets the pin functions and directions for the red LED (output) and the button (input with pull-up).
 * Also configures the button pin for external interrupt functionality.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt for the button.
 *
 * Sets EINT0 to trigger on a falling edge, clears any pending interrupts, and enables the interrupt in the NVIC.
 */
void configInt(void);

int main(void) {
    configGPIO();
    configInt();

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL1 &= ~(RED_PCB);          // P0.22 as GPIO.
    LPC_GPIO0->FIODIR |= RED_BIT;               // P0.22 as output.

    LPC_PINCON->PINSEL4 &= ~(BTN_PCB);
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;           // P2.10 as EINT0.
    LPC_PINCON->PINMODE4 &= ~(BTN_PCB);         // P2.10 pull-up resistor.
    LPC_GPIO2->FIODIR &= ~(BTN_BIT);            // P2.10 as input.

    LPC_GPIO0->FIOCLR = RED_BIT;                // Turn off the red LED.
}

void configInt(void) {
    LPC_SC->EXTMODE |= EINT0_BIT;               // EINT0 as edge-sensitive.
    LPC_SC->EXTPOLAR &= ~EINT0_BIT;             // EINT0 falling edge.

    LPC_SC->EXTINT |= EINT0_BIT;                // Clear pending EINT0.
    NVIC_ClearPendingIRQ(EINT0_IRQn);           // Clear pending EINT0 in NVIC.
    NVIC_EnableIRQ(EINT0_IRQn);                 // Enable EINT0 interrupt in NV
}

void EINT0_IRQHandler(void) {
    const uint32_t current = LPC_GPIO2->FIOPIN;

    LPC_GPIO0->FIOSET = ~current & RED_BIT;     // Toggle LED state.
    LPC_GPIO0->FIOCLR = current & RED_BIT;

    LPC_SC->EXTINT = EINT0_BIT;                 // Clear EINT0 interrupt flag.
}
