# ✨ Exercise 6
## LED Sequence with Toggle Control (SysTick + EINT0)

## 📝 Statement

> Create a program that sequences through eight LEDs (P0.0-P0.7), advancing to the next LED every 250 ms using the SysTick timer. The sequence can be started or stopped by pressing a button connected to P2.10 (EINT0).

## 📋 Specifications

- **Outputs:**
  - Eight LEDs on **P0.0-P0.7** light up in sequence, advancing every 250 ms.
- **Input:**
  - Button on **P2.10** (EINT0) toggles the sequence on or off.
- **Behavior:**
  - The LED sequence runs only when enabled by the button. Each press toggles the sequence state.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The SysTick timer is configured for a 50 ms interval; the sequence advances every 5 interrupts (250 ms).
- The main loop uses `__WFI()` to reduce CPU usage while waiting for interrupts.

---

Ready to build and test on your LPC1769 board!
