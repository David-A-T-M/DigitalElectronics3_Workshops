# ✨ Exercise 3
## Automatic Hexadecimal Counter with Direction Control via External Interrupt

## 📝 Statement

> Implement an automatic hexadecimal counter, but use an external interrupt to control the counting direction.

## 📋 Specifications

- **Input:**
  - Button connected to an external interrupt pin (e.g., **P2.13**, configured as EINT3 with pull-up).
- **Output:**
  - 7-segment display connected to **P2.0–P2.6** shows the current counter value in hexadecimal (0–F).
- **Behavior:**
  - The counter automatically increments at a fixed rate.
  - While the buttons is pressed the counting direction is reversed.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure the button on the interrupt pin has a pull-up resistor enabled.
- Use debouncing techniques if necessary to avoid multiple toggles from a single press.

---

Ready to build and test on your LPC1769 board!
