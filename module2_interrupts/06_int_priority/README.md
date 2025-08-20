# 🛡️ Interrupt Priorities

## 📝 Statement

> - If you have two interrupts, one from the external interrupt **EINT2** and another from **GPIO Port 2**, and both occur at the same time, how would you configure the NVIC to ensure that **EINT2** is always serviced first?
> - Which NVIC registers are needed to modify the priority of an interrupt? Explain all possible situations that can occur between two interrupts with different priorities and how the NVIC handles each case.
> - Write a small code snippet illustrating the configuration of these priorities.
> - What priority do interrupts have after a reset?

---

## 1. Configuring NVIC Priority

To ensure **EINT2** is always serviced before the GPIO Port 2 interrupt, assign a higher priority (lower numerical value) to **EINT2** in the NVIC.

## 2. NVIC Registers for Priority
### Registers to modify interrupt priority in NVIC:
- **NVIC_IPR[x]**: Interrupt Priority Registers (set priority for each interrupt).

### Situations and NVIC Behavior

- **Both interrupts pending, different priorities**: NVIC services the one with the highest priority (lowest value) first.
- **Interrupt occurs during another ISR**:
  - If new interrupt has higher priority: preempts current ISR (nested interrupt).
  - If new interrupt has lower or equal priority: waits until current ISR finishes.
- **Same priority**: The one with the lower IRQ number is serviced first (fixed priority).

## 3. Example Code

Below is a sketch for setting priorities (assuming CMSIS and LPC17xx):

```c
#include "LPC17xx.h"

void configPriorities(void) {
    NVIC_SetPriority(EINT2_IRQn, 1);    // Set EINT2 to higher priority.
    NVIC_SetPriority(EINT3_IRQn, 2);    // Set EINT3 to lower priority.
}
```

## 4. Reset Priority
According to the User manual, after a reset, all IPRx registers are set to 0x00, meaning all interrupts have the same priority (priority 0).
This means that no interrupt has a higher priority than another, and they will be serviced in the order they are received, based on their IRQ numbers.
