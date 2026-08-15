# AutoWater

Automatic watering control algorithm for Arduino-based systems.

This repository contains C++ code implementing an automatic watering controller intended to run on Arduino-compatible boards. The controller reads soil moisture sensors and controls pumps/valves via MOSFETs or relays. It includes configurable thresholds, scheduling, and safety checks to avoid over-watering and to recover from sensor noise.

## Features

- Read soil moisture sensors (analog or digital)
- Control up to 4 pumps/valves via MOSFETs (low-side switching)
- Hysteresis and moving-average filtering to reduce sensor noise
- Minimum-on and minimum-off timing to protect pumps
- Optional scheduling support (time-based watering windows)
- Serial logging for debugging

## Hardware (Arduino UNO + 4 pumps)

This project was developed for an Arduino UNO controlling four pump motor switches (MOSFETs) from digital pins.

Typical hardware components:

- Arduino Uno
- Soil moisture sensor(s) (capacitive recommended)
- 4 x logic‑level N-channel MOSFETs or a 4‑channel MOSFET driver to switch pump power (low-side switching)
- 12V (or appropriate) DC pumps — powered from a separate supply sized for pump current
- Common ground between Arduino and pump power supply
- Optional flyback suppression (diode/RC/T VS) or snubber network across pumps
- Optional float sensor for tank low-level detection

IMPORTANT safety notes

- Do NOT drive pumps directly from Arduino pins. Use MOSFETs or relays and a separate power supply for pumps.
- Ensure the MOSFETs are logic-level (fully enhanced at 5V gate drive) and rated for the pump current and voltage.
- Provide flyback/EMI suppression for the pumps (RC snubber or TVS). Many DC motor pumps produce voltage spikes; add protection to avoid damaging the MOSFET or Arduino.
- Use fuses and proper wiring practices for mains-powered or high-current systems.

Example wiring (4 pumps):

- Soil moisture sensors: analog input pins (A0, A1, ...)
- Pump control (low-side MOSFET gate): digital pins D4, D5, D6, D7
- MOSFET source -> ground (common ground with Arduino)
- MOSFET drain -> negative pump terminal; pump positive terminal -> external power supply positive
- Optional float/level sensor: digital input (with pull-up/pull-down as required)
- Serial: USB to host for logs and configuration

## Example pin mapping

- pumpPins[0] = 4
- pumpPins[1] = 5
- pumpPins[2] = 6
- pumpPins[3] = 7
- moistureSensor = A0

## Building and Uploading

You can build and upload the code using the Arduino IDE or PlatformIO.

Arduino IDE:

1. Open the .ino or main .cpp file in the Arduino IDE.
2. Select `Arduino/Genuino Uno` as the board and the correct serial port.
3. Click Upload.

PlatformIO (VSCode):

1. Open the project in VSCode with PlatformIO installed.
2. Configure `platformio.ini` for `board = uno`.
3. `PlatformIO: Upload` to compile and flash.

## Configuration

Configure pins and algorithm parameters in the `config.h` or top of the main sketch:

- Sensor pins (e.g., `A0`)
- Pump control pins (e.g., `D4`..`D7`)
- Moisture thresholds (dry/wet)
- Hysteresis and smoothing window size
- Minimum pump on/off durations
- Optional schedule windows

Example parameter names and sample values you may find in the code:

- `MOISTURE_DRY_THRESHOLD = 600`
- `MOISTURE_WET_THRESHOLD = 400`
- `MIN_PUMP_ON_MS = 30000`  // 30 seconds
- `MIN_PUMP_OFF_MS = 300000` // 5 minutes

Adjust these values for your sensors and soil type.

## Minimal example sketch

```cpp
// Example: minimal sketch showing pin defs and a simple control loop
const int moisturePin = A0;
const int pumpPins[4] = {4, 5, 6, 7};
const int MOISTURE_DRY = 600;
const int MOISTURE_WET = 400;
const unsigned long MIN_ON_MS = 30000UL;
const unsigned long MIN_OFF_MS = 300000UL;

unsigned long pumpOnTime[4] = {0,0,0,0};
unsigned long pumpOffTime[4] = {0,0,0,0};
bool pumpState[4] = {false,false,false,false};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; ++i) pinMode(pumpPins[i], OUTPUT), digitalWrite(pumpPins[i], LOW);
}

void loop() {
  int reading = analogRead(moisturePin);
  Serial.print("Moisture: "); Serial.println(reading);

  unsigned long now = millis();
  bool needsWater = (reading >= MOISTURE_DRY);

  for (int i = 0; i < 4; ++i) {
    if (needsWater && !pumpState[i]) {
      // respect minimal OFF time
      if (now - pumpOffTime[i] >= MIN_OFF_MS) {
        digitalWrite(pumpPins[i], HIGH); // turn pump ON (depends on wiring)
        pumpState[i] = true;
        pumpOnTime[i] = now;
      }
    } else if (!needsWater && pumpState[i]) {
      // respect minimal ON time
      if (now - pumpOnTime[i] >= MIN_ON_MS) {
        digitalWrite(pumpPins[i], LOW); // turn pump OFF
        pumpState[i] = false;
        pumpOffTime[i] = now;
      }
    }
  }

  delay(1000);
}
```

This is a very small example to demonstrate pin mapping and timing constraints; adapt it to your full algorithm (filtering, hysteresis, averaging, scheduling).

## Algorithm Overview

Each cycle, the controller:

1. Reads the soil moisture sensor (optionally averaging several samples).
2. Applies hysteresis to decide whether the soil is dry or wet.
3. Enforces minimum on/off times before switching pump state to protect hardware.
4. Optionally respects a schedule (only water during configured windows).
5. Logs events over Serial for debugging and telemetry.

## Safety & Limitations

- Test with a dry run (disable pump driver) before connecting pumps to verify sensor behavior.
- Ensure pump wiring and MOSFET selection follow safety guidelines and the pump manufacturer's recommendations.
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
