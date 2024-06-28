/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "draw.h"
#include "resources.h"
#include "driver_lcd.h"

#define animation_active() (false)
#define animation_movingOn() (false)
#define animation_offsetY() (0)

static uint8_t *oledBuffer;
static uint32_t g_xres, g_yres, g_bpp;


inline static void setBuffByte(byte*, byte, byte, byte);//, byte);
inline uint8_t pgm_read_byte (const uint8_t *abc) {return *abc;}

void draw_string(char* string, bool invert, byte x, byte y);


void draw_string_P(const char* string, bool invert, byte x, byte y)
{
	byte len = strlen(string);
	char buff[len+1];
	strcpy(buff, string);
	draw_string(buff, invert, x, y);
}

void draw_string(char* string, bool invert, byte x, byte y)
{
	byte charCount = 0;
	while(*string)
	{
		char c = *string - 0x20;
		byte xx = x + (charCount*7);
		draw_bitmap(xx, y, smallFont[(byte)c], SMALLFONT_WIDTH, SMALLFONT_HEIGHT, invert, 0);
        draw_flushArea(xx,y, SMALLFONT_WIDTH, SMALLFONT_HEIGHT);
		if(invert)
		{
			if(xx > 0){
				setBuffByte(oledBuffer, xx-1, y, 0xFF);//, WHITE);
                draw_flushArea(xx-1, y, 1, 8);
			}
			if(xx < g_xres - 5){
				setBuffByte(oledBuffer, xx+5, y, 0xFF);//, WHITE);
                draw_flushArea(xx+5, y, 1, 8);
			}
		}
		string++;
		charCount++;
	}
}

inline static void setBuffByte(byte* buff, byte x, byte y, byte val)//, byte colour)
{
	uint32_t pos = x + (y / 8) * g_xres;
	buff[pos] |= val;
}

inline static byte readPixels(const byte* loc, bool invert)
{
	//byte pixels = ((int)loc & 0x8000 ? eeprom_read_byte((int)loc & ~0x8000) : pgm_read_byte(loc));
	byte pixels = pgm_read_byte(loc);  //d读取flash里面的数据到ram
	if(invert)
		pixels = ~pixels;
	return pixels;
}

/* 把newval中从bit0开始的num_bit个数据位,
 * 设置进oldval里(从start_bit开始)
 * 比如:
 * oldval的bit[start_bit]设置为newval的bit[0]
 * oldval的bit[start_bit+1]设置为newval的bit[1]
 * 保留oldval中其他位不变
 */
static uint8_t set_bits(uint8_t oldval, uint8_t newval, uint8_t start_bit, uint8_t num_bit)
{
    uint8_t bitMsk = (1<<num_bit) - 1;
    newval &= bitMsk;

    oldval &= ~(bitMsk << start_bit);
    oldval |= (newval << start_bit);
    return oldval;
}

#if 0
static byte get_bitmap_byte(const byte* bitmap, byte w, byte h, byte x, byte y) 
{
	byte y_offset = y % 8;
	int32_t len;
	int32_t bitmap_offset;
	uint32_t val;

	len = (h+7)/8;
	len *= w;
	
	if (!y_offset)
	{
		bitmap_offset = (y/8*w) + x;
		if (bitmap_offset >= len)
			return 0;
		else
			return bitmap[bitmap_offset];
	}
	else
	{
		byte y0 = y / 8;
		bitmap_offset = (y0*w) + x;
		val = bitmap[bitmap_offset];
		
		bitmap_offset = (y0+1)*w + x;
		if (bitmap_offset < len)
			val |= (((uint32_t)bitmap[bitmap_offset])<<8);
		
		return (val >> y_offset) & 0xff;
	}
}

// Ultra fast bitmap drawing
// Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// PS - Sorry about the poorly named variables ;_;
void draw_bitmap(byte x, byte yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY)
{
	byte y_bitmap = 0;
	byte remain_h = h;
	
	// Apply animation offset
	yy += animation_offsetY();

	// 
	byte y = yy - offsetY;

	// 
	byte pixelOffset = (y % 8);

	// 
	byte pages = (pixelOffset + remain_h + 7) / 8;

	for (int page = 0 ; page < pages; page++)
	{
		// 
		byte y0 = y + (page * 8); // Current Y pos (every 8 pixels)
		byte y1 = y0 + 8; // Y pos at end of pixel column (8 pixels)

		if (y0 + remain_h > g_yres)
			return;
		
		byte num_bits = 8 - pixelOffset;
		if (num_bits > remain_h)
			num_bits = remain_h;
		
		uint32_t fb_offset = ((y0 / 8) * g_xres);

		for (int col = 0; col < w; col++)
		{
			byte pixels = get_bitmap_byte(bitmap, w, h, col, y_bitmap);
			
			/* 把pixels中的bits位设置进oledBuffer */
			oledBuffer[fb_offset + col + x] = set_bits(oledBuffer[fb_offset + col + x], pixels, pixelOffset, num_bits);
		}
		y_bitmap += num_bits;
		remain_h -= num_bits;
		pixelOffset = 0;
	}

}

#else

// Ultra fast bitmap drawing
// Only downside is that height must be a multiple of 8, otherwise it gets rounded down to the nearest multiple of 8
// Drawing bitmaps that are completely on-screen and have a Y co-ordinate that is a multiple of 8 results in best performance
// PS - Sorry about the poorly named variables ;_;
void draw_bitmap(byte x, byte yy, const byte* bitmap, byte w, byte h, bool invert, byte offsetY)
{
	// Apply animation offset
	yy += animation_offsetY();

	// 
	byte y = yy - offsetY;

	// 
	byte h2 = (h + 7) / 8;
	
	// 
	byte pixelOffset = (y % 8);

	byte thing3 = (yy+h);
	
	// 
	LOOP(h2, hh)
	{
		// 
		byte hhh = (hh * 8) + y; // Current Y pos (every 8 pixels)
		byte hhhh = hhh + 8; // Y pos at end of pixel column (8 pixels)

		// 
		if(offsetY && (hhhh < yy || hhhh > g_yres || hhh > thing3))
			continue;

		// 
		byte offsetMask = 0xFF;
		if(offsetY)
		{
			if(hhh < yy)
				offsetMask = (0xFF<<(yy-hhh));
			else if(hhhh > thing3)
				offsetMask = (0xFF>>(hhhh-thing3));
		}

		uint32_t aa = ((hhh / 8) * g_xres);
		
		const byte* b = bitmap + (hh*w);
				
		// If() outside of loop makes it faster (doesn't have to keep re-evaluating it)
		// Downside is code duplication
		if(!pixelOffset && hhh < g_yres)
		{
			byte num_bit = 8 - pixelOffset;
			if (num_bit > h)
				num_bit = h;
			// 
			LOOP(w, ww)
			{
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
			
				// Stop if X co-ordinate is outside the frame
				if(xx >= g_xres)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;

                /* 把pixels中的bits位设置进oledBuffer */
                oledBuffer[xx + aa] = set_bits(oledBuffer[xx + aa], pixels, 0, num_bit);
			}
			h -= num_bit;	
		}
		else
		{
			uint32_t aaa = ((hhhh / 8) * g_xres);
			byte tmp_h = h;
			
			// 
			LOOP(w, ww)
			{
				tmp_h = h;
				byte num_bit = 8 - pixelOffset;
				if (num_bit > tmp_h)
					num_bit = tmp_h;
				
				// Workout X co-ordinate in frame buffer to place next 8 pixels
				byte xx = ww + x;
		
				// Stop if X co-ordinate is outside the frame
				if(xx >= g_xres)
					continue;

				// Read pixels
				byte pixels = readPixels(b + ww, invert) & offsetMask;
				// 
				if(hhh < g_yres)
				{
                    oledBuffer[xx + aa] = set_bits(oledBuffer[xx + aa], pixels, pixelOffset, num_bit);
					tmp_h -= num_bit;
				}

				// 
				if(tmp_h && (hhhh < g_yres))
				{
					num_bit = 8 - num_bit;
					if (num_bit > tmp_h)
						num_bit = tmp_h;
                    
					pixels >>= (8 - pixelOffset);
                    oledBuffer[xx + aaa] = set_bits(oledBuffer[xx + aaa], pixels, 0, num_bit);
					tmp_h -= num_bit;	
				}
			}

			h = tmp_h;
		}
	}
}

#endif
volatile int bInUsed = 0;
void draw_flushArea(byte x, byte y, byte w, byte h)
{
    while (bInUsed);
    //taskENTER_CRITICAL();
    bInUsed = 1;
    LCD_FlushRegion(x, y, w, h);
    bInUsed = 0;
    //taskEXIT_CRITICAL();
}

// y must be a multiple of 8
// height is always 8
void draw_clearArea(byte x, byte y, byte w)
{
	uint32_t pos = x + (y / 8) * g_xres;
	memset(&oledBuffer[pos], 0x00, w);
}

void draw_end()
{
	LCD_Flush(); //刷新屏幕的意思, OLED的刷新需要100ms, 太慢了    
    //LCD_ClearFrameBuffer();
}

void draw_init(void)
{
    oledBuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
}

