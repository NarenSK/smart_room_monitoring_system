# Pin Connections

This document is kept separate from the firmware so the circuit diagram can be added independently.

## ESP32 Pin Mapping

| Component | Component pin / signal | ESP32 connection | Notes |
|---|---|---|---|
| IR sensor | VCC | 3.3 V (or 5 V only if supported) | Verify module rating and ESP32-safe OUT level |
| IR sensor | GND | GND | Common ground |
| IR sensor | OUT | GPIO 14 | Active LOW; INPUT_PULLUP; FALLING interrupt |
| LDR | One end | 3.3 V | Voltage-divider leg |
| LDR | Other end | GPIO 34 junction | ADC1 input; junction also connects to fixed resistor |
| Fixed resistor | ~10 kΩ | GPIO 34 junction to GND | Other voltage-divider leg |
| LED | Anode through ~220 Ω resistor | GPIO 26 | Digital output |
| LED | Cathode | GND | Common ground |
| OLED SSD1306 | VCC / GND | 3.3 V / GND | Verify breakout supply requirements |
| OLED SSD1306 | SDA / SCL | GPIO 21 / GPIO 22 | I2C address 0x3C |
| BME280 | VIN/VCC / GND | 3.3 V / GND | Verify breakout supply requirements |
| BME280 | SDA / SCL | GPIO 21 / GPIO 22 | Shared I2C bus; address 0x76 |

## Pin Summary

| ESP32 pin | Function | Firmware configuration |
|---|---|---|
| GPIO 14 | IR sensor output | INPUT_PULLUP, falling-edge interrupt |
| GPIO 34 | LDR analog input | ADC1, 12-bit reading (0–4095) |
| GPIO 26 | Room LED output | HIGH = ON, LOW = OFF |
| GPIO 21 | I2C SDA | Wire.begin(21, 22) |
| GPIO 22 | I2C SCL | I2C clock set to 400 kHz |
| USB serial | Debug output | 115200 baud |

## Important Wiring Notes
1. Connect all module grounds to ESP32 GND.
2. OLED and BME280 share SDA/SCL but use different I2C addresses.
3. GPIO 34 is input-only and is used as the LDR ADC input.
4. Divider orientation affects whether ADC readings rise or fall with brightness. The supplied firmware assumes readings below 2000 indicate a bright environment.
5. Never connect a 5 V logic output directly to an ESP32 GPIO. Check the IR module OUT voltage and use level shifting if needed.
6. Use the LED's series resistor; do not connect a bare LED directly to GPIO 26.
7. Resistor values are starting points; tune for the actual components.

## Circuit Diagram
## Circuit Diagram

The hardware implementation of this project was physically assembled and tested offline. The following Cirkit Designer project provides an online representation of the circuit for reference and visualization.

- **Interactive Circuit:** [View Smart Room Monitoring System](https://app.cirkitdesigner.com/project/fbca92e0-41f3-4058-bb56-b1095acb58e9)
- **Circuit Image:** See `<img width="1049" height="965" alt="smart-room-monitoring-system" src="https://github.com/user-attachments/assets/999a8ab6-fdc2-4b80-b02a-6378ca36d3d7" />
`

The design includes an ESP32, IR sensor, LDR voltage divider, SSD1306 OLED display, BME280 environmental sensor, and automatic lighting LED.
