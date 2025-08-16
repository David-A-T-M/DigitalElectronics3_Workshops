# ✨ Exercise 8
## Binary to 7-Segment Display Converter

## 📝 Statement

> Use 4 GPIO input pins to read a binary value (0–15) and display the corresponding hexadecimal digit (0–F) on a 7-segment display.

## 📋 Specifications

- **Input:**
  - 4 switches or buttons connected to **P0.0–P0.3** (active low, with pull-up).
- **Output:**
  - **7-segment display** connected to **P2.0–P2.6** shows the hexadecimal digit corresponding to the binary input.
- **Behavior:**
  - The display updates in real time as the input value changes.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure pull-up resistors are enabled for the input pins.
- The display will instantly reflect any change in the input pins.

---

Ready to build and test on your LPC1769 board!
