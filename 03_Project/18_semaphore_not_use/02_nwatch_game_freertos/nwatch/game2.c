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
#include "driver_ir_receiver.h"
#include "driver_rotary_encoder.h"
#include "driver_mpu6050.h"

#define NOINVERT	false
#define INVERT		true

#define CAR_COUNT	3
#define CAR_WIDTH	12
#define CAR_LENGTH	15
#define ROAD_SPEED	6

static uint32_t g_xres, g_yres, g_bpp;
static uint8_t *g_framebuffer;

struct car {
    int x;
    int y;
    int control_key;
};

struct car g_cars[3] = {
    {0, 0, IR_KEY_1},
    {0, 17, IR_KEY_2},
    {0, 34, IR_KEY_3},
};

static const byte carImg[] ={
	0x40,0xF8,0xEC,0x2C,0x2C,0x38,0xF0,0x10,0xD0,0x30,0xE8,0x4C,0x4C,0x9C,0xF0,
	0x02,0x1F,0x37,0x34,0x34,0x1C,0x0F,0x08,0x0B,0x0C,0x17,0x32,0x32,0x39,0x0F,
};

static const byte clearImg[30] ={0};

static const byte roadMarking[] ={
	0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
};

#if 0
void car_test(void)
{    
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
    
    // Draw carImg
    draw_bitmap(0, 0, carImg, CAR_LENGTH, 8 * 2, NOINVERT, 0);
    draw_flushArea(0, 0, CAR_LENGTH, 8 * 2);
        
    // Draw roadMarking
    draw_bitmap(0, CAR_LENGTH + 1, roadMarking, 8, 1, NOINVERT, 0);
    draw_flushArea(0, CAR_LENGTH + 1, 8, 1);
    
    while(1);
}
#endif

/**
  * @brief 显示小车
  * @param  pcar 小车指针
  * @retval 无
  */
void ShowCar(struct car *pcar)
{
    // Draw carImg
    draw_bitmap(pcar->x, pcar->y, carImg, CAR_LENGTH, 8 * 2, NOINVERT, 0);
    draw_flushArea(pcar->x, pcar->y, CAR_LENGTH, 8 * 2); 
}

/**
  * @brief 隐藏小车
  * @param  pcar 小车指针
  * @retval 无
  */
void HideCar(struct car *pcar)
{
    draw_bitmap(pcar->x, pcar->y, clearImg, CAR_LENGTH, 8 * 2, NOINVERT, 0);
    draw_flushArea(pcar->x, pcar->y, CAR_LENGTH, 8 * 2); 
}

/**
  * @brief 小车任务
  * @param  params 参数
  * @retval 无
  */
void CarTask(void *params)
{
    struct car *pcar = params;
    struct ir_data idata;

    QueueHandle_t g_xQueueIR = xQueueCreate(10, sizeof(struct ir_data));
    RegisterQueueHandle(g_xQueueIR);
    
    /* 默认显示小车 */
    ShowCar(pcar);
    
    while(1)
    {
        /* 读取按键: 接收红外接收器队列 */
        //xQueueReceive(g_xQueueIR, &idata, portMAX_DELAY);

        /* 移动小车 */
        //if (idata.val == pcar->control_key)
        //{
            if (pcar->x < g_xres - CAR_LENGTH)
            {
                /* 隐藏小车 */
                HideCar(pcar);
                
                /* 移动位置 */
                pcar->x += 1;
                if (pcar->x > g_xres - CAR_LENGTH)
                {
                    pcar->x = g_xres - CAR_LENGTH;
                }
                
                /* 显示小车 */
                ShowCar(pcar);
                
                vTaskDelay(50);
                
                /* 到达最右边自杀 */
                if (pcar->x == g_xres - CAR_LENGTH)
                {
                    vTaskDelete(NULL);
                }
            }
        //}
    }
}

/**
  * @brief 赛车游戏
  * @param  无
  * @retval 无
  */
void car_game(void)
{    
    int i, j;
    g_framebuffer = LCD_GetFrameBuffer(&g_xres, &g_yres, &g_bpp);
    draw_init();
    draw_end();
    
    // LCD: 64 * 128
    // Draw roadMarking
    for(i = 0; i < 3; i++)
    {
        for(j = 0; j < 8; j++)
        {
            draw_bitmap(16 * j, 16 + 17 * i, roadMarking, 8, 1, NOINVERT, 0);
            draw_flushArea(16 * j, 16 + 17 * i, 8, 1);
        }
    }
    
    /* 创建赛车任务 */
    xTaskCreate(CarTask, "car1", 128, &g_cars[0], osPriorityNormal, NULL);
    xTaskCreate(CarTask, "car2", 128, &g_cars[1], osPriorityNormal, NULL);
    xTaskCreate(CarTask, "car3", 128, &g_cars[2], osPriorityNormal, NULL);
}