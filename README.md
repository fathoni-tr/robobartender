# ROBOBARTENDER

*Serve Perfect Drinks Instantly. Efficiently. Every Time.*

![Platform](https://img.shields.io/badge/platform-Arduino%20%7C%20ESP32-lightgrey)
![Language](https://img.shields.io/badge/language-C%2B%2B-blue)
![License](https://img.shields.io/badge/license-MIT-green)

Automated beverage dispenser firmware for ESP32. Pick a flavor with a button, press Start, and the machine positions the glass, pours, and serves — driven by servos on a PCA9685 PWM driver, relays for the pumps, and a 16x2 I2C LCD for feedback.

---

## Table of Contents

- [Overview](#overview)
- [Hardware](#hardware)
  - [Pinout](#pinout)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
  - [Uploading](#uploading)
- [Usage](#usage)
- [How It Works](#how-it-works)
- [Tuning](#tuning)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

Robobartender is an open-source automation tool for robotic beverage dispensing. It coordinates three servo actions (glass transport, pour triggers) and three relay-driven pumps, with a button + LCD interface so no serial console or app is required.

### Why Robobartender?

- **Hardware control** — precise servo angles and relay timing for pouring.
- **On-device UI** — three flavor buttons, a Start button, and a 16x2 LCD.
- **Single-file firmware** — `bartender.ino`, no libraries beyond three well-known ones.
- **Smooth motion** — glass carriage moves step-by-step instead of snapping, so drinks don't spill.
- **Easy to extend** — add flavors by adding a button, a relay, and one `else if` branch.

---

## Hardware

| Component | Qty | Notes |
|---|---|---|
| ESP32 dev board | 1 | Any ESP32 with the pins below |
| PCA9685 16-ch PWM driver | 1 | I2C, default address `0x40` |
| Servo (glass carriage) | 1 | Channel 0 |
| Servo (pour trigger) | 3 | Channels 1–3, one per bottle |
| Relay module (3-ch) | 1 | Pumps / solenoids |
| Character LCD 16x2 + I2C backpack | 1 | Address `0x27` (or `0x3F`) |
| Push buttons | 5 | 3 flavor + 1 start + spare |

Wiring:

- **I2C bus** — LCD and PCA9685 share SDA/SCL (ESP32 default: GPIO21 = SDA, GPIO22 = SCL).
- **Buttons** — wired to GND, using the ESP32 internal pull-ups (`INPUT_PULLUP`), so a press reads `LOW`.
- **Relays** — active-LOW modules assumed: firmware writes `HIGH` to keep pumps off at boot.
- **Servo power** — feed the PCA9685 servo rail from a separate 5–6 V supply, **not** from the ESP32 5 V pin.

### Pinout

| Signal | ESP32 GPIO |
|---|---|
| Flavor 1 button ("Kelapa" / coconut) | 39 |
| Flavor 2 button ("Melon") | 37 |
| Flavor 3 button ("Jeruk" / orange) | 35 |
| Start button | 33 |
| Relay 1 (pump 1) | 6 |
| Relay 2 (pump 2) | 7 |
| Relay 3 (pump 3) | 8 |
| LCD + PCA9685 SDA | 21 |
| LCD + PCA9685 SCL | 22 |

> GPIO 34–39 are input-only on ESP32 — fine for buttons, not usable for relays. Keep that in mind if you remap pins.

---

## Getting Started

### Prerequisites

- [Arduino IDE](https://www.arduino.cc/en/software) 2.x, or `arduino-cli`
- ESP32 board support (`esp32` by Espressif)
- These libraries (Library Manager or `arduino-cli lib install`):

| Library | Purpose |
|---|---|
| `LiquidCrystal_I2C` (Frank de Brabander) | LCD over I2C |
| `Adafruit PWM Servo Driver Library` | PCA9685 |
| `Adafruit BusIO` | dependency, installed automatically |

`Wire` ships with the ESP32 core.

### Installation

```bash
git clone https://github.com/fathoni-tr/robobartender.git
cd robobartender
```

Open `bartender.ino` in the Arduino IDE — the folder name matches the sketch name, so no renaming needed.

With `arduino-cli`:

```bash
arduino-cli core install esp32:esp32
arduino-cli lib install "LiquidCrystal I2C" "Adafruit PWM Servo Driver Library"
```

### Uploading

1. Select **Tools → Board → ESP32 Dev Module** and your COM port.
2. Choose **115200** baud.
3. Click **Upload**.

---

## Usage

1. Power on. The LCD shows `Robot Bartender` / `Pilih Rasa...` (pick a flavor).
2. Press a flavor button — the LCD confirms, e.g. `Rasa: Kelapa` / `Tekan Start...`.
3. Press **Start**. The LCD shows `Tunggu Ya...` (please wait) and the sequence runs.
4. The pump runs for 10 seconds, then the machine returns to idle and the LCD shows `Selesai!` / `Nikmati minuman` (done / enjoy).

Changing flavor after pressing Start is ignored until the cycle finishes. The selection resets to none at the end of every cycle.

---

## How It Works

`mulaiProses()` runs this sequence:

1. Glass carriage (`SERVO_GELAS`, ch 0) moves `90° → 144°` smoothly.
2. Pour trigger 1 (`SERVO_PUTAR1`, ch 1) actuates `65° → 120° → 65°`.
3. Carriage moves `144° → 30°`.
4. Pour trigger 2 (`SERVO_PUTAR2`, ch 2) actuates `10° → 65° → 10°`.
5. Carriage returns `30° → 90°`.
6. Pour trigger 3 (`SERVO_PUTAR3`, ch 3) moves to `55°`.
7. The relay matching the selected flavor goes active (`LOW`) for **10 s**.
8. All relays return to `HIGH` (off); trigger 3 returns to `0°`.
9. LCD reports completion, then resets to the idle screen.

`smoothMove(channel, from, to, stepDelay)` interpolates one degree at a time — slow enough that liquid stays in the glass.

---

## Tuning

| Constant | Location | Effect |
|---|---|---|
| Servo angles | `setup()` and `mulaiProses()` | Glass positions and pour-trigger travel |
| `stepDelay` | `smoothMove(...)` calls | Carriage speed; raise it if the glass slips |
| `delay(10000)` | `mulaiProses()` | Pour duration in ms — **calibrate per bottle** |
| `setPWMFreq(50)` | `setup()` | Standard 50 Hz for analog servos |
| `angleToPulse()` mapping | `map(ang, 0, 180, 102, 512)` | PWM tick range for 0–180° |
| LCD address | `LiquidCrystal_I2C lcd(0x27, 16, 2)` | `0x27` or `0x3F` |
| PCA9685 address | `Adafruit_PWMServoDriver(0x40)` | Change only if you bridged the address pads |

Run each flavor once with a measuring cup before trusting the 10-second pour.

---

## Troubleshooting

| Symptom | Fix |
|---|---|
| LCD blank, backlight on | Wrong I2C address — try `0x3F`; check SDA/SCL aren't swapped |
| Nothing on the I2C bus | Run an I2C scanner sketch; verify 3.3 V logic on the PCA9685 VCC |
| Servos jitter or reset the ESP32 | Servo rail is drawing from the board — use a separate supply and share GND |
| Pump always on | Relay module is active-HIGH; invert the `digitalWrite` calls |
| Button does nothing | Not wired to GND, or re-check `INPUT_PULLUP` and the GPIO number |
| Wrong amount poured | Adjust `delay(10000)` or pump flow rate |

---

## Contributing

Issues and pull requests are welcome. See [CONTRIBUTING.md](CONTRIBUTING.md) and, for participation rules, [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md).

## License

MIT — see [LICENSE](LICENSE).
