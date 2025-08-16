# ✨ Exercise 9
## 4-bit ALU: Addition and Subtraction with Overflow Indicator

## 📝 Statement

> Use two groups of 4 input pins (**A_in**, **B_in**) and a switch to select the operation.
>
> - The switch (connected to a pin) decides whether **A_in** and **B_in** are added or subtracted.
> - The absolute value of the result must be shown on 4 output LEDs.
> - Use an additional LED to indicate if an overflow occurs during addition or if the subtraction result is negative.

## 📋 Specifications

- **Inputs:**
  - 4 pins for **A_in** (e.g., P0.0–P0.3)
  - 4 pins for **B_in** (e.g., P0.4–P0.7)
  - 1 pin for the operation select switch (e.g., P0.8)
- **Outputs:**
  - 4 LEDs to display the absolute value of the result (e.g., P2.0–P2.3)
  - 1 LED to indicate overflow or negative result (e.g., P2.4)
- **Behavior:**
  - If the switch is high, perform addition (**A_in + B_in**).
    - The overflow LED lights up if the result exceeds 4 bits.
  - If the switch is low, perform subtraction (**A_in - B_in**).
    - The overflow LED lights up if the result is negative.
  - The LEDs always display the absolute value (0–15).

## 🛠️ Included Versions

- [**Direct register access version**](LPC1769_registers.c)
- [**CMSIS drivers version**](LPC1769_CMSIS_drivers.c)

Both versions meet the specification and allow you to compare register-level programming with driver-based development.

## 🚦 Notes

- Ensure pull-up resistors are enabled for the input pins.
- The LEDs instantly reflect any change in the inputs or operation selection.

---

Ready to build and test on your LPC1769 board!
