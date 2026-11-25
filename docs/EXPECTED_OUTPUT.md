# Expected Output and Test Checklist

## Serial Monitor
Set Serial Monitor to **115200 baud**. Typical messages:

```text
=== SYSTEM BOOT: HARDENED 1-SENSOR DEBOUNCE ===
[OLED] Driver Connected Successfully.
[BME280] Sensor Connected Successfully.
=== HARDWARE SETUP PROTOCOL COMPLETE ===
[LDR TASK] Light Level Shift Detected: 2450
[LIGHT CONTROL] Room is DARK. Turning room light ON.
[SENSOR EVENT] Object Counted! Total: 1
[LDR TASK] Light Level Shift Detected: 900
[LIGHT CONTROL] Room is BRIGHT. Turning room light OFF.
[BME TASK HEARTBEAT] Temp: 27.4C | Humid: 58.2%
```

Values are illustrative and vary with the environment and hardware.

## OLED Fields
- Total occupancy event count
- Temperature in °C
- Relative humidity in %
- Raw LDR ADC reading

## Functional Test Checklist
- [ ] OLED initializes at I2C address 0x3C.
- [ ] BME280 initializes at I2C address 0x76.
- [ ] Passing an object across the IR beam increments the count after debounce.
- [ ] Covering/uncovering the LDR changes the ADC reading.
- [ ] With the supplied divider orientation, ADC below 2000 turns the LED OFF.
- [ ] ADC 2000 or above turns the LED ON.
- [ ] Temperature and humidity update approximately every 2 seconds.
- [ ] BME heartbeat appears approximately every 10 seconds.
