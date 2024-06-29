/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_led.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.8.03      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_led.h"
#include "driver_timer.h"

#include "gpio.h"

/**********************************************************************
 * 函数名称： Led_Init
 * 功能描述： LED初始化函数, 在HAL的初始化代码里已经配置好了GPIO引脚, 
 *            所以本函数可以写为空
 *            但是为了不依赖于stm32cubemx, 此函数也实现了GPIO的配置
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    
    return 0;
}

/**********************************************************************
 * 函数名称： Led_Control
 * 功能描述： LED控制函数
 * 输入参数： which-哪个LED, 在driver_led.h里定义, 比如LED_GREEN
 *            on-状态, 1-亮, 0-灭
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int Led_Control(int which, int on)
{
    if (on)
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
    return 0;
}

/**********************************************************************
 * 函数名称： Led_Test
 * 功能描述： Led测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/03        V1.0     韦东山       创建
 ***********************************************************************/
void Led_Test(void)
{
	Led_Init();

    while (1)
    {
        Led_Control(LED_GREEN, 1);
        mdelay(500);

        Led_Control(LED_GREEN, 0);
        mdelay(500);
    }
}

