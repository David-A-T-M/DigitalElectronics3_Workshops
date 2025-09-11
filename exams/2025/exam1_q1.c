/*  Utilizando Systick, programar la LPC1769 para que cada vez que se produzca una interrupción
    por flanco descendente en EINT2 se saque por el pin P2.4 la secuencia mostrada en la figura.
    En caso de que se produzca una nueva interrupción por EINT2 mientras se está realizando la secuencia,
    se pondrá en uno la salida P2.4 y se dará por finalizada la secuencia. El programa NO debe hacer
    uso de retardos por software y deben enmascararse los pines del puerto 2 que no van a ser utilizados.
    Suponer una frecuencia de reloj cclk de 60 Mhz (NO se pide configuración del reloj). Se pide el programa
    completo debidamente comentado y los respectivos cálculos de tiempo asignados al Systick.

Asumiendo:
    - Estado de reposo del pin P2.4 es 1, al finalizar la secuencia se pone en 1
    - Al finalizar la secuencia o luego de haberse interrumpido, una nueva presión de botón vuelve a iniciar la secuencia.
    - Todos los registros están en su valor por defecto al iniciar el programa.
Secuencia:
    - Estados de 10 ms cada uno: 0-0-0-0-1-0-1-0-1-0-1-0-0-0-0
*/

#include "LPC17xx.h"

void configPCB(void);
void configInt(void);
void configSystick(void);

volatile uint8_t seq      = 0;                   // Sequence running flag.
const uint16_t secuencia  = 0b00001010101000;    // Sequence from LSB to MSB. No need to store the first 0.
volatile uint32_t counter = 0;

int main(void) {
    configPCB();
    configInt();
    configSystick();

    while (1) {
        __WFI();
    }
}

void configPCB(void) {
    LPC_GPIO2->FIODIR |= (1 << 4);     // P2.4 as output.
    LPC_GPIO2->FIOMASK = ~(1 << 4);    // Mask all P2 except P2.4.
    LPC_GPIO2->FIOSET  = (1 << 4);     // P2.4 initial state is 1.

    LPC_PINCON->PINSEL4 |= (1 << 24);    // P2.12 as EINT2
}

void configInt(void) {
    LPC_SC->EXTMODE |= (1 << 2);    // EINT2 as edge sensitive.

    LPC_SC->EXTINT |= (1 << 2);
    NVIC_ClearPendingIRQ(EINT2_IRQn);
    NVIC_EnableIRQ(EINT2_IRQn);
}

void configSystick(void) {
    SysTick->LOAD = 599999;     // Load value for 10 ms interval.
    SysTick->VAL  = 0;          // Clear current value and interrupt flag.
    SysTick->CTRL = 1 << 1 |    // Enable SysTick exception request.
                    1 << 2;     // Use processor clock.
}

void EINT0_IRQHandler(void) {
    if (seq) {                           // Button pressed while sequence running.
        SysTick->CTRL &= ~1;             // Disable SysTick.
        LPC_GPIO2->FIOSET = (1 << 4);    // Set P2.4 to 1. Back to idle state.

        counter = 0;    // Reset sequence counter.
        seq     = 0;    // No sequence running.

        LPC_SC->EXTINT |= (1 << 2);    // Clear EINT2 interrupt flag.
        return;                        // Early return.
    }

    SysTick->VAL = 0;      // Reset current value.
    SysTick->CTRL |= 1;    // Enable SysTick.

    LPC_GPIO2->FIOCLR = (1 << 4);    // Set P2.4 to 0. First state of the sequence.

    seq = 1;    // Sequence running.

    LPC_SC->EXTINT |= (1 << 2);    // Clear EINT2 interrupt flag.
}

void SysTick_Handler(void) {
    const uint8_t estado = (secuencia >> counter) & 1;  // Get current state from sequence.

    if (estado) {
        LPC_GPIO2->FIOSET = (1 << 4);
    } else {
        LPC_GPIO2->FIOCLR = (1 << 4);
    }

    if (counter < 13) {
        counter++;
    } else {
        SysTick->CTRL &= ~1;    // Disable SysTick.

        counter = 0;    // Reset sequence counter.
        seq     = 0;    // No sequence running.

        LPC_GPIO2->FIOSET = (1 << 4);    // Set P2.4 to 1.
    }
}
