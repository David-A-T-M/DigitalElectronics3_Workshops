# ✨ Exercise 6
## Binary Counter of logic ones on Port 0

## 📝 Statement

> Write a program that reads the state of all available pins on Port 0 and counts how many are at a high logic level (1).
> The result must be displayed in binary using 5 LEDs connected to the least significant pins of Port 2.

## 📋 Specifications

- **Input:**
  - Reads all available pins of **Port 0** (`P0.0` to `P0.31`).
- **Output:**
  - **5 LEDs** connected to `P2.0`–`P2.4` display the count in binary.
  - LED ON: logic `1`, LED OFF: logic `0`.

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- The program continuously updates the LED display with the current count of high pins on Port 0.

---

Ready to build and test on your LPC1769 board!
