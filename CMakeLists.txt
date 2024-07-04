# Copyright (c) 2022-2023 Golioth, Inc.
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Conditionally add NCS/Zephyr-specific config
set(NCS_BOARD_LIST "nrf9160dk_nrf9160_ns")

if (BOARD IN_LIST NCS_BOARD_LIST)
    list(APPEND EXTRA_CONF_FILE prj-ncs.conf)
else()
    list(APPEND EXTRA_CONF_FILE prj-zephyr.conf)
endif(BOARD IN_LIST NCS_BOARD_LIST)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(example_upload_image)

target_sources(app PRIVATE src/main.c)
