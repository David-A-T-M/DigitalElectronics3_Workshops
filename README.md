# 🎓 Digital Electronics 3 — Workshop Projects (2025)

This repository provides a curated collection of embedded systems examples used in the **Digital Electronics 3 workshops** at FCEFyN. It is designed as a complementary resource to explore register-level programming, and driver abstractions.

---

## 🎯 Project Purpose

The main objectives of this repository are:

- Demonstrate the **contrast between register-level programming and driver-based development.**
- Provide **hands-on examples for each core hardware module** (GPIO, ADC, DAC, UART, timers, etc.).
- Support the transition from low-level register access to driver abstractions, such as the improved [CMSISv2p00_LPC17xx](https://github.com/David-A-T-M/LPC17xx-CMSIS-Driver-Enhancement) library
- Encourage students to explore different abstraction levels and understand the trade-offs between them.

---

## 🧱 Repository Organization

Each project includes implementations for LPC1769, showcasing:

- A **register-level implementation**
- A **driver-based implementation**

Repository is structured into modules, each containing various examples demonstrating the use of different peripherals and features.

> 🛠️ Toolchains used:
> - **LPC1769**: MCUXpresso IDE
    > All configuration and project setup files are IDE-managed for ease of replication.

---

## 🔬 Educational Value

This repository is actively used in _Digital Electronics 3 (ED3)_ to teach:

- **Direct register manipulation** for peripheral control
- **Progressive abstraction** using peripheral drivers
- **All code is fully documented** and complies with doxygen standards for easy understanding and learning.

---

## ⚙️ Modules Covered

Example projects are continuously added and refined. The current and planned topics include:

- ✅ GPIO and PINSEL.
- 🗓️ Interrupts.
- 🗓️ SysTick.
- 🗓️ Timers.
- 🗓️ ADC.
- 🗓️ DAC
- 🗓️ GPDMA.
- 🗓️ UART.
- 🗓️ I2C.

> 🚧 _In progress._
> 🗓️ _Planned._

---

## 🔗 Related Repository

This repository is meant to be used alongside the improved CMSIS driver library:

📦 [LPC17xx CMSISv2p00 Modernized Driver Library](https://github.com/David-A-T-M/LPC17xx-CMSIS-Driver-Enhancement)  
A modern, tested, and documented refactor of the original NXP CMSIS drivers for LPC176x/5x.

Together, these two repositories provide a complete workflow for both **educational exploration** and **professional embedded firmware development**.

---

## 👨‍🏫 Target Audience

While initially developed for students of **Digital Electronics 3**, the examples are written in a clean, modular style to benefit:

- Students transitioning from PICs to ARM.
- Engineers exploring bare-metal development.
- Developers using LPC176x.
- Anyone learning about peripheral drivers and HAL abstractions.

---

## 🤝 Contributions

Feel free to open issues or pull requests to suggest new examples, fix bugs, or improve portability. This project is open to contributions, especially from students who want to enhance their embedded development skills.

---

## 👨‍💻 Author

[David Trujillo Medina](https://github.com/David-A-T-M)

---
