/*
 * fbgfx.c
 *
 *  Created on: Mar 7, 2026
 *      Author: ssmia
 */
#include "fbgfx.h"
#include "fonts.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define WHITE false
#define BLACK true

static void set_pixel(gfx_display_t* Display, uint16_t x, uint16_t y, bool state) {
    uint16_t byte_width = (Display->width + 7) / 8;

    int byte_pos = y * byte_width + (x / 8);
    uint8_t bit = 0x80 >> (x & 7);

    if (state) {
        // black pixel → clear bit
        Display->framebuffer[byte_pos] &= ~bit;
    } else {
        // white pixel → set bit
        Display->framebuffer[byte_pos] |= bit;
    }
}

//very basic line drawing primitives for now
static void draw_hline_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t length, bool colour)
{
	for(int i=0; i<=length; i++){
		set_pixel(Display, x+i, y, colour);
	}
}

static void draw_vline_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t length, bool colour)
{
	for(int i=0; i<=length; i++){
		set_pixel(Display, x, y+i, colour);
	}
	/*
	 * personal note: if you're wondering why we're not bounds checking here, it's because of
	 * high cohesion. remember that the responsibility of every entity must be very clear. they
	 * should not be mixed. only set_pixel should be responsible for bounds checking
	 */
}

static void fill_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool colour){
	// if x and/or y exceed the bounds of the framebuffer they will be constrained
	// their values will be bound to the constraints of the framebuffer
	uint16_t maximum_width = Display->width;
	uint16_t maximum_height = Display->height;

	x = (x > maximum_width) ? maximum_width: x;
	y = (y > maximum_height) ? maximum_height : y;

	//the rectangle is built by stacking lines of pixels side by side
	for(uint16_t i = x; i<(x+width); i++){
		if(i >= maximum_width)
			break;
		for(uint16_t j=y; j<(y+height); j++){
			if(j>=maximum_height)
				break;
			set_pixel(Display, i, j, colour);
		}
	}
}

static void empty_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool colour)
{
	draw_hline_gfx(Display, x, y, width, colour);
	draw_hline_gfx(Display, x, y+height, height, colour);

	draw_vline_gfx(Display, x, y, height, colour);
	draw_vline_gfx(Display, x+width, y, height, colour);
}

void draw_rectangle_gfx(gfx_display_t* Display, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t fillStatus)
{
	switch(fillStatus){
	case 0:
		//need to figure out configuring line colour
		empty_rectangle_gfx(Display, x, y, width, height, BLACK);
		break;
	case 1:
		fill_rectangle_gfx(Display, x, y, width, height, BLACK);
		break;
	case 2:
		fill_rectangle_gfx(Display, x, y, width, height, WHITE);
		break;
	}
}


void testVerticalgfx(gfx_display_t* Display)
{
	for(int i=0; i<40; i++){
		set_pixel(Display, 60, i, true);
		set_pixel(Display, 30, i, true);
		set_pixel(Display, 90, i, true);
		set_pixel(Display, 110, i, true);
	}
}

void testHorizontalgfx(gfx_display_t* Display)
{
	for(int i=0; i<16; i++){
		set_pixel(Display, i, 100, true);
		set_pixel(Display, i, 50, true);
	}

	for(int i=0; i<8; i++){
		set_pixel(Display, i, 150, true);
		set_pixel(Display, i, 200, true);
	}
}

void HardFault_Handler(void)
{
    while (1);
}

//void draw_bitmap_gfx(gfx_display_t* Display, const uint8_t *bitmap, uint8_t width, uint16_t height, uint8_t x, uint16_t y){
//
//	uint8_t chkMsk;
//
//	//any edge cases that must be dealt with here?
//	uint8_t bitmap_size = (width * height)>>3;
//
//	uint8_t original_x = x;//save initial x so we can return to it when writing next row
//	uint8_t original_y = y;//save initial y so we know full height of bitmap has been written
//
//	//for each byte
//	for(uint8_t i=0; i<bitmap_size; i++){
//		//for each bit in that byte
//		for(uint8_t j=0; j<8; j++){
//			//hard cut off if the pixel we are setting exceeds display width
//			if(x>=Display->width){
//				break;
//			}
//			chkMsk = 0x80 >> j;//checks status of each bit in the byte we are writing
//			set_pixel(Display,x, y, (bitmap[i] & chkMsk) ? 0 : 1);
//			x++;
//			//if the full width of the bitmap has been written move to next row in bitmap
//			if(x-original_x == width){
//				x = original_x;
//				y++;
//			}
//		}
//	}
//
//	//repeat until full height of bitmap has been written or y is out of range
//	if (y - original_y == height || y>Display->height){
//		return;
//	}
//}

void draw_bitmap_gfx(gfx_display_t* Display, const uint8_t *bitmap, uint8_t width, uint16_t height, uint8_t x, uint16_t y){
	uint8_t chkMsk;

	//any edge cases that must be dealt with here?
	uint16_t bitmap_size = (width * height)>>3;

	uint8_t x0 = x;//save initial x so we can return to it when writing next row

	//for each byte
	for(uint16_t i = 0; i<bitmap_size; i++){
		//for each bit in that byte
		for(uint8_t j = 0; j<8; j++){
			chkMsk = 0x80 >> j; //checks status of each bit in the byte we are writing
			if(x<Display->width && y<Display->height)
				set_pixel(Display,x, y, (bitmap[i] & chkMsk) ? 0 : 1);
			x++; // move to next pixel in the framebuffer

			// if entire width of bitmap has been written move to the next row of the framebuffer
			if(x-x0 == width){
				x = x0;
				y++;
			}
		}
	}
}

static void bitmap_bitwise_invert(const uint8_t* bitmap, uint8_t* inverted, uint8_t width, uint16_t height){
	uint16_t bitmap_size = (width * height)>>3;
	for(uint16_t i = 0; i<bitmap_size; i++)
			inverted[i] = bitmap[i] ^ 0XFF;
}

void draw_text_gfx(gfx_display_t* Display, int x, int y, int size, char *text) {

	const uint8_t *font;
	const uint8_t *drawChar;

	int runningX = x;
	int runningY = y;

	int target;

	switch (size) {
		case 16:
			font = CGA16;
			break;
		default:
			font = TERMINUS;
	}

	for (int i = 0; i < strlen(text); i++) {

		if (runningX >= Display->width) {

			runningX = x;
			runningY += size;
		}

		if (runningY >= Display->height) {
			return;
		}

		uint16_t bitmap_size = (size*size >> 3);
		target = bitmap_size * (text[i] - ' ');


		drawChar = font + target;
		uint8_t invertedChar[bitmap_size];

		bitmap_bitwise_invert(drawChar, invertedChar, 16, 16);
		draw_bitmap_gfx(Display, invertedChar, size, size, runningX, runningY);

		runningX += size;
	}
}
