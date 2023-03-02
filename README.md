# XY Control

## Firmware for the T5 Subsystem XY Controller RPi Pico board

### Installation

This project is built using the [PlatformIO](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) extension for [VSCode](https://code.visualstudio.com/).

If using `framework = arduino` in the `platformio.ini` configuration, copy the files from `arduino-patch` into your Arduino installation.

### Building & Flashing

Change the `upload_port` fields in `platformio.ini` to the Serial ports the Raspberry Pi Picos connect to on your machine.

In a PlatformIO CLI shell:

```sh
$ pio run -t upload -e t5|arm-mock
```

### Usage

#### Connections

| Arm-Mock Pico | T5 Pico | T6 RPi |
| - | - | - |
| **4** (I2C0_SDA) | **4** (I2C0_SDA) | -- |
| **5** (I2C0_SCL) | **5** (I2C0_SCL) | -- |
| -- | **2** (I2C1_SDA) | **2** (I2C1_SDA) |
| -- | **3** (I2C1_SCL) | **3** (I2C1_SCL) |
| -- | **28** (INT_PIN) | **21** (INT_PIN) |

#### Serial Monitors

In a PlatformIO CLI shell:

```sh
$ pio device monitor -p {PORT_NAME}
```
