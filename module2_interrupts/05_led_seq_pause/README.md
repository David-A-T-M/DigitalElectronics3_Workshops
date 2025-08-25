# ✨ Exercise 5
## Automatic 8-LED Sequence with Pause/Resume via GPIO Interrupt

## 📝 Statement

> Write a program that runs an automatic sequence of 8 LEDs. When a button connected to a GPIO input is pressed, the sequence must pause. Pressing the button again resumes the sequence.

## 📋 Specifications

- **Input:**
  - Button connected to **P2.0** (configured as GPIO input with pull-up, triggers interrupt on falling edge).
- **Output:**
  - RGB LED (or 8-LED sequence using color combinations).
- **Behavior:**
  - The LED sequence runs automatically, cycling through 8 colors.
  - Pressing the button pauses the sequence; pressing again resumes it.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure the button on the interrupt pin has a pull-up resistor enabled.
- Use debouncing techniques if necessary to avoid multiple toggles from a single press.

---

Ready to build and test on your LPC1769 board!
