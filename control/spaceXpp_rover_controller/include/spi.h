/*
    Written by Nicholas Pfaff (nicholas.pfaff19@imperial.ac.uk), 2021 - SpaceX++ EEE/EIE 2nd year group project, Imperial College London
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_slave.h"

#include "drive.h"
#include "uart.h"
#include "mqtt.h"

#ifndef SPI_H
#define SPI_H

#define VISION_MOSI_PIN (GPIO_NUM_4)
#define VISION_MISO_PIN (GPIO_NUM_15)
#define VISION_SCLK_PIN (GPIO_NUM_2)
#define VISION_CS_PIN (GPIO_NUM_14)
#define VISION_SPI_HOST SPI3_HOST
#define VISION_DMA_CHAN (2)
#define VISION_BUFFER_SIZE (256)

void vision_spi_init();

void vision_spi_task(void *arg);

/* 
    Simulates vision communication when used instead of "vision_spi_task".
    Used for testing without the vision component being connected.
*/
void vision_spi_task_simulated(void *arg);

void handle_vision_stop_instruction(char* stopInformation);

#endif
