# Project: Proactive Pitot Heat Controller for Experimental Aircraft
## Overview
This repository contains an Arduino-based automation system designed to manage pitot tube heating for non-FIKI (Flight Into Known Icing) experimental aircraft. By utilizing a high-precision BME280 sensor, the controller proactively detects atmospheric conditions conducive to icing and triggers an alert or activation signal to an avionics suite, such as the Garmin G3X.

## Key Features

**Proactive Icing Detection**: Uses the Magnus-Tetens formula to calculate dew point spread. Logic is tuned for early activation (7°C OAT / 5°C Spread) to ensure the pitot tube is heated before moisture entry.

**Aviation-Grade Reliability**: Implements a hardware watchdog timer (WDT) and software-based sensor health monitoring to prevent system hangs.

**Pilot Notification**: Features a 10-second "panic flash" sequence on the G3X CAS (Crew Alerting System) to explicitly notify the pilot of a sensor failure, prompting a manual override.

**Hardware Optimized**: Designed for I2C-based digital humidity/temperature/pressure probes with sintered metal caps for high-speed external mounting.

## System Logic
The controller monitors the "Temperature-Dewpoint Spread." When the air is near saturation in cold temperatures, the system pulls a discrete line high. This logic is specifically designed for IFR-certified, non-FIKI aircraft to maintain instrument integrity during inadvertent icing encounters or while transitioning through moist layers.

# Hardware Requirements

Microcontroller: Arduino-compatible (e.g., Nano Every, Trinket M0, or ESP32-C3).

Sensor: Bosch BME280 (I2C) inside a weather-proof sintered probe.

Avionics: Garmin G3X or any EFIS with configurable discrete inputs.
