# BellSense 🔔

BellSense is a smart doorbell detector built using an **ESP8266 D1 Mini**, **ZMPT101B AC voltage sensor**, and **ESPHome**, with **Home Assistant** providing automation, history, notifications, and other integrations.

The project is designed to work with a conventional AC doorbell circuit and detect a doorbell press by monitoring the AC waveform rather than directly connecting the ESP8266 to the doorbell circuit.

> **Status:** Experimental / v0.x  
> The basic detector is working and has been deployed in a real home environment. False-positive detection caused by electrical switching transients is currently being investigated and the detection algorithm is being improved.

## Features

- Detect conventional AC doorbell presses
- ESP8266 D1 Mini based
- ZMPT101B AC voltage sensor
- ESPHome-based firmware
- Custom ESPHome external component written in C++/Python
- Home Assistant binary sensor
- Doorbell event history
- Daily doorbell press counter
- Mobile push notifications
- Can be extended to Alexa announcements and other Home Assistant automations
- Configurable sampling and detection parameters

## Project Architecture

BellSense consists of three main parts:

```text
┌─────────────────────────────────────────────────────────┐
│                       BellSense                         │
│                                                         │
│  ┌─────────────────┐                                    │
│  │   Hardware      │                                    │
│  │                 │                                    │
│  │  D1 Mini        │                                    │
│  │  ZMPT101B       │                                    │
│  │  5V SMPS        │                                    │
│  └────────┬────────┘                                    │
│           │                                             │
│           ▼                                             │
│  ┌─────────────────┐                                    │
│  │     ESPHome     │                                    │
│  │                 │                                    │
│  │ BellSense.yaml  │                                    │
│  │       +         │                                    │
│  │ Custom Doorbell │                                    │
│  │    Component    │                                    │
│  └────────┬────────┘                                    │
│           │ Wi-Fi / ESPHome API                         │
│           ▼                                             │
│  ┌─────────────────┐                                    │
│  │ Home Assistant  │                                    │
│  │                 │                                    │
│  │ Binary Sensor   │                                    │
│  │ History         │                                    │
│  │ Counter         │                                    │
│  │ Notifications   │                                    │
│  │ Alexa / etc.    │                                    │
│  └─────────────────┘                                    │
└─────────────────────────────────────────────────────────┘
```

### ESPHome project

The main ESPHome configuration is:

```text
BellSense.yaml
```

### Custom ESPHome component

The doorbell detection logic is implemented as a custom ESPHome external component:

```text
components/
└── doorbell/
    ├── __init__.py
    ├── binary_sensor.py
    ├── doorbell.cpp
    └── doorbell.h
```

The component implements the actual signal sampling and doorbell detection logic.

This means BellSense is **not just an ESPHome component**. It is a complete ESPHome project that contains a reusable custom `doorbell` component.

## Hardware

### Required Hardware

| Component | Purpose |
|---|---|
| ESP8266 D1 Mini | Main controller |
| ZMPT101B | AC voltage sensing |
| 230V AC → 5V DC SMPS | Power supply |
| Suitable enclosure | Permanent installation |

A NodeMCU ESP8266 was used during the initial development, but the final implementation uses a D1 Mini because of its smaller size.

## Hardware Connection

### D1 Mini ↔ ZMPT101B

The ZMPT101B is used to monitor the AC voltage associated with the doorbell.

```text
             ZMPT101B
          ┌─────────────┐
          │             │
          │    VCC  ────┼──── 3.3V
          │    GND  ────┼──── GND
          │    OUT  ────┼──── A0
          │             │
          └─────────────┘
                         │
                         │
                    ┌────┴────┐
                    │ D1 Mini │
                    │         │
                    │      A0 │
                    │     3V3 │
                    │     GND │
                    └─────────┘
```

### Power Supply

The D1 Mini is powered using a 230V AC → 5V DC SMPS.

```text
             230V AC
                │
                ▼
        ┌────────────────┐
        │   230V → 5V    │
        │      SMPS      │
        └───────┬────────┘
                │
           ┌────┴────┐
           │         │
          +5V       GND
           │         │
           ▼         ▼
        D1 Mini   D1 Mini
          5V        GND
```

The ZMPT101B sensor is powered from the D1 Mini's **3.3V** supply in the current implementation.

### Complete connection

```text
                         230V AC
                            │
                 ┌──────────┴──────────┐
                 │                     │
                 ▼                     ▼
          ┌─────────────┐       Doorbell circuit
          │  230V → 5V  │              │
          │    SMPS     │              │
          └──────┬──────┘              │
                 │                     │
              +5V/GND                   │
                 │                     │
                 ▼                     ▼
          ┌─────────────┐       ┌─────────────┐
          │  D1 Mini    │       │  ZMPT101B   │
          │             │       │             │
          │  5V ◄───────┤       │ VCC ◄── 3.3V│
          │  GND ◄──────┼───────┤ GND         │
          │             │       │             │
          │  A0 ◄───────┼───────┤ OUT         │
          │             │       │             │
          └──────┬──────┘       └─────────────┘
                 │
                Wi-Fi
                 │
                 ▼
          Home Assistant
```

> ⚠️ **Mains safety:** The ZMPT101B and power supply are connected to mains-voltage circuitry. Incorrect wiring can result in electric shock, fire, or death. Use appropriately rated isolated power supplies, insulation, enclosure, fusing, strain relief, and proper electrical installation practices. Do not work on live mains wiring.

## Detection Method

The ZMPT101B provides an analog representation of the AC waveform to the ESP8266 ADC.

BellSense periodically samples the ADC and examines the signal over a short time window.

The current detection approach is approximately:

```text
ZMPT101B
    │
    ▼
AC waveform
    │
    ▼
ESP8266 ADC (A0)
    │
    ▼
Sample every ~5 ms
    │
    ▼
Evaluate ~100 ms window
    │
    ▼
Determine signal swing
    │
    ▼
Threshold / validation
    │
    ▼
Doorbell event
```

The project originally experimented with RMS voltage measurement. This was eventually simplified because the application does not require accurate mains voltage measurement.

The goal is simply to determine:

> **Is the characteristic AC signal associated with a doorbell press present?**

## ESPHome Configuration

A minimal configuration looks like:

```yaml
esphome:
  name: bellsense

esp8266:
  board: d1_mini

external_components:
  - source:
      type: local
      path: components

binary_sensor:
  - platform: doorbell
    name: "Doorbell"
```

The exact configuration options depend on the current version of the custom component.

The component is loaded from:

```text
components/
```

using ESPHome's `external_components` mechanism.

## Home Assistant Integration

BellSense exposes the doorbell as a normal Home Assistant binary sensor.

For example:

```text
binary_sensor.doorbell
```

The normal state sequence for a press is:

```text
OFF
 │
 │ Doorbell pressed
 ▼
ON
 │
 │ Doorbell released
 ▼
OFF
```

This makes the entity suitable for Home Assistant automations.

### Doorbell History

Because the doorbell is exposed as a binary sensor, Home Assistant can record its state changes in the History and Logbook.

This provides a simple record of when the doorbell was pressed.

Example:

```text
Today

09:32  Doorbell
11:47  Doorbell
14:16  Doorbell
18:52  Doorbell
```

### Daily Doorbell Counter

The number of doorbell presses is handled by Home Assistant rather than by the ESP8266.

A Home Assistant **Counter helper** can be used to maintain the number of presses during the current day.

Example automation:

```yaml
alias: Doorbell - Increment Counter
trigger:
  - platform: state
    entity_id: binary_sensor.doorbell
    to: "on"

action:
  - service: counter.increment
    target:
      entity_id: counter.doorbell_today
```

The counter can then be reset once per day using another Home Assistant automation.

This approach keeps long-term state and statistics in Home Assistant instead of storing them in the ESP8266.

### Mobile Notifications

The doorbell event can also trigger a notification on a phone running the Home Assistant Companion App.

Example:

```yaml
alias: Doorbell - Phone Notification
trigger:
  - platform: state
    entity_id: binary_sensor.doorbell
    to: "on"

action:
  - service: notify.mobile_app_your_phone
    data:
      title: "🔔 Doorbell"
      message: "Someone is at the door!"
```

Replace `notify.mobile_app_your_phone` with the notification service corresponding to your phone.

### Alexa Integration

Because the doorbell event is available in Home Assistant, it can also be used to trigger an announcement on an Alexa/Echo device.

For example:

```text
Doorbell pressed
       │
       ├──────────► Phone notification
       │
       ├──────────► Doorbell history
       │
       ├──────────► Daily counter
       │
       └──────────► Alexa announcement
```

Alexa integration is intentionally handled on the Home Assistant side rather than by the ESP8266.

This keeps the BellSense firmware independent of any particular smart speaker platform.

## Repository Structure

The current repository structure is intentionally simple:

```text
BellSense/
│
├── BellSense.yaml
├── secrets.yaml
│
└── components/
    └── doorbell/
        ├── __init__.py
        ├── binary_sensor.py
        ├── doorbell.cpp
        └── doorbell.h
```

### BellSense.yaml

Main ESPHome configuration for the device.

### components/doorbell/

Custom ESPHome external component containing:

- `__init__.py` — component registration/configuration
- `binary_sensor.py` — ESPHome binary sensor configuration/code generation
- `doorbell.h` — C++ class definition
- `doorbell.cpp` — doorbell detection implementation

### secrets.yaml

Contains local/private ESPHome configuration such as Wi-Fi credentials.

This file should **not** be committed to GitHub.

## Installation

Clone the repository:

```bash
git clone https://github.com/<your-username>/BellSense.git
cd BellSense
```

Make sure ESPHome is installed:

```bash
esphome version
```

Compile the firmware:

```bash
esphome compile BellSense.yaml
```

Upload it:

```bash
esphome upload BellSense.yaml
```

View logs:

```bash
esphome logs BellSense.yaml
```

## Development History

BellSense was developed incrementally.

### Stage 1 — RMS measurement

The initial idea was to measure the RMS voltage from the ZMPT101B.

This was found to be unnecessary for the application.

### Stage 2 — Instantaneous AC detection

The project was simplified to detect the presence of the AC waveform rather than calculate its RMS voltage.

### Stage 3 — Windowed sampling

The detector samples the ADC periodically and evaluates the minimum/maximum signal swing over a short window.

### Stage 4 — ESPHome external component

The initial ESPHome custom component approach was replaced with an external component after ESPHome removed support for the old `custom_component` mechanism.

### Stage 5 — Home Assistant integration

Home Assistant was used for:

- Event history
- Notifications
- Daily press counting
- Automation
- Future smart-speaker integration

### Stage 6 — Permanent installation

The prototype was moved from a NodeMCU to a D1 Mini and installed as a permanent doorbell detector.

## Current Limitations

The current detection algorithm is based on detecting changes in the AC waveform.

This can make the detector susceptible to **electrical transients**.

In particular, switching other loads in the house on or off can occasionally produce a signal that resembles a doorbell press.

False detections have been observed more frequently during periods of higher household electrical activity.

This is an active area of development.

The next improvement will focus on distinguishing:

```text
Actual doorbell waveform
        vs.
Electrical switching transient
```

rather than simply increasing the detection threshold.

## Roadmap

- [x] ESP8266 prototype
- [x] ZMPT101B AC signal detection
- [x] Windowed ADC sampling
- [x] Custom ESPHome external component
- [x] D1 Mini implementation
- [x] Home Assistant integration
- [x] Doorbell event history
- [x] Daily press counter
- [x] Mobile notifications
- [ ] Improve transient rejection
- [ ] Reduce false-positive detections
- [ ] Improve detection reliability
- [ ] Add configurable detection parameters
- [ ] Add optional diagnostic values
- [ ] Document ZMPT101B calibration
- [ ] Add Alexa integration example
- [ ] Release v1.0

## Contributing

Issues, suggestions, and improvements are welcome.

In particular, feedback regarding:

- Different doorbell circuits
- ZMPT101B configurations
- False-positive detection
- False-negative detection
- ESP8266 compatibility
- Home Assistant integrations

would be useful for improving the project.

## License

This project is released under the [MIT License](LICENSE).
