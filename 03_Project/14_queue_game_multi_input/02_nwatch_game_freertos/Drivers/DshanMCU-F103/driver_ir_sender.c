/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_ir_sender.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_ir_sender.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"

#define IR_SENDER_GPIO_GROUP GPIOA
#define IR_SENDER_GPIO_PIN   GPIO_PIN_8

#define IR_SENDER_CHANNEL TIM_CHANNEL_1

extern TIM_HandleTypeDef htim1;

static TIM_HandleTypeDef *g_HPWM_IRSender = &htim1;


/**********************************************************************
 * 函数名称： IRSender_Init
 * 功能描述： 红外发送器的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void IRSender_Init(void)
{
    TIM_OC_InitTypeDef sConfig;

    HAL_TIM_Base_DeInit(g_HPWM_IRSender);
    g_HPWM_IRSender->Instance = TIM1;
    g_HPWM_IRSender->Init.Prescaler = 0;
    g_HPWM_IRSender->Init.CounterMode = TIM_COUNTERMODE_UP;
    g_HPWM_IRSender->Init.Period = 1894;  /* 38KHz */
    g_HPWM_IRSender->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_HPWM_IRSender->Init.RepetitionCounter = 0;
    g_HPWM_IRSender->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(g_HPWM_IRSender);


    sConfig.OCMode = TIM_OCMODE_PWM1;        // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW; // 设置极性为低(硬件上低电平亮灯)
    sConfig.OCFastMode = TIM_OCFAST_DISABLE; // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
    sConfig.Pulse      = 473;                // 在 PWM1 模式下,75%占空比
    
    HAL_TIM_PWM_ConfigChannel(g_HPWM_IRSender, &sConfig, IR_SENDER_CHANNEL);
}

/**********************************************************************
 * 函数名称： IRSender_Write
 * 功能描述： 红外发送器的发送函数
 * 输入参数： device - 设备ID
 *            data   - 按键码
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void IRSender_Write(uint8_t device, uint8_t data)
{
    uint8_t datas[4];
    int i, j;

    datas[0] = device;
    datas[1] = ~device;
    datas[2] = data;
    datas[3] = ~data;
    
    /* 发送引导码: 9ms低脉冲,4.5ms高脉冲 */    
    HAL_TIM_PWM_Start(g_HPWM_IRSender, IR_SENDER_CHANNEL);
    udelay(9000);
    HAL_TIM_PWM_Stop(g_HPWM_IRSender, IR_SENDER_CHANNEL);
    udelay(4500);

    /* 发送32位数据: 先发送MSB */
    for (i = 0; i < 4; i++)
    {
        /* 发送数据位: 
         * 1 : 0.56ms低脉冲, 1.6ms高脉冲 
         * 0 : 0.56ms低脉冲, 0.56ms高脉冲 
         */
        for (j = 7; j >= 0; j--)
        {
            HAL_TIM_PWM_Start(g_HPWM_IRSender, IR_SENDER_CHANNEL);
            udelay(560);
            HAL_TIM_PWM_Stop(g_HPWM_IRSender, IR_SENDER_CHANNEL);
            if (datas[i] & (1<<j))
            {
                udelay(1600);
            }
            else
            {
                udelay(560);
            }
        }
    }

    /* 0.56ms的低脉冲表示数据传输结束 */
    HAL_TIM_PWM_Start(g_HPWM_IRSender, IR_SENDER_CHANNEL);
    udelay(560);
    HAL_TIM_PWM_Stop(g_HPWM_IRSender, IR_SENDER_CHANNEL);    
}


/**********************************************************************
 * 函数名称： IRSender_Test
 * 功能描述： 红外发送器测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void IRSender_Test(void)
{
    int i;
    int len;
    uint8_t device = 0x55;
    
    const uint8_t codes[]= {0xa2, 0xe2, 0x22, 0x02, 0xc2, 0xe0, 0xa8, 0x90, \
                            0x68, 0x98, 0xb0, 0x30, 0x18, 0x7a, 0x10, 0x38, \
                            0x5a, 0x42, 0x4a, 0x52, 0x00};
    const char *names[]= {"Power", "Menu", "Test", "+", "Return", "Left", "Play", "Right", \
                            "0", "-", "C", "1", "2", "3", "4", "5", \
                            "6", "7", "8", "9", "Repeat"};
    
    IRSender_Init();

    while (1)
    {
        for (i = 0; i < sizeof(codes)/sizeof(codes[0]); i++)
        {
            LCD_PrintString(0, 0, "IR Sender: ");        
            LCD_PrintString(0, 2, "Device  Data");
            LCD_PrintString(0, 4, "                ");
            LCD_PrintHex(0, 4, device, 1);
            LCD_PrintHex(8, 4, codes[i], 1);
            LCD_PrintString(0, 6, "                ");
            len = LCD_PrintString(0, 6, "Key name: ");
            LCD_PrintString(len, 6, names[i]);
            
            IRSender_Write(device, codes[i]);
            mdelay(3000);
        }
    }
}

