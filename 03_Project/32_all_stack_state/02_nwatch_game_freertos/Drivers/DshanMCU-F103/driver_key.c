/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_key.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.8.05      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_key.h"
#include "driver_led.h"
#include "driver_lcd.h"

#include "gpio.h"

#define KEY_GPIO_GROUP GPIOB
#define KEY_GPIO_PIN   GPIO_PIN_14


/**********************************************************************
 * 函数名称： Key_Init
 * 功能描述： 按键初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Pin  = KEY_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(KEY_GPIO_GROUP, &GPIO_InitStruct);
}

/**********************************************************************
 * 函数名称： Key_Read
 * 功能描述： 按键读取函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 1 - 按键被按键, 0 - 按键被松开
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
int Key_Read(void)
{
    if (GPIO_PIN_RESET == HAL_GPIO_ReadPin(KEY_GPIO_GROUP, KEY_GPIO_PIN))
        return 1;
    else
        return 0;    
}

/**********************************************************************
 * 函数名称： Key_Test
 * 功能描述： 按键测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void Key_Test(void)
{
    int len;
    int val;
    
    Key_Init();
    Led_Init();

    while (1)
    {
        LCD_PrintString(0, 0, "Key Test: ");
        
        val = Key_Read();
        Led_Control(LED_GREEN, val);
        
        len = LCD_PrintString(0, 2, "Key : ");
        len += LCD_PrintString(len, 2, val ? "Pressed":"Released");
        LCD_ClearLine(len, 2);
    }
}

