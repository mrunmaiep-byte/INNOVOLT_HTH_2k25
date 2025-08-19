# Fall Detection Wearable (TeamName_HTH_2k25)

## 📌 Problem Statement
Falls among elderly people often go unnoticed, leading to delayed medical attention. We aim to create a wearable fall detection device that detects falls and sends SMS alerts to caregivers, with cancel/SOS functionality and escalation.

## 🎯 Solution Approach
- ESP32 + MPU6050 IMU for fall detection.
- SIM900A GSM module for SMS alerts.
- Li-Po battery, XL6009 booster.
- User feedback via RGB LED, buzzer, and push button.
- Cancel window (10s) to avoid false alerts.
- Escalation to next caregiver if first does not respond.

## 🛠 Hardware Components
- ESP32 DevKit V1 (30-pin)
- MPU6050 (I²C IMU)
- SIM900A GSM Module
- Li-Po 3.7V Battery (1000–1500 mAh)
- XL6009 Boost Converter
- RGB LED + resistors, buzzer, button, slide switch
- Capacitors (1000µF, 100µF, 0.1µF, etc.)

## ⚡ Circuit Connections
- *ESP32 ↔ MPU6050*: SDA=GPIO21, SCL=GPIO22
- *ESP32 ↔ SIM900A*: TX=GPIO17 → RXD, RX=GPIO16 ← TXD
- *Button*: GPIO4 (INPUT_PULLUP)
- *LEDs*: R=GPIO25, G=GPIO26, B=GPIO27
- *Buzzer*: GPIO15
- Power regulated via XL6009 (4V)

## 🔄 Workflow
1. Normal mode → Green LED
2. Fall detected → White LED + buzzer countdown
3. Cancel (button press) → Blue LED
4. If not cancelled → Red LED + SMS alert
5. Caregiver reply “OK” → Reset
6. No reply in 60s → Escalation to next contact
7. Manual SOS (long press) → Immediate SMS alert

## 🚀 Demo Plan
- Simulate fall → observe LED/buzzer + SMS
- Show cancel button
- Show caregiver acknowledgement
- Show escalation
- Show manual SOS


