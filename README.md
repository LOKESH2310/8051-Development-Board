# 🧠 8051 Development Board – Bring-Up from Scratch

> A complete ground-up development of an 8051-based embedded system featuring external memory, in-circuit programming, RS-232 serial communication, EEPROM, LCD, DAC, and custom device drivers. Developed at University of Colorado Boulder.

---

## 📌 Project Overview

This project demonstrates a full-cycle embedded system design around the **AT89C51RC2 (8051)** microcontroller. The board was built from scratch, covering hardware prototyping, memory decoding, serial interfaces, and firmware development in **C and Assembly**. It serves as an educational platform to learn low-level embedded design and system bring-up techniques.

---

## 🛠️ Hardware Architecture

### ✅ Core Components
- **Microcontroller:** AT89C51RC2 (8051 family, In-System Programmable)
- **Clock Source:** 11.0592 MHz Crystal Oscillator
- **Power Supply:** Regulated +5V (using 7805 and filtering capacitors)
- **Reset Circuit:** Manual and power-on reset using RC circuit

### 🧩 Memory Expansion
- **External NVRAM:** 32 KB, split as:
  - XRAM (Data)
  - EEPROM-like (Config Storage)
- **Latches:** 74HC573 for address/data demultiplexing
- **Glue Logic:** AT16V8C SPLD programmed using WinCUPL

### 🔌 Interfaces
- **Serial Communication:** RS-232 via MAX232 level shifter
- **LCD Display:** 16x4 character LCD (memory-mapped)
- **Serial EEPROM:** Bit-banged I²C using GPIO
- **DAC Output:** For analog signal generation
- **Push Buttons & LEDs:** User input and visual feedback
- **Test Points:** For oscilloscope/logic analyzer debugging

---

## 💻 Firmware Highlights

- **Language:** Embedded C (SDCC) and 8051 Assembly
- **Toolchain:** SDCC compiler, PuTTY for serial debug, WinCUPL for SPLD logic
- **Protocols:** 
  - UART for PC communication
  - Bit-banged I²C for EEPROM
  - Memory-mapped access for LCD & NVRAM
- **Drivers Developed:**
  - UART with command-line interface
  - LCD driver with pointer-based updates
  - I²C EEPROM read/write
  - DAC initialization and output
- **Utilities:**
  - Hex dumping of memory
  - Debug logging over UART
  - EEPROM-based configuration persistence

---

## 🔧 Getting Started

### 1. Prerequisites
- SDCC toolchain installed on your system
- RS-232 interface or USB-to-Serial converter
- PuTTY or any terminal emulator
- Logic analyzer or oscilloscope (optional for debugging)
- WinCUPL software (for PLD programming)

---

## 🧪 Testing & Validation

- Verified UART baud rate and transmission integrity using PuTTY
- Address decoding tested by writing data patterns to NVRAM and reading back
- Used logic analyzer to observe ALE, RD̅, WR̅, and CS signals
- Oscilloscope measurements for DAC waveform accuracy
- Performed functional test cases on LCD, EEPROM, and memory-mapped I/O

---

## 🚀 Features

- Fully in-system programmable (no chip removal needed)
- Modular hardware layout for expansion
- Hybrid firmware (Assembly + C)
- Dynamic heap management simulated via EEPROM
- Complete bare-metal peripheral control and debugging capability

---

## 📚 Learnings & Outcomes

- Deep understanding of 8051 architecture and instruction set
- Hands-on experience with address decoding and memory interfacing
- Mastery of serial protocols, glue logic, and device driver design
- Familiarity with embedded debugging tools and signal validation

---

## 👨‍🔬 Tools Used

| Tool           | Purpose                              |
|----------------|--------------------------------------|
| SDCC           | Firmware compilation (C)             |
| WinCUPL        | Glue logic programming (AT16V8C)     |
| PuTTY          | Serial communication                 |
| Logic Analyzer | Signal debugging                     |
| Oscilloscope   | Analog output & timing verification  |
| Altium/KiCad   | Schematic design                     |

---

## 🧠 Author

**Lokesh Senthil Kumar**  
Graduate Student – Embedded Systems  
University of Colorado Boulder  


