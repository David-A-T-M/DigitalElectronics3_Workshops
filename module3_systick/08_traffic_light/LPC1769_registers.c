/**
 * @file LPC1769_registers.c
 * @brief Traffic light controller with pedestrian request using SysTick and EINT0 for LPC1769.
 *
 * This file configures the SysTick timer, GPIO, and external interrupt to implement a traffic light
 * controller. The system cycles through a predefined sequence for car and pedestrian lights.
 * Pressing the button on P2.10 (EINT0) immediately switches to the pedestrian crossing phase and
 * restarts the sequence.
 */

#include "LPC17xx.h"

/** Generic bit mask macro. */
#define BIT_MASK(x)     (0x1 << (x))
/** Generic n-bit mask macro. */
#define BITS_MASK(x, s) (((0x1 << (x)) - 1) << (s))

/** Car traffic lights connected to P0.0-P0.2. */
#define CAR_LIGHT (0)
/** Pedestrian traffic lights connected to P0.4-P0.6. */
#define PED_LIGHT (4)
/** Button connected to P2.10 (EINT0). */
#define BTN       (10)

/** Bit mask for car traffic lights (P0.0-P0.2). */
#define CAR_LIGHT_BITS BITS_MASK(3, CAR_LIGHT)
/** Bit mask for pedestrian traffic lights (P0.4-P0.6). */
#define PED_LIGHT_BITS BITS_MASK(3, PED_LIGHT)
/** Bit mask for the button (P2.10). */
#define BTN_BIT        BIT_MASK(BTN)
/** External interrupt 0 bit mask. */
#define EINT0_BIT      BIT_MASK(0)

/** PCB mask for car traffic lights (P0.0-P0.2). */
#define CAR_LIGHT_PCB BITS_MASK(6, CAR_LIGHT * 2)
/** PCB mask for pedestrian traffic lights (P0.4-P0.6). */
#define PED_LIGHT_PCB BITS_MASK(6, PED_LIGHT * 2)
/** PCB mask for the button (P2.10). */
#define BTN_PCB       BITS_MASK(2, BTN * 2)
/** PCB lower bit mask for the button (P2.10). */
#define BTN_PCB_L     BIT_MASK(BTN * 2)

/** Reset time in milliseconds. */
#define STATE_TIME (5000)
/** SysTick timer interval in milliseconds. */
#define ST_TIME    (100)

/** SysTick load value for the desired time interval. */
#define ST_LOAD       ((ST_TIME * 100000) - 1)
/** Number of SysTick interrupts to achieve the desired state time. */
#define ST_MULT_STATE ((STATE_TIME / ST_TIME) - 1)
/** SysTick enable bit mask. */
#define ST_ENABLE     BIT_MASK(0)
/** SysTick interrupt enable bit mask. */
#define ST_TICKINT    BIT_MASK(1)
/** SysTick clock source bit mask. */
#define ST_CLKSOURCE  BIT_MASK(2)

/** Size of the traffic light sequence array. */
#define SEQ_SIZE (sizeof(trafficSeq) / sizeof(trafficSeq[0]))

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
 * @param ticks Number of clock cycles between SysTick interrupts.
 *
 * Loads the specified value, clears the current counter, and enables
 * the SysTick timer and its interrupt.
 */
void configSysTick(uint32_t ticks);

/** Traffic light sequence steps for cars and pedestrians. */
const TrafficStep trafficSeq[] = {
    {0x4, 0x10},    // Car red, Ped green
    {0x4, 0x10},    // Car red, Ped green
    {0x4, 0x10},    // Car red, Ped green
    {0x4, 0x10},    // Car red, Ped green
    {0x4, 0x10},    // Car red, Ped green
    {0x4, 0x20},    // Car red, Ped yellow
    {0x1, 0x40},    // Car green, Ped red
    {0x1, 0x40},    // Car green, Ped red
    {0x1, 0x40},    // Car green, Ped red
    {0x1, 0x40},    // Car green, Ped red
    {0x1, 0x40},    // Car green, Ped red
    {0x2, 0x40}     // Car yellow, Ped red
};

/** State variable for tracking the current step in the traffic light sequence. */
volatile uint32_t state = 0;
/** Counter for timing the duration of each state in the traffic light sequence. */
volatile uint8_t reset = ST_MULT_STATE;

int main(void) {
    configGPIO();
    configInt();
    configSysTick(ST_LOAD);

    while (1) {
        __WFI();
    }
    return 0;
}

void configGPIO(void) {
    LPC_PINCON->PINSEL0 &= ~CAR_LIGHT_PCB;    // P0.0 to P0.2 as GPIO.
    LPC_GPIO0->FIODIR |= CAR_LIGHT_BITS;      // P0.0 to P0.2 as outputs.

    LPC_PINCON->PINSEL0 &= ~PED_LIGHT_PCB;    // P0.4 to P0.6 as GPIO.
    LPC_GPIO0->FIODIR |= PED_LIGHT_BITS;      // P0.4 to P0.6 as outputs.

    LPC_PINCON->PINSEL4 &= ~BTN_PCB;
    LPC_PINCON->PINSEL4 |= BTN_PCB_L;    // P2.10 as EINT0.
    LPC_PINCON->PINMODE4 &= ~BTN_PCB;    // P2.10 pull-up.
    LPC_GPIO2->FIODIR &= ~BTN_BIT;       // P2.10 as input.

    LPC_GPIO0->FIOCLR = CAR_LIGHT_BITS;    // Turn off first traffic light.
    LPC_GPIO0->FIOCLR = PED_LIGHT_BITS;    // Turn off second traffic light.
    LPC_GPIO0->FIOSET = trafficSeq[0].car;
    LPC_GPIO0->FIOSET = trafficSeq[0].ped;
}

void configInt(void) {
    LPC_SC->EXTMODE |= EINT0_BIT;     // EINT0 edge sensitive.
    LPC_SC->EXTPOLAR |= EINT0_BIT;    // EINT0 rising edge.

    LPC_SC->EXTINT = EINT0_BIT;          // Clear EINT0 flag
    NVIC_ClearPendingIRQ(EINT0_IRQn);    // Clear pending interrupt.
    NVIC_EnableIRQ(EINT0_IRQn);          // Enable EINT0 interrupt in the NVIC.
}

void configSysTick(uint32_t ticks) {
    SysTick->LOAD = ticks;           // Load value for 100 ms interval.
    SysTick->VAL  = 0;               // Clear current value and interrupt flag.
    SysTick->CTRL = ST_ENABLE |      // Enable SysTick interrupt.
                    ST_TICKINT |     // Enable SysTick exception request.
                    ST_CLKSOURCE;    // Use processor clock.
}

void EINT0_IRQHandler(void) {
    LPC_GPIO0->FIOCLR = CAR_LIGHT_BITS;
    LPC_GPIO0->FIOCLR = PED_LIGHT_BITS;
    LPC_GPIO0->FIOSET = trafficSeq[11].car;
    LPC_GPIO0->FIOSET = trafficSeq[11].ped;

    SysTick->VAL = 0;    // Clear current value and interrupt flag.

    reset = ST_MULT_STATE;    // Reset 5 s counter.
    state = 0;                // Reset state counter.

    LPC_SC->EXTINT |= EINT0_BIT;    // Clear flag.
}

void SysTick_Handler(void) {
    if (!reset) {    // 5 s elapsed.
        LPC_GPIO0->FIOCLR = CAR_LIGHT_BITS;
        LPC_GPIO0->FIOCLR = PED_LIGHT_BITS;
        LPC_GPIO0->FIOSET = trafficSeq[state % SEQ_SIZE].car;
        LPC_GPIO0->FIOSET = trafficSeq[state % SEQ_SIZE].ped;

        reset = ST_MULT_STATE;    // Reset sequence counter.
    }

    reset--;
}
