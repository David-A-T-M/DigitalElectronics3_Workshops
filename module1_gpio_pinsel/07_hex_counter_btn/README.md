# ✨ Exercise 7
## Hexadecimal Counter with 7-Segment Display and Button

## 📝 Statement

> Use 7 GPIO pins to control a 7-segment display.  
> The program must cyclically show the 16 hexadecimal digits (0-F) in ascending order.  
> The count increments each time a button connected to **P0.0** is pressed.

## 📋 Specifications

- **Input:**
  - Button connected to **P0.0** (active low, with pull-up).
- **Output:**
  - **7-segment display** connected to **P2.0–P2.6** shows the current hexadecimal digit.
- **Behavior:**
  - On each valid button press, the displayed digit increases (0 → 1 → ... → F → 0).
  - The display updates only on button press (debounced).
  - The count wraps around after F.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Button input is debounced in software.
- The display remains stable until the next button press.

---

Ready to build and test on your LPC1769 board!
