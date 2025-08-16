/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Counts high pins on Port 0 and displays the result in binary on 5 LEDs (P2.0–P2.4).
 */

#include "LPC17xx_gpio.h"
#include "LPC17xx_pinsel.h"

/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Mask for the 5 LEDs connected to P2.0-P2.4. */
#define LEDS_MASK           BITS_MASK(5, 0)

/** Mask for the available pins on port 0. */
#define PORT0_AV_MASK       (0x7FFF8FFF)

/**
 * @brief Configures the GPIO pins for the LEDs and input port.
 *
 * Sets P0.0-P0.15 and P1.0-P1.31 as GPIO,
 * P2.0-P2.4 as output for LEDs, and P0.0-P0.31 as input.
 * Sets pull-up resistors for P0 and P1.
 * Clears the LEDs at startup.
 */
void configGPIO(void);

/**
 * @brief Counts the number of bits set to 1 in a 32-bit value.
 *
 * @return Number of bits set to 1 in the input value.
 */
uint8_t countOnes(uint32_t value);

int main(void) {
    configGPIO();

    while (1) {
        const uint8_t leds = countOnes(GPIO_ReadValue(GPIO_PORT_0) & PORT0_AV_MASK);

        GPIO_ClearPins(GPIO_PORT_2, LEDS_MASK);             // Turn off all LEDs.
        GPIO_SetPins(GPIO_PORT_2, leds);                    // Displays the count on the LEDs
    }
    return 0;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                           // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, PORT0_AV_MASK);      // P0.0-P0.31 as GPIO with pull-up.

    pinCfg.portNum = PINSEL_PORT_2;
    PINSEL_ConfigMultiplePins(&pinCfg, LEDS_MASK);          // P2.0-P2.4 as GPIO.

    GPIO_SetDir(GPIO_PORT_0, PORT0_AV_MASK, GPIO_INPUT);    // P0.0-P0.31 as input.
    GPIO_SetDir(GPIO_PORT_2, LEDS_MASK, GPIO_OUTPUT);       // P2.0-P2.4 as output.

    GPIO_ClearPins(GPIO_PORT_2, LEDS_MASK);                 // Turn off the LEDs.
}

uint8_t countOnes(uint32_t value) {
    uint8_t count = 0;

    for (uint8_t i = 0; i < 32; i++)
        if (value & (0x1 << i))
            count++;

    return count;
}
