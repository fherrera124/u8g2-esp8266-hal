# U8g2 compatibility component for ESP8266_RTOS_SDK framework

## Note
This has been tested on [ESP8266_RTOS_SDK](https://github.com/espressif/ESP8266_RTOS_SDK) version 3.4.
For a version for ESP32 devices using ESP-IDF framework, visit: [u8g2-hal-esp-idf](https://github.com/mkfrey/u8g2-hal-esp-idf)

## Description
There is an excellent open source library called `u8g2` that can be found on Github here:

[https://github.com/olikraus/u8g2](https://github.com/olikraus/u8g2)

The purpose of the library is to provide a display independent driver layer for monochrome displays including LCD and OLED.
The library "knows" how to driver the underlying displays as well as providing drawing primitives including text, fonts, lines and
other geometrical shapes.

A version of the library is available in cleanly compiling C and compiles without incident on the ESP8266_RTOS_SDK framework.

However, since the library is agnostic of MCU environments and will work on a variety of boards, there has to be a mapping from
the functions expected by the library to the underlying MCU board hardware. This includes driving GPIOs, I2C, SPI and more.

The code in this folder provides a mapping from U8g2 to the ESP8266 ESP8266_RTOS_SDK. This should be included in your build of U8g2 applications.

To use with the ESP8266, we must invoke the `u8g2_esp8266_hal_init()` function before invoking any of the normal U8g2 functions. What
this call does is tell the ESP8266 what pins we wish to map.  Here is an example of SPI use:

```
u8g2_esp8266_hal_t u8g2_esp8266_hal = U8G2_ESP8266_HAL_DEFAULT;
u8g2_esp8266_hal.clk   = PIN_CLK;
u8g2_esp8266_hal.mosi  = PIN_MOSI;
u8g2_esp8266_hal.cs    = PIN_CS;
u8g2_esp8266_hal.dc    = PIN_DC;
u8g2_esp8266_hal.reset = PIN_RESET;
u8g2_esp8266_hal_init(u8g2_esp8266_hal);
```

Remember, ESP8266 pins are not hard-coded to functions and as such, all the GPIO pins on the ESP8266 are open for use. Following
this initialization, we can use U8g2 as normal and described in the U8g2 documentation.

## TODO
Add I2C hal implementation

## Installation
To use the actual U8g2 library in your ESP8266 project, perform the following steps:

1. Create a directory called `components` in your main project directory.
2. Change into the `components` directory.
3. Run `git clone https://github.com/fherrera124/u8g2-esp8266-hal.git` to bring in the latest copy of this library.
4. Run `git clone https://github.com/olikraus/u8g2.git` to bring in a the latest copy of u8g2 library.

If your project itself is a git repository, you should consider using `git submodule add` instead of cloning.

## Development
While in principal, there should be nothing specific needed beyond this addition to make U8g2 work on the ESP8266, only a small
number of boards have been tested.
