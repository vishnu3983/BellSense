# BellSense 🔔

BellSense is a smart doorbell detector built using an **ESP8266 D1 Mini**, **ZMPT101B AC voltage sensor**, and **ESPHome**, with **Home Assistant** providing the automation, history, notifications, and other integrations.

The project is designed to work with a conventional AC doorbell circuit and detect a doorbell press by monitoring the AC waveform rather than directly connecting the ESP8266 to the doorbell circuit.

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

> **Current status:** The basic detector is working and has been deployed in a real home environment. False-positive detection caused by electrical switching transients is currently being investigated and the detection algorithm is being improved.

---

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
│           │ Wi-Fi / ESPHome API                          │
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
