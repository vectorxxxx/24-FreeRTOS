/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_active_buzzer.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.8.03      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_active_buzzer.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"

/**********************************************************************
 * 函数名称： ActiveBuzzer_Init
 * 功能描述： 有源蜂鸣器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ActiveBuzzer_Init(void)
{
    /* 对于STM32F103, 有源蜂鸣器的引脚PA8配置为"output, push-pull" */
    /*Configure GPIO pin : PA8 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/**********************************************************************
 * 函数名称： ActiveBuzzer_Control
 * 功能描述： 有源蜂鸣器控制函数
 * 输入参数： on - 1-响, 0-不响
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void ActiveBuzzer_Control(int on)
{
    if (on)
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}

/**********************************************************************
 * 函数名称： ActiveBuzzer_Test
 * 功能描述： 有源蜂鸣器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void ActiveBuzzer_Test(void)
{
    ActiveBuzzer_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "ActiveBuzzer ON ");
        ActiveBuzzer_Control(1);
        mdelay(1000);

        LCD_PrintString(0, 0, "ActiveBuzzer OFF");
        ActiveBuzzer_Control(0);
        mdelay(1000);
    }
}


