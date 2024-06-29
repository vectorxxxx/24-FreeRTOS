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

#include "driver_passive_buzzer.h"

static TimerHandle_t g_xTimerBuzzer;

/**
  * @brief 停止蜂鸣器
  * @param  无
  * @retval 无
  */
void buzzer_stop(TimerHandle_t xTimer)
{
    PassiveBuzzer_Control(0);
}

/**
  * @brief 初始化蜂鸣器
  * @param  无
  * @retval 无
  */
void buzzer_init(void)
{
    /* 初始化无源蜂鸣器 */
    PassiveBuzzer_Init(); 
    
    /* 创建定时器 */
    g_xTimerBuzzer = xTimerCreate("GameSound", 
							200,
							pdFALSE,  // 一次性
							NULL,
							buzzer_stop);
}

/**
  * @brief 发出蜂鸣器
  * @param  无
  * @retval 无
  */
void buzzer_buzz(int freq, int time_ms)
{
    /* 设置蜂鸣器频率 */
    PassiveBuzzer_Set_Freq_Duty(freq, 50);
    
    /* 修改定时器周期，兼有启动之功效 */
    xTimerChangePeriod(g_xTimerBuzzer, time_ms, 0);
}