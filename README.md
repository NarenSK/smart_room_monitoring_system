# Smart Room Monitoring & Automatic Lighting System

An ESP32-based smart-room prototype combining IR event detection, LDR-based automatic lighting, environmental monitoring, and an OLED dashboard. The firmware uses four FreeRTOS tasks, a binary semaphore, and an I2C mutex.

## Features
- IR sensor interrupt with a 400 ms debounce window and event counting.
- LDR-based ambient-light sensing and automatic LED control.
- BME280 temperature and humidity monitoring.
- SSD1306 128×64 OLED dashboard.
- Four FreeRTOS tasks pinned to ESP32 core 1.
- Serial diagnostics at 115200 baud.

## Hardware
ESP32 development board, IR sensor module, LDR and ~10 kΩ resistor, BME280, SSD1306 OLED, LED and ~220 Ω resistor, jumper wires.

**Wiring:** See [Pin Connections](docs/PIN_CONNECTIONS.md). The circuit diagram can be added later under `diagrams/`.

## Pin Summary
| Function | ESP32 pin |
|---|---:|
| IR sensor OUT | GPIO 14 |
| LDR divider output | GPIO 34 (ADC1) |
| LED | GPIO 26 |
| I2C SDA | GPIO 21 |
| I2C SCL | GPIO 22 |

OLED address: `0x3C`; BME280 address: `0x76`.

## FreeRTOS Task Design
| Task | Priority | Period / trigger | Function |
|---|---:|---|---|
| TaskIR | 3 | Binary semaphore from ISR | Debounce and count IR events |
| TaskDisplay | 2 | 400 ms | Refresh OLED |
| TaskLDR | 1 | 1000 ms | Read LDR and control LED |
| TaskBME | 1 | 2000 ms | Read temperature and humidity |

The I2C mutex protects the OLED and BME280 from concurrent bus access. See [System Overview](docs/SYSTEM_OVERVIEW.md).

## Software Requirements
- Arduino IDE with ESP32 board support
- Adafruit GFX
- Adafruit SSD1306
- Adafruit BME280
- Adafruit Unified Sensor (if required by library installation)

## Build and Run
1. Install the ESP32 board package and required libraries.
2. Wire the components using [docs/PIN_CONNECTIONS.md](docs/PIN_CONNECTIONS.md).
3. Open `src/SmartRoomMonitoring.ino`.
4. Select your ESP32 board and port, then upload.
5. Open Serial Monitor at **115200 baud**.
6. Test the IR sensor, vary light on the LDR, and observe the OLED and LED.

See [Expected Output & Test Checklist](docs/EXPECTED_OUTPUT.md).

## Repository Structure
```
.
├── src/SmartRoomMonitoring.ino
├── docs/
│   ├── PIN_CONNECTIONS.md
│   ├── SYSTEM_OVERVIEW.md
│   └── EXPECTED_OUTPUT.md
├── diagrams/README.md
└── README.md
```

## Limitations and Safety
- A single IR sensor counts detected crossings; it cannot distinguish entry from exit, so this is not bidirectional people counting.
- The count is volatile and resets after restart.
- The LDR threshold (2000) is a starting value and should be calibrated for the actual divider and lighting conditions.
- The LED is a low-voltage demonstration output. Do not connect mains lighting directly; mains control requires appropriately rated, isolated hardware and safe electrical practices.
- Verify each module's supply rating. Never apply a 5 V logic output directly to an ESP32 GPIO; use level shifting if needed. Use a series resistor with the LED.

## Contributors
- Naren S K
- Manjuprasad T.K
