# System Overview

## Functional Blocks
- **Input sensing:** IR sensor detects an object crossing the entry beam; LDR senses ambient light; BME280 measures temperature and humidity.
- **Processing:** ESP32 runs four FreeRTOS tasks on core 1.
- **Outputs:** OLED presents readings, GPIO 26 drives the demonstration LED, and USB Serial reports diagnostics.

## FreeRTOS Task Design

| Task | Priority | Trigger / period | Responsibility |
|---|---:|---|---|
| TaskIR | 3 (highest) | Binary semaphore from ISR | Debounces and counts confirmed IR events |
| TaskDisplay | 2 | Every 400 ms | Refreshes OLED; takes I2C mutex |
| TaskLDR | 1 | Every 1000 ms | Reads ADC and controls LED |
| TaskBME | 1 | Every 2000 ms | Reads temperature/humidity; takes I2C mutex |

## Synchronization
- `entrySemaphore`: ISR gives the binary semaphore using `xSemaphoreGiveFromISR()`; TaskIR blocks on `xSemaphoreTake(..., portMAX_DELAY)`.
- `i2cMutex`: protects the shared I2C bus while OLED and BME280 use it.
- Shared variables: `occupancyCount`, `globalTemp`, `globalHumid`, and `globalLight` carry sensor state to the display task.
- `vTaskDelay()`: provides periodic scheduling for display, LDR, and BME280 tasks.

## I2C Devices
| Device | Address |
|---|---|
| SSD1306 OLED | `0x3C` |
| BME280 | `0x76` |

The firmware initializes I2C with `Wire.begin(21, 22)` and sets the clock to 400 kHz.
