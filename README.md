# ESP8266 Dual-Mode Relay Demo (On-Premise vs Cloud)

This project demonstrates two control models using one ESP8266 + relay module:

- **On-Premise (Offline):** control relay from a personal laptop through USB serial commands (no internet required).
- **Cloud (Blynk IoT):** control relay from Blynk mobile app over Wi-Fi.

The firmware starts in **Cloud mode** by default and can be switched to **Offline mode** using batch scripts.

## Hardware Used

- ESP8266 NodeMCU (ESP8266MOD)
- 4-channel relay board
- AC bulb connected through one relay channel

## Relay Mapping

Based on board mapping:

- `D1 -> Relay 1` (used for demo bulb)
- `D4 -> Relay 2`
- `D5 -> Relay 3`
- `D6 -> Relay 4`

## Project Files

- `src/main.cpp` - dual-mode firmware
- `platformio.ini` - PlatformIO target + dependencies
- `scripts/mode_offline.bat` - switch firmware to offline mode
- `scripts/mode_cloud.bat` - switch firmware back to cloud mode
- `scripts/light_on.bat` - turn light ON via serial command
- `scripts/light_off.bat` - turn light OFF via serial command

## Configuration Before Upload

Open `src/main.cpp` and set:

- `WIFI_SSID`
- `WIFI_PASS`

Auth token is already configured in code for this demo.

## Build and Upload (PlatformIO)

From project root:

```powershell
pio run
pio run -t upload
pio device monitor -b 115200
```

## Blynk App Setup

1. Create a template/device in Blynk IoT.
2. Add one **Switch/Button** widget.
3. Set Datastream/Virtual Pin to **`V0`**.
4. Run device.
5. Toggle switch to control relay in cloud mode.

## Offline Control (Laptop, No Internet)

The `.bat` scripts send serial commands to ESP8266 over USB serial.
They auto-detect the correct serial port using:

- `pio device list --json-output`
- common USB-UART VID/PID filters (`CP210x`, `CH340`, `FTDI`)

### 1) Requirement

Make sure `pio` command is available in your terminal path.
Baud is already set to `115200` in scripts.

### 2) Use scripts

- Switch to offline mode:
  - `scripts\mode_offline.bat`
- Turn bulb ON:
  - `scripts\light_on.bat`
- Turn bulb OFF:
  - `scripts\light_off.bat`
- Return to cloud mode:
  - `scripts\mode_cloud.bat`

If relay scripts are used while device is in cloud mode, terminal shows:
- `Cloud mode active: local relay scripts are disabled. Run mode_offline.bat first.`

## Serial Commands Supported

You can also type these manually in serial monitor:

- `MODE OFFLINE`
- `MODE CLOUD`
- `RELAY ON`
- `RELAY OFF`
- `STATUS`
- `HELP`

## Demo Flow (College Presentation)

1. Power ESP8266 and connect relay wiring.
2. Show cloud mode:
   - internet ON
   - toggle Blynk switch (`V0`) to control bulb
3. Switch to on-premise mode:
   - run `mode_offline.bat`
   - disconnect internet
   - run `light_on.bat` / `light_off.bat`
4. Explain difference:
   - Cloud depends on internet + remote platform.
   - On-premise works locally via direct laptop-to-device command.

## Simple Web UI (No Chrome Serial Prompt)

Because browsers do not execute `.bat` files directly, use the local launcher:

1. Double-click `run_ui.bat`
2. Browser opens `http://127.0.0.1:8765`
3. Use UI buttons to run scripts through local server

This avoids Chrome opening `.bat` files as plain text.

## Safety Notes

- Be careful with AC mains wiring.
- Use proper insulation and avoid touching live terminals.
- Prefer demonstration with a protected setup under supervision.

