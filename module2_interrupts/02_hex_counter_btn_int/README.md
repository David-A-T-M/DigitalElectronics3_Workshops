# ✨ Exercise 2
## Hexadecimal Counter with GPIO Interrupt

## 📝 Statement

> Implement the hexadecimal counter exercise, but this time the count increment must be handled by a rising edge interrupt on a GPIO pin.

## 📋 Specifications

- **Input:**
  - Button connected to **P0.0** (configured as GPIO input with pull-up, triggers interrupt on rising edge).
- **Output:**
  - 7-segment display connected to **P2.0–P2.6** shows the current counter value in hexadecimal (0–F).
- **Behavior:**
  - Each rising edge on **P0.0** increments the counter by one (modulo 16) and updates the display.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure the button on **P0.0** has a pull-up resistor enabled.
- Use debouncing techniques if necessary to avoid multiple toggles from a single press.

---

Ready to build and test on your LPC1769 board!
