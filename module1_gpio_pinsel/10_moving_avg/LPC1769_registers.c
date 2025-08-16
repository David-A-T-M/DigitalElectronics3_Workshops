/**
 * @file LPC1769_registers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */
#include "LPC17xx.h"

#define INPUT_MASK  0xFF      // P0.0–P0.7
#define OUTPUT_MASK 0xFF      // P2.0–P2.7
#define BUFFER_SIZE 8
#define DELAY       2500

/**
 * @brief Configures GPIO pins P2.0-P2.6 as outputs to control a 7-segment display.
 */
void configGPIO(void);

/**
 * @brief Generates a blocking delay using nested loops.
 * Used to control the LED blink timing.
 */
void delay();

int main(void) {
    configGPIO();

    uint32_t i = 0;
    uint8_t buffer[BUFFER_SIZE] = {0};
    uint16_t sum = 0;

    while (1) {
        sum -= buffer[i % BUFFER_SIZE];
        buffer[i % BUFFER_SIZE] = LPC_GPIO0->FIOPIN & INPUT_MASK;
        sum += buffer[i % BUFFER_SIZE];

        const uint8_t avg = sum / BUFFER_SIZE;
        LPC_GPIO2->FIOCLR = OUTPUT_MASK;
        LPC_GPIO2->FIOSET = avg;

        i++;
        delay();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~(0xFFFF); // Set P0.0-P0.7 as GPIO
    LPC_PINCON->PINSEL4 &= ~(0xFFFF); // Set P2.0-P2.7 as GPIO

    LPC_GPIO0->FIODIR &= ~INPUT_MASK;   // P0.0–P0.7 as input
    LPC_GPIO2->FIODIR |= OUTPUT_MASK;   // P2.0–P2.7 as output

    LPC_GPIO2->FIOCLR = OUTPUT_MASK; // Clear outputs
}

void delay() {
    for(uint32_t i=0; i<DELAY; i++)
        for(uint32_t j=0; j<DELAY; j++);
}
