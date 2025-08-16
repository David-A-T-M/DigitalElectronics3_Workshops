/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Implements 4-bit ALU operations (add, subtract) using GPIO on LPC1769.
 */
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"

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
        buffer[i % BUFFER_SIZE] = GPIO_ReadValue(GPIO_PORT_0) & INPUT_MASK;
        sum += buffer[i % BUFFER_SIZE];

        const uint8_t avg = sum / BUFFER_SIZE;
        GPIO_ClearPins(GPIO_PORT_2, OUTPUT_MASK);
        GPIO_SetPins(GPIO_PORT_2, avg);

        i++;
        delay();
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};

    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, INPUT_MASK);
    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, OUTPUT_MASK);

    GPIO_SetDir(GPIO_PORT_0, INPUT_MASK, GPIO_INPUT);   // P0.0–P0.7 as input
    GPIO_SetDir(GPIO_PORT_2, OUTPUT_MASK, GPIO_OUTPUT); // P2.0–P2.7 as output

    GPIO_ClearPins(GPIO_PORT_2, OUTPUT_MASK); // Clear outputs
}

void delay() {
    for(uint32_t i=0; i<DELAY; i++)
        for(uint32_t j=0; j<DELAY; j++);
}
