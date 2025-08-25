# ✨ Exercise 4
## Dual Interrupt-Controlled LED Sequences

## 📝 Statement

> Create a program that uses two interrupt sources to control different LED sequences.

## 📋 Specifications

- **Input:**
  - **P0.0**: Configured to generate an interrupt on rising edge.
  - **P2.11**: Configured as external interrupt (EINT1) on falling edge.
- **Output:**
  - RGB LEDs or 7-segment display.
- **Behavior:**
  - When an interrupt is detected on **P0.0**, execute a sequence of 4 LEDs or display 4 numbers.
  - When an external interrupt is detected on **P2.11** (EINT1), execute a different blinking sequence.
- **Priority:**
  - The external interrupt (**EINT1** on **P2.11**) must have higher priority than the GPIO interrupt (**P0.0**).

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure the button on the interrupt pin has a pull-up resistor enabled.
- Use debouncing techniques if necessary to avoid multiple toggles from a single press.

---

Ready to build and test on your LPC1769 board!
