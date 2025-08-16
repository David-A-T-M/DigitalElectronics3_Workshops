# ✨ Exercise 5
## Button Detection and LED Control

## 📝 Statement

> Write a program that detects when a push button connected to a GPIO input pin is pressed. Use another output pin to turn on an LED only while the button is pressed.

## 📋 Specifications

- **Input:**
  - A push button connected to `P0.0` (GPIO input).
- **Output:**
  - An LED connected to `P2.0` (GPIO output).
  - The LED turns on only while the button is pressed.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The program updates the LED state in real time according to the button press.

---

Ready to build and test on your LPC1769 board!
