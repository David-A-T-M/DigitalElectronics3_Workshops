# ✨ Exercise 7
## External Interrupt Counter with Auto-Reset (SysTick + EINT1)

## 📝 Statement

> Create a program that increments a counter each time a button (P2.11, EINT1) is pressed, displaying the value on LEDs (P0.0-P0.3). The counter should automatically reset to zero every 2 seconds using the SysTick timer.

## 📋 Specifications

- **Outputs:**
  - Four LEDs on **P0.0-P0.3** display the current count in binary.
- **Input:**
  - Button on **P2.11** (EINT1) increments the counter.
- **Behavior:**
  - Each button press increments the counter and updates the LEDs.
  - The counter resets to zero every 2 seconds, regardless of button presses.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The SysTick timer is configured for a 100 ms interval; the counter resets every 20 interrupts (2 seconds).
- The main loop uses `__WFI()` to reduce CPU usage while waiting for interrupts.

---

Ready to build and test on your LPC1769 board!
