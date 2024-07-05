# Golioth Example Upload Image

## Overview

This firmware uploads large files to an Amazon S3 bucket.

The application will connect to Golioth and start sending Hello log
messages every 5 seconds. After the fourth message a sample text file of
2246 bytes will be uploaded as a way for you to verify the data route is
configured correctly.

## Supported Boards

- Nordic nrf9160dk

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

## Supported Boards

| Vendor    | Model                      | Zephyr name          |
| --------- | -------------------------- | -------------------- |
| Espressif | ESP32-DevkitC              | esp32_devkitc_wrover |
| Nordic    | nRF9160 DK                 | nrf9160dk_nrf9160_ns |

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
west init -m git@github.com:golioth/example-template.git --mf west-zephyr.yml .

# Initalize for NCS (Nordic boards only)
cd ~/example-upload-image
west init -m git@github.com:golioth/example-template.git --mf west-ncs.yml .

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

```
west build -p -b <my_board_name> --sysbuild app
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
