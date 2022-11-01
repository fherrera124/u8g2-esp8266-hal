#pragma once
#include "driver/gpio.h"
#include "u8g2.h"

typedef struct {
    gpio_num_t clk;
    gpio_num_t mosi;
    gpio_num_t sda; // data for I²C
    gpio_num_t scl; // clock for I²C
    gpio_num_t cs;
    gpio_num_t reset;
    gpio_num_t dc;
} u8g2_esp8266_hal_t;

void u8g2_esp8266_hal_init(u8g2_esp8266_hal_t u8g2_esp8266_hal_param);

uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);

uint8_t u8g2_esp8266_spi_byte_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int, void* arg_ptr);
