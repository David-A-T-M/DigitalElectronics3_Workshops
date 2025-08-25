# ✨ Exercise 5
## Multitask: LED Blink and Sequence with SysTick

## 📝 Statement

> Create a program that performs two independent tasks using periodic interrupts from the SysTick timer:
> - Blink the red LED (P0.22) every 500 ms.
> - Sequence through four LEDs (P2.0-P2.3), advancing to the next LED every 200 ms.

## 📋 Specifications

- **Outputs:**
  - Red LED on **P0.22** blinks every 500 ms.
  - Four LEDs on **P2.0-P2.3** light up in sequence, advancing every 200 ms.
- **Behavior:**
  - Both tasks run concurrently, managed by counters in the SysTick interrupt handler.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The SysTick timer is configured for a 100 ms interval.
- The main loop uses `__WFI()` to reduce CPU usage while waiting for interrupts.

---

Ready to build and test on your LPC1769 board!
