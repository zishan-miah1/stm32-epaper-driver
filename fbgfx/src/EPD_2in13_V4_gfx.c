/*
 * EPD_2in13_V4_gfx.c
 *
 *  Created on: Mar 7, 2026
 *      Author:
 */
#include "fbgfx.h"
#include "EPD_2in13_V4.h"
#include "EPD_2in13_V4_gfx.h"
#include <stdint.h>
#include <string.h>

/*
 * this file should use the graphics library to fill the framebuffer with data
 * then use the display driver to push that framebuffer to the display
 */

static uint8_t FB[(EPD_2in13_V4_WIDTH * EPD_2in13_V4_HEIGHT) >> 3];


gfx_display_t EPD_2in13_V4_FB = {
    .width = EPD_2in13_V4_WIDTH,
    .height = EPD_2in13_V4_HEIGHT,
	.framebuffer = FB //FB decays to a pointer
};

//void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height){
//	fill_rectangle_gfx(&EPD_2in13_V4_FB,x,y,width,height);
//}

void drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fillStatus){
	//call drawRectangle_gfx
	draw_rectangle_gfx(&EPD_2in13_V4_FB,x,y,width,height,fillStatus);
}


void init(void){
	memset(FB, 0xFF, sizeof(FB));
	EPD_2in13_V4_Init();
}

void drawBitmap(const uint8_t *bitmap, uint8_t width, uint16_t height, uint8_t x, uint16_t y)
{
	draw_bitmap_gfx(&EPD_2in13_V4_FB, bitmap, width, height, x, y);
}

void drawText(int x, int y, int size, char *text)
{
	draw_text_gfx(&EPD_2in13_V4_FB, x, y, size, text);
}

void clearDisplay(){
	memset(FB, 0xFF, sizeof(FB));
}

void testHorizontal(){
	testHorizontalgfx(&EPD_2in13_V4_FB);
}

void testVertical(){
	testVerticalgfx(&EPD_2in13_V4_FB);
}

void refresh(void){
	EPD_2in13_V4_Display(EPD_2in13_V4_FB.framebuffer);
}
