/*
 * Project: N|Watch
 * Author: Zak Kemble, contact@zakkemble.co.uk
 * Copyright: (C) 2013 by Zak Kemble
 * License: GNU GPL v3 (see License.txt)
 * Web: http://blog.zakkemble.co.uk/diy-digital-wristwatch/
 */
#include <stdlib.h>
#include <stdio.h>

#include "cmsis_os.h"
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#include "draw.h"
#include "resources.h"

#include "driver_lcd.h"

#define NOINVERT	false
#define INVERT		true

#define sprintf_P  sprintf
#define PSTR(a)  a

#define PLATFORM_WIDTH	12
#define PLATFORM_HEIGHT	4
#define UPT_MOVE_NONE	0
#define UPT_MOVE_RIGHT	1
#define UPT_MOVE_LEFT	2
#define BLOCK_COLS		32
#define BLOCK_ROWS		5
#define BLOCK_COUNT		(BLOCK_COLS * BLOCK_ROWS)

typedef struct{
	float x;
	float y;
	float velX;
	float velY;
}s_ball;

static const byte block[] ={
	0x07,0x07,0x07,
};

static const byte platform[] ={
	0x60,0x70,0x50,0x10,0x30,0xF0,0xF0,0x30,0x10,0x50,0x70,0x60,
};

static const byte ballImg[] ={
	0x03,0x03,
};

static const byte clearImg[] ={
	0,0,0,0,0,0,0,0,0,0,0,0,
};

static bool btnExit(void);
static bool btnRight(void);
static bool btnLeft(void);
void game1_draw(void);

static byte uptMove;
static s_ball ball;
static bool* blocks;
static byte lives, lives_origin;
static uint score;
static byte platformX;

static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;

/* 声明队列 */
QueueHandle_t g_xQueuePlatform;

/* 挡球板任务 */
static void platform_task(void *params)
{
    byte platformXtmp = platformX;    
    uint8_t dev, data, last_data;
    struct input_data idata;

    // Draw platform
    draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
    
    while (1)
    {
        /* 读取红外遥控器 */
//		if (0 == IRReceiver_Read(&dev, &data))  // 无阻塞，效率低
        if (pdPASS == xQueueReceive(g_xQueuePlatform, &idata, portMAX_DELAY))   // 有阻塞，效率高
		{
            data = idata.val;
            if (data == 0x00)
            {
                data = last_data;
            }
            
            if (data == 0xe0) /* Left */
            {
                btnLeft();
            }

            if (data == 0x90)  /* Right */
            {
                btnRight();
            }
            last_data = data;

            
            // Hide platform
            draw_bitmap(platformXtmp, g_yres - 8, clearImg, 12, 8, NOINVERT, 0);
            draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
            
            // Move platform
            if(uptMove == UPT_MOVE_RIGHT)
                platformXtmp += 3;
            else if(uptMove == UPT_MOVE_LEFT)
                platformXtmp -= 3;
            uptMove = UPT_MOVE_NONE;
            
            // Make sure platform stays on screen
            if(platformXtmp > 250)
                platformXtmp = 0;
            else if(platformXtmp > g_xres - PLATFORM_WIDTH)
                platformXtmp = g_xres - PLATFORM_WIDTH;
            
            // Draw platform
            draw_bitmap(platformXtmp, g_yres - 8, platform, 12, 8, NOINVERT, 0);
            draw_flushArea(platformXtmp, g_yres - 8, 12, 8);
            
            platformX = platformXtmp;
            
		}
    }
}

void game1_task(void *params)
{		
    uint8_t dev, data, last_data;
    struct input_data idata;
    
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
    
    /* 创建队列 */
    g_xQueuePlatform = xQueueCreate(10, sizeof(struct input_data));
    
	uptMove = UPT_MOVE_NONE;

	ball.x = g_xres / 2;
	ball.y = g_yres - 10;
        
	ball.velX = -0.5;
	ball.velY = -0.6;
//	ball.velX = -1;
//	ball.velY = -1.1;

	blocks = pvPortMalloc(BLOCK_COUNT);
    memset(blocks, 0, BLOCK_COUNT);
	
	lives = lives_origin = 3;
	score = 0;
	platformX = (g_xres / 2) - (PLATFORM_WIDTH / 2);

    xTaskCreate(platform_task, "platform_task", 128, NULL, osPriorityNormal, NULL);

    while (1)
    {
        game1_draw();
        //draw_end();
        vTaskDelay(50);
    }
}

static bool btnExit()
{
	
	vPortFree(blocks);
	if(lives == 255)
	{
		//game1_start();
	}
	else
	{
		//pwrmgr_setState(PWR_ACTIVE_DISPLAY, PWR_STATE_NONE);	
		//animation_start(display_load, ANIM_MOVE_OFF);
		vTaskDelete(NULL);
	}
	return true;
}

static bool btnRight()
{
	uptMove = UPT_MOVE_RIGHT;
	return false;
}

static bool btnLeft()
{
	uptMove = UPT_MOVE_LEFT;
	return false;
}

void game1_draw()
{
	bool gameEnded = ((score >= BLOCK_COUNT) || (lives == 255));

	byte platformXtmp = platformX;

    static bool first = 1;

	// Move ball
	// hide ball
	draw_bitmap(ball.x, ball.y, clearImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    //draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    //draw_flushArea(platformX, g_yres - 8, 12, 8);
	
	if(!gameEnded)
	{
		ball.x += ball.velX;
		ball.y += ball.velY;
	}

	bool blockCollide = false;
	const float ballX = ball.x;
	const byte ballY = ball.y;

	// Block collision
	byte idx = 0;
	LOOP(BLOCK_COLS, x)
	{
		LOOP(BLOCK_ROWS, y)
		{
			if(!blocks[idx] && ballX >= x * 4 && ballX < (x * 4) + 4 && ballY >= (y * 4) + 8 && ballY < (y * 4) + 8 + 4)
			{
//				buzzer_buzz(100, TONE_2KHZ, VOL_UI, PRIO_UI, NULL);
				// led_flash(LED_GREEN, 50, 255); // 100ask todo
				blocks[idx] = true;

                // hide block
                draw_bitmap(x * 4, (y * 4) + 8, clearImg, 3, 8, NOINVERT, 0);                
                draw_flushArea(x * 4, (y * 4) + 8, 3, 8);                
				blockCollide = true;
				score++;
			}
			idx++;
		}
	}


	// Side wall collision
	if(ballX > g_xres - 2)
	{
		if(ballX > 240)
			ball.x = 0;		
		else
			ball.x = g_xres - 2;
		ball.velX = -ball.velX;		
	}
	if(ballX < 0)
  {
		ball.x = 0;		
		ball.velX = -ball.velX;	
  }

	// Platform collision
	bool platformCollision = false;
	if(!gameEnded && ballY >= g_yres - PLATFORM_HEIGHT - 2 && ballY < 240 && ballX >= platformX && ballX <= platformX + PLATFORM_WIDTH)
	{
		platformCollision = true;
		// buzzer_buzz(200, TONE_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
		ball.y = g_yres - PLATFORM_HEIGHT - 2;
		if(ball.velY > 0)
			ball.velY = -ball.velY;
		ball.velX = ((float)rand() / (RAND_MAX / 2)) - 1; // -1.0 to 1.0
	}

	// Top/bottom wall collision
	if(!gameEnded && !platformCollision && (ballY > g_yres - 2 || blockCollide))
	{
		if(ballY > 240)
		{
			// buzzer_buzz(200, TONE_2_5KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = 0;
		}
		else if(!blockCollide)
		{
			// buzzer_buzz(200, TONE_2KHZ, VOL_UI, PRIO_UI, NULL); // 100ask todo
			ball.y = g_yres - 1;
			lives--;
		}
		ball.velY *= -1;
	}

	// Draw ball
	draw_bitmap(ball.x, ball.y, ballImg, 2, 2, NOINVERT, 0);
    draw_flushArea(ball.x, ball.y, 2, 8);

    // Draw platform
    //draw_bitmap(platformX, g_yres - 8, platform, 12, 8, NOINVERT, 0);
    //draw_flushArea(platformX, g_yres - 8, 12, 8);

    if (first)
    {
        first = 0;
        
    	// Draw blocks
    	idx = 0;
    	LOOP(BLOCK_COLS, x)
    	{
    		LOOP(BLOCK_ROWS, y)
    		{
    			if(!blocks[idx])
    			{
    				draw_bitmap(x * 4, (y * 4) + 8, block, 3, 8, NOINVERT, 0);
                    draw_flushArea(x * 4, (y * 4) + 8, 3, 8);                
    			}
    			idx++;
    		}
    	}
        
    }

	// Draw score
	char buff[6];
	sprintf_P(buff, PSTR("%u"), score);
	draw_string(buff, false, 0, 0);

    // Draw lives
    if(lives != 255)
    {
        LOOP(lives_origin, i)
        {
            if (i < lives)
                draw_bitmap((g_xres - (3*8)) + (8*i), 1, livesImg, 7, 8, NOINVERT, 0);
            else
                draw_bitmap((g_xres - (3*8)) + (8*i), 1, clearImg, 7, 8, NOINVERT, 0);
            draw_flushArea((g_xres - (3*8)) + (8*i), 1, 7, 8);    
        }
    }   

	// Got all blocks
	if(score >= BLOCK_COUNT)
		draw_string_P(PSTR(STR_WIN), false, 50, 32);

	// No lives left (255 because overflow)
	if(lives == 255)
		draw_string_P(PSTR(STR_GAMEOVER), false, 34, 32);

}

