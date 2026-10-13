/*****************************************************************************
* | File      	:   EPD_2Iin13_V4.h
* | Author      :   Waveshare team, Zishan Miah
* | Function    :   2.13inch e-paper V4
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   03-03-2026
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/

#ifndef DISPLAY_INCLUDE_EPD_2IN13_V4_H_
#define DISPLAY_INCLUDE_EPD_2IN13_V4_H_

#include "hal_driver_utils.h"
#include "stm32f030xx_gpio_drivers.h"
#include "stm32f030xx_spi_drivers.h"
#include <stdint.h>

// ===============================
// === CONFIGURATION SECTION
// ===============================

// GPIO pins used to interface with the e-Paper
#define EPD_RST_PIN 	GPIOB, GPIO_PIN_NO_1
#define EPD_DC_PIN		GPIOB, GPIO_PIN_NO_14
#define EPD_BUSY_PIN 	GPIOB, GPIO_PIN_NO_2
#define EPD_MOSI_PIN 	GPIOB, GPIO_PIN_NO_15
#define EPD_SCLK_PIN 	GPIOB, GPIO_PIN_NO_13
#define EPD_CS_PIN 		GPIOB, GPIO_PIN_NO_12

// ===============================
// === STM32 INTEGRATION
// ===============================

// Waveshare data types:
#define UBYTE uint8_t
#define UWORD uint16_t
#define UDOUBLE uint32_t

/*
 * after preprocessing, waveshare defined functions will be replaced by their
 * equivalent from custom hal layer
 */
#define DEV_Digital_Write(_pin, _value) GPIO_WriteToOutputPin(_pin, _value == 0 ? GPIO_PIN_RESET : GPIO_PIN_SET)
#define DEV_Digital_Read(_pin) 			GPIO_ReadFromInputPin(_pin)
#define DEV_Delay_ms(__xms) 			crude_delay_ms(__xms)


// Display resolution
#define EPD_2in13_V4_WIDTH       122
#define EPD_2in13_V4_HEIGHT      263
//#define DISPLAY_WIDTH			EPD_2in13_V4_WIDTH
//#define DISPLAY_HEIGHT			EPD_2in13_V4_HEIGHT

//void DEV_SPI_WriteByte(UBYTE value);
void EPD_2in13_V4_Init(void);
void EPD_2in13_V4_Init_Fast(void);
void EPD_2in13_V4_Init_GUI(void);
void EPD_2in13_V4_Clear(void);
void EPD_2in13_V4_Clear_Black(void);
void EPD_2in13_V4_Display(UBYTE *Image);
void EPD_2in13_V4_Display_Fast(UBYTE *Image);
void EPD_2in13_V4_Display_Base(UBYTE *Image);
void EPD_2in13_V4_Display_Partial(UBYTE *Image);
void EPD_2in13_V4_Sleep(void);

#endif /* DISPLAY_INCLUDE_EPD_2IN13_V4_H_ */
