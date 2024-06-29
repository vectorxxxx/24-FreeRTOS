/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_ir_receiver.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_light_sensor.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "adc.h"


#define LIGHT_SENSOR_ADC_TIMEOUT 500

extern ADC_HandleTypeDef hadc1;
static ADC_HandleTypeDef *g_HADC_LightSensor = &hadc1;

/**********************************************************************
 * 函数名称： LightSensor_Init
 * 功能描述： 光敏电阻的初始化函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void LightSensor_Init(void)
{
    /* PA3用作ADC(ADC1_IN3) */
    HAL_ADCEx_Calibration_Start(g_HADC_LightSensor);

}

/**********************************************************************
 * 函数名称： LightSensor_Read
 * 功能描述： 读取光敏电阻的ADC值
 * 输入参数： 无
 * 输出参数： pData - 用来保存ADC值
 * 返 回 值： 0 - 成功, (-1) - 失败
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
int LightSensor_Read(uint32_t *pData)
{
     HAL_ADC_Start(g_HADC_LightSensor);
     if (HAL_OK == HAL_ADC_PollForConversion(g_HADC_LightSensor, LIGHT_SENSOR_ADC_TIMEOUT))
     {
        *pData = HAL_ADC_GetValue(g_HADC_LightSensor);
        return 0;
     }
     else
     {
        return -1;
     }
}


/**********************************************************************
 * 函数名称： LightSensor_Test
 * 功能描述： 光敏电阻测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 无
 * 修改日期        版本号     修改人        修改内容
 * -----------------------------------------------
 * 2023/08/04        V1.0     韦东山       创建
 ***********************************************************************/
void LightSensor_Test(void)
{
    uint32_t val;
    
    LightSensor_Init();

    while (1)
    {
        
        LCD_PrintString(0, 0, "Light Sensor: ");

        if (!LightSensor_Read(&val))
        {
            LCD_PrintSignedVal(0, 2, val);
        }
        
        mdelay(500);
    }
}


