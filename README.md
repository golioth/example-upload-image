# Golioth Example Upload Image

## Overview

This firmware uploads large files to an Amazon S3 bucket. It
demonstrates capturing an image with a camera and uploading using the
Golioth Firmware SDK for blockwise upload.

The application will connect to Golioth and await user input.

- Pressing button1 will capture and upload an image.
- Pressing button2 will upload a 2246 byte text file.
- Calling `capture_image` with the Golioth RPC service will remotely
  trigger image capture and upload.
- The Pipeline found in the pipelines folder is responsible for routing
  the image upload. By default it will be sent to an Amazon S3 bucket.
  Captured images are jpg format.

## Supported Boards

| Vendor    | Model                      | Zephyr name          |
| --------- | -------------------------- | -------------------- |
| Espressif | ESP32-DevkitC              | esp32_devkitc_wrover |
| Nordic    | nRF9160 DK                 | nrf9160dk_nrf9160_ns |
| NXP       | i.MX RT1024 Evaluation Kit | mimxrt1024_evk       |

### Camera connections

This demo uses the [Arducam Mega
5MP-AF](https://www.arducam.com/product/presale-mega-5mp-color-rolling-shutter-camera-module-with-autofocus-lens-for-any-microcontroller/)
model.

| Function | Arducam Pin | nRF9160DK |  esp32  |  mimxrt1024_evk |
| -------- | ----------- | --------- |  ------ |  -------------- |
| VCC      | 1 (red)     | VDD[^1]   |  3.3V   |  3.3V  (J20.08) |
| GND      | 2 (black)   | GND       |  GND    |  GND   (J20.12) |
| CS       | 6 (orange)  | P0.10     |  GPIO15 |  b1_13 (J18.08) |
| MOSI     | 5 (yellow)  | P0.11     |  GPIO13 |  b1_14 (J18.12) |
| MISO     | 4 (brown)   | P0.12     |  GPIO12 |  b1_15 (J18.10) |
| SCK      | 3 (white)   | P0.13     |  GPIO27 |  b1_12 (J18.06) |

[^1]: For the nRF9160DK, change the `VDD IO` switch (near the power
    switch) to `3V`.

### ESP32 Extra Setup

Connect two momentary push buttons as follows:

- Button1: GPIO26---Switch---GND
- Button2: GPIO27---Switch---GND

### NXP mimxrt1024_evk Extra Setup

The switch labelled `SW4` serves as Button 1. Connect one additional
momentary push button as follows:

- Button2: b1_10 (J18.2)---Switch---GND

## Data Route Setup

- Create an Amazon S3 bucket and generate a credential that allows
  upload to it.
- Use add the contents of the YAML file in the pipelines directory of
  this repository to your Golioth project.
- Use your S3 credential to set up the follow secrets in your Golioth
  project:
  - AWS_S3_ACCESS_KEY
  - AWS_S3_ACCESS_SECRET

You may follow the [Local Setup](#local-setup) guide to build your own
firmware, or download and use the precompiled binary from the latest
release. Note that you will need to [add Golioth PSK-ID/PSK to your
device](#configure-authentication-credential) before it can connect to
Golioth.

## Local Setup

> :important: Do not clone this repo using git. Zephyr's ``west`` meta
> tool should be used to set up your local workspace.

### Install the Python virtual environment

```
cd ~
mkdir example-upload-image
python -m venv example-upload-image/.venv
source example-upload-image/.venv/bin/activate
pip install wheel west
```

### Initialize and install

Run one of these two initializations based on your target board:

```
# Initalize for Zephyr
cd ~/example-upload-image
west init -m git@github.com:golioth/example-upload-image.git --mf west-zephyr.yml .

# Initalize for NCS (Nordic boards only)
cd ~/example-upload-image
west init -m git@github.com:golioth/example-upload-image.git --mf west-ncs.yml .

```

Fetch dependencies and configure the build environment

```
west update
west zephyr-export
pip install -r deps/zephyr/scripts/requirements.txt
```

## Building the application

Prior to building, update ``CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION`` in the
``prj.conf`` file to reflect the firmware version number you want to
assign to this build.

Then run the following commands to build and program the firmware based
on the device you are using.

### Zephyr build commands

Choose the build command for your board.

```
# ESP32
west build -p -b esp32_devkitc_wrover --sysbuild app
west flash

# mimxrt1024
west build -p -b mimxrt1024_evk --sysbuild app
west flash
```

### NCS build commands

```
west build -p -b nrf9160dk_nrf9160_ns app
west flash
```

### Configure Authentication credential

Configure PSK-ID and PSK using the device shell based on your Golioth
credentials and reboot:

```
   uart:~$ settings set golioth/psk-id <my-psk-id@my-project>
   uart:~$ settings set golioth/psk <my-psk>
   uart:~$ kernel reboot cold
```

## Features

This example implements the following Golioth services

* Runtime credentials
* Backend Logging
* Device Settings
* OTA Firmware Update
* Remote Procedure Call (RPC)
* Stream data using block upload

## Switch Between Zephyr and NCS

After initializing your local repository, you may switch between
building for Zephyr boards and building for NCS boards by using the
following commands:

### Switch to NCS Build

```
west config manifest.file west-ncs.yml
west update
```

### Switch to Zephyr Build

```
west config manifest.file west-zephyr.yml
west update
```
