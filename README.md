# Golioth Example Upload Image

## Overview

This firmware uploads large files to an Amazon S3 bucket. It
demonstrates capturing an image with a camera and uploading using the
Golioth Firmware SDK for blockwise upload.

The application will connect to Golioth and start sending Hello log
messages every 5 seconds. After the fourth message a sample text file of
2246 bytes will be uploaded as a way for you to verify the data route is
configured correctly. The camera will then capture an image and upload
it.

## Supported Boards

- Nordic nrf9160dk

### Camera connections

This demo uses the [Arducam Mega
5MP-AF](https://www.arducam.com/product/presale-mega-5mp-color-rolling-shutter-camera-module-with-autofocus-lens-for-any-microcontroller/)
model.

| Function | nRF9160DK | Arducam Pin
| -------- | --------- | -----------
| VCC      | 5V        | 1 (red)
| GND      | GND       | 2 (black)
| CS       | P0.10     | 6 (orange)
| MOSI     | P0.11     | 5 (yellow)
| MISO     | P0.12     | 4 (brown)
| SCK      | P0.13     | 3 (white)

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

```
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

### NCS build commands

```
$ (.venv) west build -p -b nrf9160dk_nrf9160_ns app
$ (.venv) west flash
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
* Stream data using block upload
