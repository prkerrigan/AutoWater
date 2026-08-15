# AutoWater

Automatic watering control algorithm for Arduino-based systems.

This repository contains C++ code implementing an automatic watering controller intended to run on Arduino-compatible boards. The controller reads soil moisture sensors and controls pumps/valves via relays or MOSFETs. It includes configurable thresholds, scheduling, and safety checks to avoid over-watering and to recover from sensor noise.

## Features

- Read soil moisture sensors (analog or digital)
- Control one or more pumps/valves via relay or MOSFET
- Hysteresis and moving-average filtering to reduce sensor noise
- Minimum-on and minimum-off timing to protect pumps
- Optional scheduling support (time-based watering windows)
- Serial logging for debugging

## Hardware

Typical hardware components:

- Arduino Uno / Nano / Mega (or compatible)
- Soil moisture sensors (capacitive or resistive)
- Relay module or MOSFET driver to switch pump power
- 12V/5V pump(s) (depending on your system)
- Power supply sized for pump current
- Optional float sensor for tank low-level detection

Wiring (example):

- Soil moisture sensor(s): analog input pins (A0, A1, ...)
- Pump control: digital pins to relay/MOSFET (e.g. D4, D5)
- Float/level sensor: digital input (with pull-up/pull-down as required)
- Serial: USB to host for logs and configuration

IMPORTANT: Do not drive pumps directly from Arduino pins. Use a relay module or a MOSFET and provide a separate power supply for pumps with a common ground.

## Building and Uploading

You can build and upload the code using the Arduino IDE or PlatformIO.

Arduino IDE:

1. Open the .ino or main .cpp file in the Arduino IDE.
2. Select the correct board and serial port.
3. Click Upload.

PlatformIO (VSCode):

1. Open the project in VSCode with PlatformIO installed.
2. Configure `platformio.ini` for your board.
3. `PlatformIO: Upload` to compile and flash.

## Configuration

Configure pins and algorithm parameters in the `config.h` or top of the main sketch:

- Sensor pins (e.g., A0)
- Pump control pins (e.g., D4)
- Moisture thresholds (dry/wet)
- Hysteresis and smoothing window size
- Minimum pump on/off durations
- Optional schedule windows

Example parameters you may find in the code:

- MOISTURE_DRY_THRESHOLD = 600
- MOISTURE_WET_THRESHOLD = 400
- MIN_PUMP_ON_MS = 30_000  // 30 seconds
- MIN_PUMP_OFF_MS = 5 * 60_000 // 5 minutes

Adjust these values for your sensors and soil type.

## Algorithm Overview

Each cycle, the controller:

1. Reads the soil moisture sensor (optionally averaging several samples).
2. Applies hysteresis to decide whether the soil is dry or wet.
3. Enforces minimum on/off times before switching pump state to protect hardware.
4. Optionally respects a schedule (only water during configured windows).
5. Logs events over Serial for debugging and telemetry.

This approach avoids rapid cycling caused by noisy sensors and reduces pump wear.

## Safety & Limitations

- Test with a dry run (disable pump driver) before connecting pumps to verify sensor behavior.
- Ensure mains-powered pumps and relay wiring follow safety guidelines.
- Add fuses and current protection as appropriate for your pump.
- The code assumes reliable sensor readings; adding redundancy or float switch protection is recommended for production systems.

## Contributing

Contributions and improvements are welcome. Please open issues or PRs with:

- Hardware compatibility notes
- Improvements to the control algorithm
- Additional safety checks or telemetry

## License

This project is provided under the MIT License. See LICENSE (or add one) for details.

## Contact

Repository owner: prkerrigan

