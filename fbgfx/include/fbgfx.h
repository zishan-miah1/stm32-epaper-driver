/*
 * fbgfx.h
 *
 *  Created on: Mar 7, 2026
 *      Author:
 */

#ifndef INCLUDE_FBGFX_H_
#define INCLUDE_FBGFX_H_
#include <stdint.h>

typedef struct{
	uint16_t width;
	uint16_t height;
	uint8_t *framebuffer;
}gfx_display_t;

//void fill_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool colour);
//void empty_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void draw_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fillStatus);
//void draw_line_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t length, bool colour);

void draw_bitmap_gfx(gfx_display_t* Display, const uint8_t *bitmap, uint8_t width, uint16_t height, uint8_t x, uint16_t y);
void draw_text_gfx(gfx_display_t* Display, int x, int y, int size, char *text);
void testHorizontalgfx(gfx_display_t* Display);
void testVerticalgfx(gfx_display_t* Display);

#endif /* INCLUDE_FBGFX_H_ */
