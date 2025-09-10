#include "LPC17xx.h"

// configuración de pines PINSEL/PINMODE
void configPCB(void);
// configuro interrupciones EINT0/P0.0 (r)
void configINT(void);
// configurar systick 200ms
void configST(void);

volatile uint8_t fBoton = 0;
volatile uint8_t cBoton = 0;
const uint8_t iBoton = 15;
volatile uint8_t pBoton = 0;

volatile uint8_t fMotor = 0;    // estado
volatile uint8_t cMotor = 0;    // contador
volatile uint8_t iMotor = 25;   // limite

int main(void) {

}

// 0.15 Barrera
// 1.4 LED ROJO
// 0.0 Botón
// 2.9 Estado
// 2.10 int auto
void configPCB(void) {
    // asumo valores por defecto
    LPC_GPIO0->FIODIR |= 1 << 15;

    LPC_GPIO1->FIODIR |= 1 << 4;

    LPC_PINCON->PINSEL4 |= 1 << 20;
}

void configINT(void) {
    LPC_SC->EXTMODE = 0x1;    // flanco
    LPC_SC->EXTPOLAR = 1 << 0;    // subida

    LPC_SC->EXTINT = 1 << 0;    // limpio eint
    NVIC_ClearPendingIRQ(EINT0_IRQn);
    NVIC_EnableIRQ(EINT0_IRQn);

    LPC_GPIOINT->IO0IntEnF |= 1 << 0;
    LPC_GPIOINT->IO0IntClr = 1 << 0;
    NVIC_EnableIRQ(EINT3_IRQn);
}

void configST(void) {
    SysTick_Config(14000000);
}

void EINT0_IRQHandler(void) {
    LPC_GPIO1->FIOCLR |= 1 << 4;    // apago led

    if (LPC_GPIO2->FIOPIN & (1 << 9)) {
        fMotor = 1;
    } else {
        fMotor = 0;
        LPC_GPIO1->FIOSET |= 1 << 4;
    }

    LPC_SC->EXTINT |= 1 << 0;
}

void EINT3_IRQHandler(void) {
    if (LPC_GPIOINT->IO0IntStatF & (1 << 0)) {
        if (fBoton == 1) {
            pBoton++;
        }
        fBoton = 1; // inicia la ventana 3s
    }
    LPC_GPIOINT->IO0IntClr = 1 << 0;
}

void SysTick_Handler(void) {
    uint8_t comp = 0;
    if (fMotor) {
        cMotor++;
        if (cMotor >= iMotor) {
            cMotor = 0;
            LPC_GPIO0->FIOCLR |= 1 << 15;
            fMotor = 0;
        }
    }
    if (fBoton) {
        cBoton++;
        if (cBoton >= iBoton) {
            fBoton = 0;
            cBoton = 0;
            comp = 1;
        }
    }
    if (comp) {
        switch (pBoton) {
            case 1:    iMotor = 50; break;
            case 2:    iMotor = 100; break;
            case 3:    iMotor = 200; break;
            default:   iMotor = 25;
        }
        comp = 0;
    }
}
