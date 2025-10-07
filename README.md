# Arduino Temperature Control System (C245 Soldering Configuration)

## Overview
This Arduino-based temperature control system is designed for precise heater management using a thermocouple (MAX6675), a potentiometer for temperature setpoint control, an LCD display for live feedback, and a MOSFET for heater actuation.  
The setup is ideal for **C245 soldering configurations** or similar high-precision thermal systems.

---

## Features
- **Closed-loop temperature control** with proportional–integral feedback  
- **PWM-based heater control** (up to 60% duty cycle for safety)  
- **Smooth 3-second ramp** when changing temperature setpoints (prevents thermal shock)  
- **Accurate temperature measurement** by periodically disabling PWM  
  - Every 1 second, PWM turns **off for 100 ms**
  - Includes buffer periods before and after measurement for stability  
- **Automatic heater cutoff** when target temperature is reached  
- **LCD display** shows both current and target temperatures  
- **NAN value filtering** to prevent invalid readings from causing control errors

---

## Hardware Requirements
- **Arduino Uno / Nano**
- **MAX6675 thermocouple amplifier**
- **C245 iron tip**
- **MOSFET driver circuit (high-side switching recommended)**
- **LCD I2C display (16x2)**
- **10k potentiometer**

 ---
## Hardware Connections

| Component | Pin (LGT8F) | Notes |
|------------|--------------|-------|
| LCD SDA | A4 | I²C Data |
| LCD SCL | A5 | I²C Clock |
| MAX6675 SCK | D13 | SPI Clock |
| MAX6675 CS | D10 | SPI Chip Select |
| MAX6675 SO | D12 | SPI Data |
| Potentiometer | A0 | Temperature Set |
| MOSFET Gate | D9 | PWM output |
| 12–24 V DC | VIN / External | Heater power |
| Thermocouple | MAX6675 input | From handle tip |

---

## Important Note on High-Side Switching
High-side switching is **necessary** to properly isolate the heater element during temperature measurement.  
If the MOSFET is placed on the **low side**, the heater may remain **partially energized** due to current paths through the **MAX6675 module**.  
This results in:
- Incorrect or noisy temperature readings  
- The element staying perpetually on, even when PWM is off  

By using **high-side switching**, the heater is **completely disconnected** during measurement, ensuring accurate temperature sensing and preventing unwanted heating.

---

## C245 Configuration Reference
This design is specifically tuned for **JBC C245 soldering cartridge control**, similar to professional soldering stations:
- The heater operates from a **24V DC supply, I used 12V be cause i have one laying around**
- The **embedded thermocouple** provides temperature feedback via the MAX6675
- The **MOSFET (high-side)** allows precise PWM control and isolation  
- The **controller maintains target temperature** automatically via a proportional–integral loop  

Refer to the following image for the recommended wiring and setup:

![C245 Configuration Diagram](images.png)

---

## Safety Notes
- Maximum PWM duty cycle is **limited to 60%** for safety.
- Ensure proper electrical isolation between heater and signal electronics.
- Use a heatsink for the MOSFET if continuous heating is expected.
- Double-check thermocouple polarity before operation — reversed wiring causes incorrect readings.

