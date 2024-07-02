#ifndef __ZEPHYRHAL_H
#define __ZEPHYRHAL_H

#include "spi_hal.h"

#define delayMs(ms) k_sleep(K_MSEC(ms))
#define delayUs(us) k_sleep(K_USEC(us))

#define arducamSpiBegin()        spiBegin()
#define arducamSpiTransfer(val)  spiReadWriteByte(val) //  SPI communication sends a byte
#define arducamSpiCsPinHigh(pin) spiCsHigh(pin)        // Set the CS pin of SPI to high level
#define arducamSpiCsPinLow(pin)  spiCsLow(pin)         // Set the CS pin of SPI to low level
#define arducamCsOutputMode(pin) spiCsOutputMode(pin)
#define arducamDelayMs(val)      delayMs(val) //  Delay Ms
#define arducamDelayUs(val)      delayUs(val) // Delay Us

#endif /*__ZEPHYRHAL_H*/
