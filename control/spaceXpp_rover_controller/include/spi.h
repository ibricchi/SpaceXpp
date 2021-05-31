#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_slave.h"

#ifndef SPI_H
#define SPI_H

#define VISION_MOSI_PIN (GPIO_NUM_4)
#define VISION_MISO_PIN (GPIO_NUM_15)
#define VISION_SCLK_PIN (GPIO_NUM_2)
#define VISION_CS_PIN (GPIO_NUM_14)
#define VISION_SPI_HOST SPI3_HOST
#define VISION_DMA_CHAN (2)

void vision_spi_init();

void vision_spi_task(void *arg);

#endif
