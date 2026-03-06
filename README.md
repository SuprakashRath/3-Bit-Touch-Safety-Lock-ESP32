# 3-Bit Touch Safety Lock — ESP32

A small interrupt-driven touch lock built on the ESP32.  
This project uses three capacitive touch inputs as a **3-bit binary encoder** and a fourth touch input as a **submit trigger**.

When the submit pad is touched, the system captures a **stable snapshot** of the binary input, converts it to decimal feedback through LED blinking, and validates it against a predefined password sequence.

The main focus of this project is building a **reliable interrupt-driven input system** that avoids race conditions between hardware interrupts and the main program loop.

---

## Overview

This project demonstrates how a simple touch interface can be used to build a basic digital lock.

Key behaviours:

- Three touch inputs represent a **3-bit binary number (1,2,4)**.
- A **submit touch pad** captures the current value.
- A **red LED blinks the decimal equivalent** of the captured number.
- A **password sequence (default: 1 → 2 → 3)** must be entered correctly.
- If the sequence matches, the **built-in LED turns on to indicate unlock**.

The design ensures that interrupt-triggered inputs never overwrite data while the main program is processing it.

---

# Hardware Components

- ESP32 Development Board
- Breadboard
- Jumper wires
- 3 × resistive touch electrodes (simple exposed resistor leads)
- 1 × touch electrode for **submit**
- 3 × green LEDs (bit state indicators)
- 1 × red LED (decimal output)
- Current limiting resistors for LEDs

---

# Pin Mapping

| Function | GPIO |
|--------|------|
| Touch Bit 0 (LSB - weight 1) | 27 |
| Touch Bit 1 (weight 2) | 33 |
| Touch Bit 2 (MSB - weight 4) | 32 |
| Submit Touch | 14 |
| Decimal Blink LED (Red) | 12 |
| Bit Indicator LED 0 | 25 |
| Bit Indicator LED 1 | 26 |
| Bit Indicator LED 2 | 13 |
| Unlock Indicator | LED_BUILTIN |

---

# How the System Works

## 1. Binary Touch Input

Each touch pad represents a binary bit.

Touching a pad toggles its bit using a **bitwise XOR operation**.

Example:

| Bit Pattern | Decimal |
|-------------|---------|
| 001 | 1 |
| 010 | 2 |
| 011 | 3 |
| 100 | 4 |
| 101 | 5 |
| 110 | 6 |
| 111 | 7 |

Green LEDs show the current bit states.

---

## 2. Submit Snapshot Mechanism

The submit pad triggers a separate interrupt that performs three steps:

1. Copy the current binary value (`count`) into a **snapshot variable (`n`)**
2. Reset the input state so the user can start entering the next value
3. Signal the main loop to process the captured value

This ensures the system always evaluates a **stable value**, preventing data corruption from concurrent interrupts.

---

## 3. Password Validation

The password is defined in the code as:

```cpp
const int passwd[3] = {1,2,3};
```

Each submitted value must match the next expected number.

Example input sequence:

```
1 → 2 → 3
```

If the full sequence matches, the system unlocks.

---

## 4. Decimal Feedback LED

After a value is submitted:

- The red LED blinks **n times**, where `n` is the decimal value of the binary input.
- This feedback is implemented using **non-blocking timing (`millis()`)** so the program remains responsive.

---

# Running the Project

### 1. Clone the repository

```
https://github.com/SuprakashRath/3-Bit-Touch-Safety-Lock-ESP32.git
```

### 2. Install ESP32 board support

In Arduino IDE:

```
File → Preferences → Additional Boards Manager URLs
```

Add:

```
https://dl.espressif.com/dl/package_esp32_index.json
```

Then install **ESP32 by Espressif Systems** from the Boards Manager.

---

### 3. Upload the code

- Select your ESP32 board
- Connect the board via USB
- Upload `main.ino`

---

### 4. Open Serial Monitor

Set baud rate to:

```
115200
```

You will see messages like:

```
Submit Triggered! Evaluating Snapshot: 3
Correct digit: progress++
System Unlocked!
```

---

# Using the Lock

1. Touch the electrodes to set the binary number.
2. Observe the **green LEDs** showing active bits.
3. Touch the **submit pad**.
4. The **red LED blinks the decimal value**.
5. Enter the correct sequence to unlock.

Default sequence:

```
1 → 2 → 3
```

---

# Possible Improvements

Future improvements I may explore:

- Add an **OLED display** for better feedback
- Store password in **non-volatile memory**
- Add **timeout / lockout** after repeated incorrect attempts
- Build a **PCB version** of the touch interface
- Add **WiFi logging or remote unlock capability**

---

# License

MIT License

This project is open for learning and experimentation.  
Feel free to modify and build on it.
