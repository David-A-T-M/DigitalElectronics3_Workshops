# ✨ Exercise 1
## LED Toggle with External Interrupt

## 📝 Statement

> Create a program that toggles an LED using an external interrupt and a button.

## 📋 Specifications

- **Input:**
  - Button connected to **P2.10** (configured as EINT0, active low).
- **Output:**
  - LED connected to **P0.22** toggles state on each button press (falling edge).
- **Behavior:**
  - The LED changes state every time the button is pressed and released (falling edge detected).

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure pull-up resistors are enabled for the input pins.
- Use debouncing techniques if necessary to avoid multiple toggles from a single press.

---

Ready to build and test on your LPC1769 board!
