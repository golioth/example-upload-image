/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __SPI_HAL_H
#define __SPI_HAL_H

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/spi.h>

void spiBegin(void);
void spiCsOutputMode(int cs);
void spiCsHigh(int cs);
void spiCsLow(int cs);
uint8_t spiReadWriteByte(uint8_t val);

#endif /*__SPI_HAL_H*/
