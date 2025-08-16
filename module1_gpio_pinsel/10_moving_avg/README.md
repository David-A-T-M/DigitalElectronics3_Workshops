# ✨ Exercise 10
## 8-bit Moving Average with GPIO

## 📝 Statement

> Write a program that performs a moving average with the last 8 data values read from port 0.
>
> - **Input:** The 8 least significant bits of Port 0, read periodically (using a delay).
> - **Output:** The result of the average, also 8 bits, must be shown on the 8 lowest pins of Port 2.

## 📋 Specifications

- **Inputs:**
  - 8 pins for input data (**P0.0–P0.7**)
- **Outputs:**
  - 8 pins for output (**P2.0–P2.7**), showing the moving average result
- **Behavior:**
  - Every cycle, read the value from **P0.0–P0.7**
  - Store the last 8 readings in a buffer
  - Compute the average (sum of last 8 readings divided by 8)
  - Output the average to **P2.0–P2.7**
  - Use a delay to control the sampling rate

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure pull-up resistors are enabled for the input pins if required.
- The output updates automatically with each new sample.

---

Ready to build and test on your LPC1769 board!
