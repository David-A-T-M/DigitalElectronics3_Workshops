# ✨ Exercise 2
## Alternating Color Sequences on the Onboard RGB LED

## 📝 Statement

> Write a program that alternates between two or more color sequences on the onboard RGB LED.  
> The duration of each sequence must be significantly longer than the delay between colors within the same sequence.

## 📋 Specifications

- **RGB LED Pins:**
  - 🔴 Red: `P0.22`
  - 🟢 Green: `P3.25`
  - 🔵 Blue: `P3.26`
- LEDs are **active low** (turn on with logic `0`).
- The code must be modularized using functions like `configGPIO()` and `delay()`.
- The delay must be implemented in software using a `for` loop.
- The delay time and the sequence duration should be defined with macros (`#define`).
- The program must alternate between at least two different color sequences.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Each sequence consists of a set of colors that are displayed in order.
- The time spent on each sequence is much longer than the time spent on each color.
- The delay is blocking and its duration depends on the macro values.

---

Ready to build and test on your LPC1769 board!
