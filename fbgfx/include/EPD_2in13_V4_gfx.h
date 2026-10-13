/*
 * EPD_2in13_V4_gfx.h
 *
 *  Created on: Mar 7, 2026
 *      Author:
 */

#ifndef INCLUDE_EPD_2IN13_V4_GFX_H_
#define INCLUDE_EPD_2IN13_V4_GFX_H_

#include "fbgfx.h"
#include "EPD_2in13_V4.h"

#define NO_FILL 0
#define FILL_BLACK 1
#define FILL_WHITE 2

void init(void);
void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fillStatus);
void drawBitmap(const uint8_t *bitmap, uint8_t width, uint16_t height, uint8_t x, uint16_t y);
void drawText(int x, int y, int size, char *text);
void clearDisplay();

//test functions
void testHorizontal();
void testVertical();

void refresh(void);

#endif /* INCLUDE_EPD_2IN13_V4_GFX_H_ */
