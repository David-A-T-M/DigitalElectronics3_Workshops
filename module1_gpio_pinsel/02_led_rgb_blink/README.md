# ✨ Exercise 2
## Using the Onboard RGB LED

## 📝 Statement

> Develop a program that blinks a color on the onboard RGB LED of the LPC1769 board.

## 📋 Specifications

- **RGB LED Pins:**
  - 🔴 Red: `P0.22`
  - 🟢 Green: `P3.25`
  - 🔵 Blue: `P3.26`
- LEDs are **active low** (turn on with logic `0`).
- The code must be modularized using functions like `configGPIO()` and `delay()`.
- The delay must be implemented in software using a `for` loop.
- The delay time should be defined with a macro (`#define`).

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The blinking color chosen is **red**.
- The delay is blocking and its duration depends on the macro value.

---

Ready to build and test on your LPC1769 board!
