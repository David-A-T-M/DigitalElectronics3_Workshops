# ✨ Exercise 8
## Traffic Light Controller with Pedestrian Request (SysTick + EINT0)

## 📝 Statement

> Create a program that implements a traffic light controller for cars and pedestrians. The system cycles through a predefined sequence for car and pedestrian lights, advancing every 5 seconds using the SysTick timer. Pressing a button (P2.10, EINT0) immediately switches to the pedestrian crossing phase and restarts the sequence.

## 📋 Specifications

- **Outputs:**
  - Car traffic lights on **P0.0-P0.2** (green, yellow, red).
  - Pedestrian lights on **P0.4-P0.6** (green, yellow, red).
- **Input:**
  - Button on **P2.10** (EINT0) requests pedestrian crossing.
- **Behavior:**
  - The lights cycle through a fixed sequence, advancing every 5 seconds.
  - Pressing the button immediately switches to the pedestrian crossing phase and restarts the timer.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The SysTick timer is configured for a 100 ms interval; the sequence advances every 50 interrupts (5 seconds).
- The main loop uses `__WFI()` to reduce CPU usage while waiting for interrupts.

---

Ready to build and test on your LPC1769 board!
