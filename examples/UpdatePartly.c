/*

  UpdatePartly.ino

  This will scroll text on the display similar to ScrollingText.ino example,
  Scrolling will be faster, because only the changing part of the display
  gets updated.
  Enable U8g2 16 bit mode (see FAQ) for larger text!

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "u8g2_esp8266_hal.h"

/*
  U82glib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.

  This is a page buffer example.

*/

// This example shows a scrolling text.
// If U8G2_16BIT is not set (default), then the pixel width of the text must be lesser than 128
// If U8G2_16BIT is set, then the pixel width an be up to 32000

u8g2_uint_t offset; // current offset for the scrolling text
u8g2_uint_t width; // pixel width of the scrolling text (must be lesser than 128 unless U8G2_16BIT is defined
const char* text = "U8g2 "; // scroll this text from right to left

u8g2_t u8g2;

void drawText(void)
{
    u8g2_uint_t x = offset;
    do { // repeated drawing of the scrolling text...
        u8g2_DrawUTF8(&u8g2, x, 19, text); // draw the scrolling text
        x += width; // add the pixel width of the scrolling text
    } while (x < u8g2_GetDisplayWidth(&u8g2)); // draw again until the complete display is filled
}

static void begin(u8g2_t* u8g2)
{
    u8g2_InitDisplay(u8g2);
    u8g2_ClearDisplay(u8g2);
    u8g2_SetPowerSave(u8g2, 0);
}

void app_main(void)
{
    u8g2_esp8266_hal_t u8g2_esp8266_hal = {
        .mosi = GPIO_NUM_13,
        .clk = GPIO_NUM_14,
        .cs = GPIO_NUM_15
    };
    u8g2_esp8266_hal_init(u8g2_esp8266_hal);

    u8g2_Setup_st7920_s_128x64_f(&u8g2, U8G2_R0,
        u8g2_esp8266_spi_byte_cb,
        u8g2_esp8266_gpio_and_delay_cb);

    begin(&u8g2);

    u8g2_SetFont(&u8g2, u8g2_font_helvB18_tr); // set the target font to calculate the pixel width

    width = u8g2_GetUTF8Width(&u8g2, text); // calculate the pixel width of the text

    u8g2_SetFontMode(&u8g2, 0); // enable transparent mode, which is faster

    // draw the constant part of the screen
    // this will not be overwritten later
    u8g2_FirstPage(&u8g2);
    do {
        u8g2_DrawUTF8(&u8g2, 18, 20, "[ U8g2 ]"); // This part will stay constantly on the screen
    } while (u8g2_NextPage(&u8g2));

    while (1) {
        int i;

        // write the scrolling text to the lower part of the display
        for (i = 0; i < 3; i++) {
            // draw to lines 0...23 (3*8-1)
            // draw to the upper part of the screen
            u8g2_SetBufferCurrTileRow(&u8g2, i);
            u8g2_ClearBuffer(&u8g2);
            drawText();

            // but write the buffer to the lower part (offset 4*8 = 32)
            u8g2_SetBufferCurrTileRow(&u8g2, 4 + i);
            u8g2_SendBuffer(&u8g2);
        }

        // calculate the new offset for the scrolling
        offset -= 1; // scroll by one pixel
        if ((u8g2_uint_t)offset < (u8g2_uint_t)-width)
            offset = 0; // start over again

        vTaskDelay(pdMS_TO_TICKS(10)); // do some small delay
    }
}
