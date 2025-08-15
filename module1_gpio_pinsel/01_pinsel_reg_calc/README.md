# 🧭 Exercise 1
## Identify `PINSEL_` and `PINMODE_` for any pin `Px.y`

> Given a pin `Px.y`, determine **which register** (`PINSEL_` / `PINMODE_`) and **which 2-bit field** configure it on LPC17xx (e.g., LPC1769).

---

## 🧠 Quick intuition

- Each **port** `x` has **32 pins** (`y = 0..31`).
- For each port, configuration is split across **two 32-bit registers**:
- **Each pin uses exactly 2 bits** inside its register.

---

## 📌 Mapping rules

| Pin number (`y`) |        Register        |     Bit field formula     |
|:----------------:|:----------------------:|:-------------------------:|
|   `0 ≤ y ≤ 15`   |      `PINSEL(2x)`      |       `[2y+1 : 2y]`       |
|                  |     `PINMODE(2x)`      |       `[2y+1 : 2y]`       |
|  `16 ≤ y ≤ 31`   |     `PINSEL(2x+1)`     | `[2*(y-16)+1 : 2*(y-16)]` |
|                  |    `PINMODE(2x+1)`     | `[2*(y-16)+1 : 2*(y-16)]` |

> **Notation:** `[msb : lsb]` uses **bit 0 as LSB**. Example: `[3:2]` means “use bits 3 and 2”.
---

## 🧮 Worked examples

### 1) `P1.8`
| x | y  | Register formula        | Register   | Bits formula                | Bits      |
|---|----|-------------------------|------------|-----------------------------|-----------|
| 1 | 8  | `PINSEL(2x)`            | PINSEL2    | `[2y+1 : 2y]`               | [17:16]   |

✅ **Answer:** `PINSEL2[17:16]` and `PINMODE2[17:16]`

---

### 2) `P0.22`
| x | y  | Register formula        | Register   | Bits formula                | Bits      |
|---|----|-------------------------|------------|-----------------------------|-----------|
| 0 | 22 | `PINSEL(2x+1)`          | PINSEL1    | `[2*(y-16)+1 : 2*(y-16)]`   | [13:12]   |

✅ **Answer:** `PINSEL1[13:12]` and `PINMODE1[13:12]`

---

### 3) `P3.26`
| x | y  | Register formula        | Register   | Bits formula                | Bits      |
|---|----|-------------------------|------------|-----------------------------|-----------|
| 3 | 26 | `PINSEL(2x+1)`          | PINSEL7    | `[2*(y-16)+1 : 2*(y-16)]`   | [21:20]   |

✅ **Answer:** `PINSEL7[21:20]` and `PINMODE7[21:20]`

---
