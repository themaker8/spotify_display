# Project Play — Spotify Display

A physical Spotify controller built with an ESP32-C3 and a 2.8" ILI9341 TFT display. Shows the currently playing track, artist, a live progress bar, and time remaining. Three buttons let you skip, go back, and play/pause — without touching your phone.

---

## What it does

- Shows current track name and artist on a 320×240 colour display
- Live progress bar that updates every 2 seconds
- Time remaining counter (e.g. `-2:34`)
- PLAYING / PAUSED indicator
- Three physical buttons: previous, play/pause, skip
- Boot splash screen on startup
- Connects to Spotify over WiFi using the official Spotify Web API

---

## Hardware

### Bill of Materials

| Part | Description | Qty | Buy |
|---|---|---|---|
| ESP32-C3 Pro Mini | ESP32-C3 RISC-V, WiFi + BT5, USB-C, compact form factor | 1 | [robu.in](https://robu.in/product/pro-mini-wifi-bluetooth-development-board-pin/) |
| 2.8" ILI9341 TFT LCD | SPI display, 320×240 resolution, 65K colours | 1 | [robu.in](https://robu.in/product/2-8-inch-spi-screen-module-tft-interface-240-x-320-without-touch/) |
| 6×6×5mm Tactile Push Button Switch | For prev / play-pause / skip (pack of 10) | 1 pack | [robu.in](https://robu.in/product/6x6x5-tactile-push-button-switch/) |
| M3×4mm Brass Heat Set Insert | For securing the 3D printed case lid (pack of 25) | 1 pack | [robu.in](https://robu.in/product/m3-x-4-mm-brass-heat-set-knurl-threaded-round-insert-nut-25-pcs/) |
| Male-to-Female Jumper Wires 20cm | For connecting ESP32 to TFT and buttons (pack of 40) | 1 pack | [robu.in](https://robu.in/product/male-to-female-jumper-wires-40pcs-20cm/) |


---

### Wiring

#### TFT Display → ESP32-C3

| TFT Pin | ESP32-C3 GPIO |
|---|---|
| CS | GPIO 1 |
| RST | GPIO 2 |
| DC / A0 | GPIO 3 |
| SCK | GPIO 4 |
| SDA / MOSI | GPIO 5 |
| VCC | 3.3V |
| GND | GND |

#### Buttons → ESP32-C3

| Button | GPIO | Function |
|---|---|---|
| KEY-1 | GPIO 6 | Previous track |
| KEY-2 | GPIO 7 | Play / Pause |
| KEY-3 | GPIO 8 | Skip track |
| All GND pins | GND | Common ground |


---

## Software setup

### 1. Arduino IDE

Download from [arduino.cc/en/software](https://www.arduino.cc/en/software)

### 2. Install ESP32 board support

Follow [this guide](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/) to add the ESP32 package. When selecting your board in Arduino IDE, choose **ESP32C3 Dev Module**.

### 3. Install libraries

Open the Library Manager in Arduino IDE and install:

- `Adafruit_ILI9341`
- `Adafruit_GFX`
- `ArduinoJson`

Install **SpotifyEsp32** manually as a `.zip` from [github.com/FinianLandes/SpotifyEsp32](https://github.com/FinianLandes/SpotifyEsp32) via Sketch → Include Library → Add .ZIP Library.

### 5. Configure the code

Open the firmware file and fill in your credentials:

```cpp
char* SSID            = "YOUR_WIFI_NAME";
const char* PASSWORD  = "YOUR_WIFI_PASSWORD";
const char* CLIENT_ID     = "YOUR_SPOTIFY_CLIENT_ID";
const char* CLIENT_SECRET = "YOUR_SPOTIFY_CLIENT_SECRET";
```

Also add your name to the boot splash:

```cpp
tft.write("by YOUR NAME");
```


## Screen layout

```
┌────────────────────────────────────┐
│                                    │  y=0
│  Artist Name          (cyan)       │  y=20
│                                    │
│  Track Name           (white)      │  y=80
│                                    │
│                                    │
│  PLAYING              -2:34        │  y=180
│  [████████████░░░░░░░░░░░░░░░░░░] │  y=200
└────────────────────────────────────┘  y=240
              320px wide
```


---

## CAD / Case

The case is designed in Fusion 360. The lid is secured with 4× M3 heat set inserts pressed into the print using a soldering iron tip, then fastened with M3 screws. 


![CAD](screenshots/cad.png)



---

## Schematics

I have created a schematic for the project using kicad for better understanding of the circuit.

![circuit](screenshots/circuit.png)