#include "u8g2_esp8266_hal.h"

#include <stdint.h>
#include <string.h>

#include "driver/gpio.h"
#include "driver/spi.h"
#include "esp8266/spi_struct.h"
#include "freertos/portmacro.h"
#include "rom/ets_sys.h"

#define SPI_CUSTOM_INTERFACE 0x43; // spi_interface_t bit fields: mosi_en, byte_tx_order, cpha, cpol

static u8g2_esp8266_hal_t u8g2_esp8266_hal; // HAL state data.

static gpio_config_t gpio_c;

static inline void init_config()
{
    // Init spi configuration
    spi_config_t spi_config;
    memset(&spi_config, 0, sizeof(spi_config));
    spi_config.interface.val = SPI_CUSTOM_INTERFACE;
    spi_config.intr_enable.val = SPI_MASTER_DEFAULT_INTR_ENABLE;
    spi_config.mode = SPI_MASTER_MODE;
    spi_config.clk_div = SPI_2MHz_DIV;
    spi_config.event_cb = NULL;

    spi_init(HSPI_HOST, &spi_config);
}

/**
 * @brief Initialize the ESP8266 HAL.
 */
void u8g2_esp8266_hal_init(u8g2_esp8266_hal_t u8g2_esp8266_hal_param)
{
    u8g2_esp8266_hal = u8g2_esp8266_hal_param;

    gpio_c.intr_type = GPIO_INTR_DISABLE;
    gpio_c.mode = GPIO_MODE_OUTPUT;
    gpio_c.pin_bit_mask = (1ULL << u8g2_esp8266_hal.mosi) | (1ULL << u8g2_esp8266_hal.clk);
    gpio_c.pull_down_en = 0;
    gpio_c.pull_up_en = 0;
}

/**
 * @brief HAL callback function as prescribed by the U8G2 library.  This callback is
 * invoked to handle callbacks for GPIO and delay functions.
 */
uint8_t u8g2_esp8266_gpio_and_delay_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int,
    void* arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase
        gpio_config(&gpio_c);
        break;

    case U8X8_MSG_DELAY_MILLI:
        os_delay_us(1000 * arg_int);
        break;

    case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
        gpio_set_level(u8g2_esp8266_hal.cs, arg_int);
        break;

    // Set the Software I²C pin to the value passed in through arg_int.
    case U8X8_MSG_GPIO_I2C_CLOCK:
        gpio_set_level(u8g2_esp8266_hal.scl, arg_int);
        break;

    // Set the Software I²C pin to the value passed in through arg_int.
    case U8X8_MSG_GPIO_I2C_DATA:
        gpio_set_level(u8g2_esp8266_hal.sda, arg_int);
        break;

        /* case U8X8_MSG_GPIO_MENU_SELECT:
            u8x8_SetGPIOResult(u8x8, gpio_get_level(KEY_SELECT_PIN));
            break;

        case U8X8_MSG_GPIO_MENU_NEXT:
            u8x8_SetGPIOResult(u8x8, gpio_get_level(KEY_NEXT_PIN));
            break;

        case U8X8_MSG_GPIO_MENU_PREV:
            u8x8_SetGPIOResult(u8x8, gpio_get_level(KEY_PREV_PIN));
            break;

        case U8X8_MSG_GPIO_MENU_HOME:
            u8x8_SetGPIOResult(u8x8, gpio_get_level(KEY_HOME_PIN));
            break; */
    }
    return 1;
}

/**
 * @brief HAL callback function as prescribed by the U8G2 library.
 * This callback is invoked to handle SPI communications.
 */
uint8_t u8g2_esp8266_spi_byte_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int,
    void* arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_BYTE_INIT:
        init_config();
        break;

    case U8X8_MSG_BYTE_SEND:;
        uint8_t* data = (uint8_t*)arg_ptr;

        while (arg_int-- > 0) {
            // Waiting for an incomplete transfer
            while ((&SPI1)->cmd.usr)
                ;

            portENTER_CRITICAL();

            (&SPI1)->user.usr_command = 0; // Discard cmd
            (&SPI1)->user.usr_addr = 0; // Discard addr
            (&SPI1)->user.usr_miso = 0; // Discard miso
            (&SPI1)->user.usr_mosi = 1; // Enable mosi
            (&SPI1)->user1.usr_mosi_bitlen = 8 - 1;
            (&SPI1)->data_buf[0] = *data++;

            (&SPI1)->cmd.usr = 1; // Start transmission

            portEXIT_CRITICAL();
        }
        break;
    }
    return 1;
}

/**
 * @brief HAL callback function as prescribed by the U8G2 library.
 * This callback is invoked to handle I2C communications.
 *
 * Not implemented yet.
 */
uint8_t u8g2_esp8266_i2c_byte_cb(u8x8_t* u8x8, uint8_t msg, uint8_t arg_int,
    void* arg_ptr);