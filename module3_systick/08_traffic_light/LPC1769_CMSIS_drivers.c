/**
 * @file LPC1769_CMSIS_drivers.c
 * @brief Traffic light controller with pedestrian request using SysTick and EINT0 for LPC1769.
 *
 * This file configures the SysTick timer, GPIO, and external interrupt to implement a traffic light
 * controller. The system cycles through a predefined sequence for car and pedestrian lights.
 * Pressing the button on P2.10 (EINT0) immediately switches to the pedestrian crossing phase and
 * restarts the sequence.
 */

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)         (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s)     (((0x1 << (x)) - 1) << (s))

/** Car traffic lights connected to P0.0-P0.2. */
#define CAR_LIGHT           (0)
/** Pedestrian traffic lights connected to P0.4-P0.6. */
#define PED_LIGHT           (4)
/** Button connected to P2.10 (EINT0). */
#define BTN                 (10)

/** Bit mask for car traffic lights (P0.0-P0.2). */
#define CAR_LIGHT_BITS      BITS_MASK(3, CAR_LIGHT)
/** Bit mask for pedestrian traffic lights (P0.4-P0.6). */
#define PED_LIGHT_BITS      BITS_MASK(3, PED_LIGHT)
/** Bit mask for the button (P2.10). */
#define BTN_BIT             BIT_MASK(BTN)

/** Reset time in milliseconds. */
#define STATE_TIME          (5000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME             (100)

/** Number of SysTick interrupts to achieve the desired state time. */
#define ST_MULT_STATE       ((STATE_TIME/ST_TIME) - 1)

/** Size of the traffic light sequence array. */
#define SEQ_SIZE            (sizeof(trafficSeq)/sizeof(trafficSeq[0]))

/**
 * @brief Structure representing a step in the traffic light sequence.
 */
typedef struct {
    uint8_t car;
    uint8_t ped;
} TrafficStep;

/**
 * @brief Configures GPIO pins for car and pedestrian traffic lights, and the pedestrian button.
 *
 * Sets P0.0-P0.2 as outputs for car lights, P0.4-P0.6 as outputs for pedestrian lights,
 * and P2.10 as input for the pedestrian request button (EINT0).
 * Initializes the lights to the starting state.
 */
void configGPIO(void);

/**
 * @brief Configures the external interrupt (EINT0) for the pedestrian button.
 *
 * Sets up EINT0 on P2.10 to trigger on a rising edge and enables its interrupt in the NVIC.
 */
void configInt(void);

/**
 * @brief Configures the SysTick timer to generate periodic interrupts.
 *
 * @param time Time interval in milliseconds between SysTick interrupts.
 *
 * Initializes the SysTick timer using CMSIS drivers, sets the interrupt interval,
 * and enables the SysTick interrupt and counter.
 */
void configSysTick(uint32_t time);

/** Traffic light sequence steps for cars and pedestrians. */
const TrafficStep trafficSeq[] = {
    {0x4, 0x10},   // Car red, Ped green
    {0x4, 0x10},   // Car red, Ped green
    {0x4, 0x10},   // Car red, Ped green
    {0x4, 0x10},   // Car red, Ped green
    {0x4, 0x10},   // Car red, Ped green
    {0x4, 0x20},   // Car red, Ped yellow
    {0x1, 0x40},   // Car green, Ped red
    {0x1, 0x40},   // Car green, Ped red
    {0x1, 0x40},   // Car green, Ped red
    {0x1, 0x40},   // Car green, Ped red
    {0x1, 0x40},   // Car green, Ped red
    {0x2, 0x40}    // Car yellow, Ped red
};

/** State variable for tracking the current step in the traffic light sequence. */
volatile uint32_t state = 0;
/** Counter for timing the duration of each state in the traffic light sequence. */
volatile uint8_t reset = ST_MULT_STATE;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_TIME);

    while(1) {
        __WFI();
    }
    return 0 ;
}

void configGPIO(void) {
    PINSEL_CFG_Type pinCfg = {0};                           // PINSEL configuration structure.

    pinCfg.portNum = PINSEL_PORT_0;
    pinCfg.pinNum = PINSEL_PIN_0;
    pinCfg.funcNum = PINSEL_FUNC_0;
    pinCfg.pinMode = PINSEL_PULLUP;
    pinCfg.openDrain = PINSEL_OD_NORMAL;

    PINSEL_ConfigMultiplePins(&pinCfg, CAR_LIGHT_BITS);     // P0.0 to P0.2 as GPIO with pull-up.
    PINSEL_ConfigMultiplePins(&pinCfg, PED_LIGHT_BITS);     // P0.4 to P0.6 as GPIO with pull-up.

    GPIO_SetDir(GPIO_PORT_0, CAR_LIGHT_BITS, GPIO_OUTPUT);  // P0.0 to P0.2 as outputs.
    GPIO_SetDir(GPIO_PORT_0, PED_LIGHT_BITS, GPIO_OUTPUT);  // P0.4 to P0.6 as outputs.

    pinCfg.portNum = PINSEL_PORT_2;
    pinCfg.pinNum = PINSEL_PIN_10;
    pinCfg.funcNum = PINSEL_FUNC_1;

    PINSEL_ConfigPin(&pinCfg);                              // P2.10 as EINT0 with pull-up.
    GPIO_SetDir(GPIO_PORT_2, BTN_BIT, GPIO_INPUT);          // P2.10 as input.

    GPIO_ClearPins(GPIO_PORT_0, CAR_LIGHT_BITS);
    GPIO_ClearPins(GPIO_PORT_0, PED_LIGHT_BITS);
    GPIO_SetPins(GPIO_PORT_0, trafficSeq[0].car);
    GPIO_SetPins(GPIO_PORT_0, trafficSeq[0].ped);
}

void configInt(void) {
    EXTI_CFG_Type extiCfg = {0};
    extiCfg.line = EXTI_EINT0;
    extiCfg.mode = EXTI_EDGE_SENSITIVE;
    extiCfg.polarity = EXTI_RISING_EDGE;

    EXTI_ConfigEnable(&extiCfg);                            // Configure and enable EINT0.
}

void configSysTick(uint32_t time) {
    SYSTICK_InternalInit(time);                             // Initialize SysTick.
    SYSTICK_IntCmd(ENABLE);                                 // Enable SysTick interrupt.
    SYSTICK_Cmd(ENABLE);                                    // Enable SysTick timer.

    NVIC_EnableIRQ(SysTick_IRQn);
}

void EINT0_IRQHandler(void) {
    GPIO_ClearPins(GPIO_PORT_0, CAR_LIGHT_BITS);
    GPIO_ClearPins(GPIO_PORT_0, PED_LIGHT_BITS);
    GPIO_SetPins(GPIO_PORT_0, trafficSeq[11].car);
    GPIO_SetPins(GPIO_PORT_0, trafficSeq[11].ped);

    SysTick->VAL = 0;                                       // Clear current value and interrupt flag.
    reset = ST_MULT_STATE;                                  // Reset 5 s counter.
    state = 0;                                              // Reset state counter.

    EXTI_ClearFlag(EXTI_EINT0);
}

void SysTick_Handler(void) {
    if (!reset) {                                           // 5 s elapsed.
        GPIO_ClearPins(GPIO_PORT_0, CAR_LIGHT_BITS);
        GPIO_ClearPins(GPIO_PORT_0, PED_LIGHT_BITS);
        GPIO_SetPins(GPIO_PORT_0, trafficSeq[state % SEQ_SIZE].car);
        GPIO_SetPins(GPIO_PORT_0, trafficSeq[state % SEQ_SIZE].ped);

        reset = ST_MULT_STATE;                              // Reset sequence counter.
    }

    reset--;
}
