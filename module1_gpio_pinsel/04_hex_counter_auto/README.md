# ✨ Exercise 4
## Hexadecimal Counter on 7-Segment Display

## 📝 Statement

> Use 7 GPIO pins to control a 7-segment display.  
> The program must automatically and cyclically display the 16 hexadecimal digits (0-F) in ascending order.

## 📋 Specifications

- **7-Segment Display:**
  - Type: **Common Cathode**
  - **Segment Pins Used:** `P2.0` to `P2.6` (one GPIO per segment: a-g)
- **Display Logic:**
  - Segment ON: logic `1` (since common cathode)
  - Segment OFF: logic `0`
- The program must cycle through all hexadecimal digits (0-F) automatically.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The display is **common cathode**: connect the common pin to GND.
- Each segment (a-g) is connected to `P2.0` through `P2.6`.
- Segment patterns for each digit are defined in a lookup table.
- The delay is blocking and its duration depends on the macro value.

---

Ready to build and test on your LPC1769 board!
