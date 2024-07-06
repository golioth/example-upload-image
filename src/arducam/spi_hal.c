/*
 * Copyright (c) 2024 Golioth, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "spi_hal.h"
#include <string.h>

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app_spi, LOG_LEVEL_DBG);

#define SPI_OP (SPI_HOLD_ON_CS | SPI_OP_MODE_MASTER | SPI_WORD_SET(8) | SPI_LINES_SINGLE)

static const struct spi_dt_spec arducam = SPI_DT_SPEC_GET(DT_NODELABEL(arducam), SPI_OP, 0);
static const struct gpio_dt_spec arducam_cs = SPI_CS_GPIOS_DT_SPEC_GET(DT_NODELABEL(arducam));

void spiBegin(void)
{
    gpio_pin_configure_dt(&arducam_cs, GPIO_OUTPUT_INACTIVE);
    return;
}

void spiCsOutputMode(int cs)
{
    ARG_UNUSED(cs);
    return;
}

void spiCsHigh(int cs)
{
    ARG_UNUSED(cs);
    gpio_pin_set_dt(&arducam_cs, 0);
}

void spiCsLow(int cs)
{
    ARG_UNUSED(cs);
    gpio_pin_set_dt(&arducam_cs, 1);
}

uint8_t spiReadWriteByte(uint8_t val)
{
    uint8_t write_reg[1] = {val};
    uint8_t read_reg[1] = {0};

    struct spi_buf my_write_buf = {.buf = write_reg, .len = 1};
    struct spi_buf my_read_buf = {.buf = read_reg, .len = 1};

    struct spi_buf_set tx_bufs = {&my_write_buf, 1};
    struct spi_buf_set rx_bufs = {&my_read_buf, 1};

    spi_transceive_dt(&arducam, &tx_bufs, &rx_bufs);
    return read_reg[0];
}
