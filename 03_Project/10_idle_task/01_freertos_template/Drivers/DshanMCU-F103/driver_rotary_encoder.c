// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */

 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 *
 * 免责声明: 百问网编写的文档，仅供学员学习使用，可以转发或引用(请保留作者信息)，禁止用于商业用途！
 * 免责声明: 百问网编写的程序，可以用于商业用途，但百问网不承担任何后果！
 * 
 * 
 * 本程序遵循GPL V3协议，使用请遵循协议许可
 * 本程序所用的开发板：	DShanMCU-F103
 * 百问网嵌入式学习平台：https://www.100ask.net
 * 百问网技术交流社区：	https://forums.100ask.net
 * 百问网官方B站：				https://space.bilibili.com/275908810
 * 百问网官方淘宝：			https://100ask.taobao.com
 * 联系我们(E-mail)：	  weidongshan@qq.com
 *
 * 版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */


#include "driver_rotary_encoder.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

/*
 * PB12 - S1
 * PB0  - S2
 * PB1  - Key
 */
#define ROTARY_ENCODER_S1_GPIO_GROUP GPIOB
#define ROTARY_ENCODER_S1_GPIO_PIN   GPIO_PIN_12

#define ROTARY_ENCODER_S2_GPIO_GROUP GPIOB
#define ROTARY_ENCODER_S2_GPIO_PIN   GPIO_PIN_0

#define ROTARY_ENCODER_KEY_GPIO_GROUP GPIOB
#define ROTARY_ENCODER_KEY_GPIO_PIN   GPIO_PIN_1

static int32_t g_count = 0;
static int32_t g_speed = 0; /* 速度(正数表示顺时针旋转,负数表示逆时针旋转,单位:每秒转动次数) */

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_Key
 * 功能描述： 读取旋转编码器Key引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1-按键被按下, 0-按键被松开
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_Key(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ROTARY_ENCODER_KEY_GPIO_GROUP, ROTARY_ENCODER_KEY_GPIO_PIN))
        return 1;
    else
        return 0;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S1
 * 功能描述： 读取旋转编码器S1引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S1(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ROTARY_ENCODER_S1_GPIO_GROUP, ROTARY_ENCODER_S1_GPIO_PIN))
        return 0;
    else
        return 1;
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Get_S2
 * 功能描述： 读取旋转编码器S2引脚电平
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int RotaryEncoder_Get_S2(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(ROTARY_ENCODER_S2_GPIO_GROUP, ROTARY_ENCODER_S2_GPIO_PIN))
        return 0;
    else
        return 1;
}


/**********************************************************************
 * 函数名称： RotaryEncoder_IRQ_Callback
 * 功能描述： 旋转编码器的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_IRQ_Callback(void)
{
    uint64_t time;
    static uint64_t pre_time = 0;
        
	/* 1. 记录中断发生的时刻 */	
	time = system_get_ns();

    /* 上升沿触发: 必定是高电平 
     * 防抖
     */
    mdelay(2);
    if (!RotaryEncoder_Get_S1())
        return;

    /* S1上升沿触发中断
     * S2为0表示逆时针转, 为1表示顺时针转
     */
    g_speed = (uint64_t)1000000000/(time - pre_time);
    if (RotaryEncoder_Get_S2())
    {
        g_count++;
    }
    else
    {
        g_count--;
        g_speed = 0 - g_speed;
    }
    pre_time = time;
        
}


/**********************************************************************
 * 函数名称： RotaryEncoder_Init
 * 功能描述： 旋转编码器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_Init(void)
{
    /* PB0,PB1在MX_GPIO_Init中被配置为输入引脚 */
    /* PB12在MX_GPIO_Init中被配置为中断引脚,上升沿触发 */
}

/**********************************************************************
 * 函数名称： RotaryEncoder_Write
 * 功能描述： 旋转编码器的读取函数
 * 输入参数： 无
 * 输出参数： pCnt   - 用于保存计数值 
 *            pSpeed - 用于保存速度(正数表示顺时针旋转,负数表示逆时针旋转,单位:每秒转动次数)
 *            pKey   - 用于保存按键状态(1-按键被按下, 0-按键被松开)
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void RotaryEncoder_Read(int32_t *pCnt, int32_t *pSpeed, int32_t *pKey)
{
    static int32_t pre_count = 0;
    *pCnt = g_count;

    if (g_count == pre_count)
        *pSpeed = 0;
    else
        *pSpeed = g_speed;

    pre_count = g_count;
    
    *pKey = RotaryEncoder_Get_Key();
}


/**********************************************************************
 * 函数名称： RotaryEncoder_Test
 * 功能描述： 旋转编码器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void RotaryEncoder_Test(void)
{
    int32_t count, speed, key;
    int len;
    
    RotaryEncoder_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "RotaryEncoder: ");

        RotaryEncoder_Read(&count, &speed, &key);

        len = LCD_PrintString(0, 2, "Cnt  : ");
        len += LCD_PrintSignedVal(len, 2, count);
        LCD_ClearLine(len, 2);
        
        len = LCD_PrintString(0, 4, "Speed: ");
        len += LCD_PrintSignedVal(len, 4, speed);
        LCD_ClearLine(len, 4);
        
        len = LCD_PrintString(0, 6, "Key  : ");
        len += LCD_PrintString(len, 6, key ? "Pressed":"Released");
        LCD_ClearLine(len, 6);
    }
}


