/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_motor.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.08.05      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_motor.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"


#define MOTOR_A_IN1_GPIO_GROUP GPIOA
#define MOTOR_A_IN1_GPIO_PIN   GPIO_PIN_4

#define MOTOR_A_IN2_GPIO_GROUP GPIOA
#define MOTOR_A_IN2_GPIO_PIN   GPIO_PIN_0

#define MOTOR_B_IN1_GPIO_GROUP GPIOB
#define MOTOR_B_IN1_GPIO_PIN   GPIO_PIN_4

#define MOTOR_B_IN2_GPIO_GROUP GPIOB
#define MOTOR_B_IN2_GPIO_PIN   GPIO_PIN_15

#define MOTOR_B_IN1_CHANNEL    TIM_CHANNEL_1
#define MOTOR_B_IN2_CHANNEL    TIM_CHANNEL_3

extern TIM_HandleTypeDef htim3;
static TIM_HandleTypeDef *g_HPWM_BIN1 = &htim3;

//extern TIM_HandleTypeDef htim1;
//static TIM_HandleTypeDef *g_HPWM_BIN2 = &htim1;


/**********************************************************************
 * 函数名：Motor_PWM_SetDuty
 * 功能描述：初始化Motor
 * 输入参数：channel - MOTOR_A 或 MOTOR_B
 * 输出参数：无
 * 返回值：无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
static void Motor_PWM_SetSpeed(int32_t speed)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfig = {0};
    
    if (speed == 0)
    {
        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(MOTOR_B_IN1_GPIO_GROUP, MOTOR_B_IN1_GPIO_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_GROUP, MOTOR_B_IN2_GPIO_PIN, GPIO_PIN_SET);

        /*Configure GPIO pins*/
        GPIO_InitStruct.Pin = MOTOR_B_IN1_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN1_GPIO_GROUP, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOTOR_B_IN2_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN2_GPIO_GROUP, &GPIO_InitStruct);        
    }
    else if (speed > 0)
    {
        /* B_IN1 设置为PWM,  B_IN2设置为输出并且输出0 */
        /* B_IN1 */

        /* 配置B_IN1为 PWM引脚 */
        GPIO_InitStruct.Pin = MOTOR_B_IN1_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN1_GPIO_GROUP, &GPIO_InitStruct);
        
        /* 配置B_IN1的PWM占空比 */
        
        sConfig.OCMode = TIM_OCMODE_PWM1;         // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
        sConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // 设置极性为高(硬件上BIN1为高,BIN2为低电平让motor正转)
        sConfig.OCFastMode = TIM_OCFAST_DISABLE;  // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
        sConfig.Pulse = speed*10;                 // 周期是1000, speed取值0~99

        HAL_TIM_PWM_Stop(g_HPWM_BIN1, MOTOR_B_IN1_CHANNEL);        
        HAL_TIM_PWM_ConfigChannel(g_HPWM_BIN1, &sConfig, MOTOR_B_IN1_CHANNEL);
        HAL_TIM_PWM_Start(g_HPWM_BIN1, MOTOR_B_IN1_CHANNEL);

        /* B_IN2 */
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_GROUP, MOTOR_B_IN2_GPIO_PIN, GPIO_PIN_RESET);
        GPIO_InitStruct.Pin = MOTOR_B_IN2_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN2_GPIO_GROUP, &GPIO_InitStruct);        
    }
    else
    {
        speed = 0 - speed;
        /* B_IN1 设置为PWM,  B_IN2设置为输出并且输出1 */
        /* B_IN1 */

        /* 配置B_IN1为 PWM引脚 */
        GPIO_InitStruct.Pin = MOTOR_B_IN1_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN1_GPIO_GROUP, &GPIO_InitStruct);
        
        /* 配置B_IN1的PWM占空比 */
        
        sConfig.OCMode = TIM_OCMODE_PWM1;         // PWM 输出的两种模式:PWM1 当极性为低,CCR<CNT,输出低电平,反之高电平
        sConfig.OCPolarity = TIM_OCPOLARITY_HIGH; // 设置极性为高(硬件上BIN2为高,BIN1为低电平让motor反转)
        sConfig.OCFastMode = TIM_OCFAST_DISABLE;  // 输出比较快速使能禁止(仅在 PWM1 和 PWM2 可设置)
        sConfig.Pulse = (99 - speed)*10;          // 周期是1000, speed取值0~99

        HAL_TIM_PWM_Stop(g_HPWM_BIN1, MOTOR_B_IN1_CHANNEL);        
        HAL_TIM_PWM_ConfigChannel(g_HPWM_BIN1, &sConfig, MOTOR_B_IN1_CHANNEL);
        HAL_TIM_PWM_Start(g_HPWM_BIN1, MOTOR_B_IN1_CHANNEL);

        /* B_IN2 */
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_GROUP, MOTOR_B_IN2_GPIO_PIN, GPIO_PIN_SET);
        GPIO_InitStruct.Pin = MOTOR_B_IN2_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN2_GPIO_GROUP, &GPIO_InitStruct);        
    }
}


/**********************************************************************
 * 函数名：Motor_Init
 * 功能描述：初始化Motor
 * 输入参数：channel - MOTOR_A 或 MOTOR_B
 * 输出参数：无
 * 返回值：无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void Motor_Init(int32_t channel)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    if (MOTOR_A == channel)
    {
        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_GROUP, MOTOR_A_IN1_GPIO_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_GROUP, MOTOR_A_IN2_GPIO_PIN, GPIO_PIN_SET);

        /*Configure GPIO pins*/
        GPIO_InitStruct.Pin = MOTOR_A_IN1_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_A_IN1_GPIO_GROUP, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOTOR_A_IN2_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_A_IN2_GPIO_GROUP, &GPIO_InitStruct);        
    }

    if (MOTOR_B == channel)
    {
        /*Configure GPIO pin Output Level */
        HAL_GPIO_WritePin(MOTOR_B_IN1_GPIO_GROUP, MOTOR_B_IN1_GPIO_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MOTOR_B_IN2_GPIO_GROUP, MOTOR_B_IN2_GPIO_PIN, GPIO_PIN_SET);

        /*Configure GPIO pins*/
        GPIO_InitStruct.Pin = MOTOR_B_IN1_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN1_GPIO_GROUP, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = MOTOR_B_IN2_GPIO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(MOTOR_B_IN2_GPIO_GROUP, &GPIO_InitStruct);        
    }
    
}

/**********************************************************************
 * 函数名：Motor_SetPpeed
 * 功能描述：设置马达速度
 * 输入参数：channel - MOTOR_A 或 MOTOR_B
 *           speed - 取值(-100~100),
 *                   0表示停止, 
 *                   正数表示顺时针旋转速度, 
 *                   负数表示逆时钟旋转速度
 * 输出参数：无
 * 返回值：无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/05        V1.0     韦东山       创建
 ***********************************************************************/
void Motor_SetSpeed(int32_t channel, int32_t speed)
{
    if (speed > 99)
        speed = 99;
    else if (speed < -99)
        speed = -99;
    
    if (MOTOR_A == channel)
    {
        if (0 == speed)
        {
            HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_GROUP, MOTOR_A_IN1_GPIO_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_GROUP, MOTOR_A_IN2_GPIO_PIN, GPIO_PIN_SET);
        }
        else if (0 < speed)
        {
            HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_GROUP, MOTOR_A_IN1_GPIO_PIN, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_GROUP, MOTOR_A_IN2_GPIO_PIN, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(MOTOR_A_IN1_GPIO_GROUP, MOTOR_A_IN1_GPIO_PIN, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_A_IN2_GPIO_GROUP, MOTOR_A_IN2_GPIO_PIN, GPIO_PIN_SET);
        }
    }

    if (MOTOR_B == channel)
    {
        Motor_PWM_SetSpeed(speed);
    }
}


/**********************************************************************
 * 函数名称： Motor_Test
 * 功能描述： Motor测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/03        V1.0     韦东山       创建
 ***********************************************************************/
void Motor_Test(void)
{
    int32_t speed = 0;
    int i, len;
    
    Motor_Init(MOTOR_A);
    Motor_Init(MOTOR_B);

    while (1)
    {
        LCD_PrintString(0, 0, "Motor Test: ");

        /* 正转 */
        speed = 0;
        for (i = 0; i < 10; i++)
        {
            len = LCD_PrintString(0, 2, "Speed:");
            len += LCD_PrintSignedVal(len, 2, speed);
            LCD_ClearLine(len, 2);

            Motor_SetSpeed(MOTOR_A, speed);
            Motor_SetSpeed(MOTOR_B, speed);

            speed += 10;
            mdelay(1000);
        }
		
        /* 停止 */
        speed = 0;
        Motor_SetSpeed(MOTOR_A, speed);
        Motor_SetSpeed(MOTOR_B, speed);
        len = LCD_PrintString(0, 2, "Speed:");
        len += LCD_PrintSignedVal(len, 2, speed);
        LCD_ClearLine(len, 2);
        mdelay(2000);

        /* 反转 */
        for (i = 0; i < 10; i++)
        {
            len = LCD_PrintString(0, 2, "Speed:");
            len += LCD_PrintSignedVal(len, 2, speed);
            LCD_ClearLine(len, 2);

            Motor_SetSpeed(MOTOR_A, speed);
            Motor_SetSpeed(MOTOR_B, speed);

            speed -= 10;
            mdelay(1000);
        }

        /* 停止 */
        speed = 0;
        Motor_SetSpeed(MOTOR_A, speed);
        Motor_SetSpeed(MOTOR_B, speed);
        len = LCD_PrintString(0, 2, "Speed:");
        len += LCD_PrintSignedVal(len, 2, speed);
        LCD_ClearLine(len, 2);
        mdelay(2000);		
    }
}

