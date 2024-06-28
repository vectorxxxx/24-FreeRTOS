/*  Copyright (s) 2023 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_mpu6050.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2023.08.03      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "stm32f1xx_hal.h"
#include "driver_mpu6050.h"
#include "driver_lcd.h"
#include "driver_timer.h"
#include <math.h>

//****************************************
// 定义MPU6050内部地址
//****************************************
#define	MPU6050_SMPLRT_DIV		0x19  // 陀螺仪采样率，典型值：0x07(125Hz)
#define	MPU6050_CONFIG			0x1A  // 低通滤波频率，典型值：0x06(5Hz)
#define	MPU6050_GYRO_CONFIG		0x1B  // 陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	MPU6050_ACCEL_CONFIG	0x1C  // 加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B //电源管理，典型值：0x00(正常启用)
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75 //IIC地址寄存器(默认数值0x68，只读)


#define MPU6050_I2C_ADDR     0xD0
#define MPU6050_TIMEOUT     500

/* 传感器数据修正值（消除芯片固定误差，根据硬件进行调整） */
#define MPU6050_X_ACCEL_OFFSET	(-64) 
#define MPU6050_Y_ACCEL_OFFSET 	(-30)
#define MPU6050_Z_ACCEL_OFFSET 	(14400) 
#define MPU6050_X_GYRO_OFFSET 	(40)
#define MPU6050_Y_GYRO_OFFSET 	(-7)
#define MPU6050_Z_GYRO_OFFSET 	(-14)

extern I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef *g_pHI2C_MPU6050 = &hi2c1;

/* MPU6050队列 */
static QueueHandle_t g_xQueueMPU6050;
QueueHandle_t GetQueueMPU6050(void)
{
    return g_xQueueMPU6050;
}

/**********************************************************************
 * 函数名称： MPU6050_WriteRegister
 * 功能描述： 写MPU6050寄存器
 * 输入参数： reg-寄存器地址, data-要写入的数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int MPU6050_WriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;
    
    return HAL_I2C_Master_Transmit(g_pHI2C_MPU6050, MPU6050_I2C_ADDR, tmpbuf, 2, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_ReadRegister
 * 功能描述： 读MPU6050寄存器
 * 输入参数： reg-寄存器地址
 * 输出参数： pdata-用来保存读出的数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_ReadRegister(uint8_t reg, uint8_t *pdata)
{
	return HAL_I2C_Mem_Read(g_pHI2C_MPU6050, MPU6050_I2C_ADDR, reg, 1, pdata, 1, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_Init
 * 功能描述： MPU6050初始化函数,
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_Init(void)
{
	MPU6050_WriteRegister(MPU6050_PWR_MGMT_1, 0x00);	//解除休眠状态
	MPU6050_WriteRegister(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteRegister(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_WriteRegister(MPU6050_CONFIG, 0x06);
	MPU6050_WriteRegister(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteRegister(MPU6050_ACCEL_CONFIG, 0x18);
    
    // 创建队列
    g_xQueueMPU6050 = xQueueCreate(MPU6050_QUEUE_LEN, sizeof(struct mpu6050_data));

    return 0;
}

/**********************************************************************
 * 函数名称： MPU6050_GetID
 * 功能描述： 读取MPU6050 ID
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1 - 失败, 其他值 - ID
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_GetID(void)
{
    uint8_t id;
	if(0 == MPU6050_ReadRegister(MPU6050_WHO_AM_I, &id))
        return id;
    else
        return -1;
}



/**********************************************************************
 * 函数名称： MPU6050_ReadData
 * 功能描述： 读取MPU6050数据
 * 输入参数： 无
 * 输出参数： pAccX/pAccY/pAccZ         - 用来保存X/Y/Z轴的加速度
 *            pGyroX/pGyroY/pGyroZ - 用来保存X/Y/Z轴的角速度
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_ReadData(int16_t *pAccX, int16_t *pAccY, int16_t *pAccZ, int16_t *pGyroX, int16_t *pGyroY, int16_t *pGyroZ)
{
	uint8_t datal, datah;
    int err = 0;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_XOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_XOUT_L, &datal);
	if(pAccX)
        *pAccX = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_YOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_YOUT_L, &datal);
	if(pAccY)
        *pAccY = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_ZOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_ZOUT_L, &datal);
	if(pAccZ)
        *pAccZ = (datah << 8) | datal;


	err |= MPU6050_ReadRegister(MPU6050_GYRO_XOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_XOUT_L, &datal);
	if(pGyroX)
        *pGyroX = (datah << 8) | datal;

	
	err |= MPU6050_ReadRegister(MPU6050_GYRO_YOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_YOUT_L, &datal);
	if(pGyroY)
        *pGyroY = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_GYRO_ZOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_ZOUT_L, &datal);
	if(pGyroZ)
        *pGyroZ = (datah << 8) | datal;

    return err;	
}


/**********************************************************************
 * 函数名称： MPU6050_ParseData
 * 功能描述： 解析MPU6050数据
 * 输入参数： AccX/AccY/AccZ/GyroX/GyroY/GyroZ
 *            X/Y/Z轴的加速度,X/Y/Z轴的角速度
 * 输出参数： result - 用来保存计算出的结果,目前仅支持X方向的角度
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/09/05	     V1.0	  韦东山	      创建
 ***********************************************************************/
void MPU6050_ParseData(int16_t AccX, int16_t AccY, int16_t AccZ, int16_t GyroX, int16_t GyroY, int16_t GyroZ, struct mpu6050_data *result)
{
	if (result)
	{
		result->angle_x = (int32_t)(acos((double)((double)(AccX + MPU6050_X_ACCEL_OFFSET) / 16384.0)) * 57.29577);
	}
}

/**********************************************************************
 * 函数名称： MPU6050_Test
 * 功能描述： MPU6050测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
void MPU6050_Test(void)
{
    int id;
    int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
    int len;
    struct mpu6050_data result;
 
    MPU6050_Init();
    
    id = MPU6050_GetID();
    LCD_PrintString(0, 0, "MPU6050 ID:");
    LCD_PrintHex(0, 2, id, 1);

    mdelay(1000);
    LCD_Clear();

    while (1)
    {    
        MPU6050_ReadData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
        MPU6050_ParseData(AccX, AccY, AccZ, GyroX, GyroY, GyroZ, &result);

        LCD_PrintString(0, 0, "X: ");
        LCD_PrintSignedVal(3, 0,  acos((double)((double)(AccX + MPU6050_X_ACCEL_OFFSET) / 16384.0)) * 57.29577);  			
        LCD_PrintSignedVal(10, 0, GyroX + MPU6050_X_GYRO_OFFSET); 

        LCD_PrintString(0, 2, "Y: ");
        LCD_PrintSignedVal(3, 2,  acos((double)((double)(AccY + MPU6050_Y_ACCEL_OFFSET) / 16384.0)) * 57.29577);
        LCD_PrintSignedVal(10, 2, GyroY + MPU6050_Y_GYRO_OFFSET);    

        LCD_PrintString(0, 4, "Z: ");
        LCD_PrintSignedVal(3, 4,  acos((double)((double)(AccZ + MPU6050_Z_ACCEL_OFFSET) / 16384.0)) * 57.29577);
        LCD_PrintSignedVal(10, 4, GyroZ + MPU6050_Z_GYRO_OFFSET);   

        len = LCD_PrintString(0, 6, "AngleX: ");
		LCD_PrintSignedVal(len, 6, result.angle_x);	   

		mdelay(100);
        LCD_Clear();
    }
}

void MPU6050_Task(void)
{
    int16_t AccX;
    struct mpu6050_data result;
    int ret;
    extern volatile int bInUsed;
    
    while(1)
    {
        // 读取数据
        while (bInUsed);
        bInUsed = 1;
        ret = MPU6050_ReadData(&AccX, NULL, NULL, NULL, NULL, NULL);
        bInUsed = 0;

        if (0 == ret)
        {
            // 解析数据
            MPU6050_ParseData(AccX, 0, 0, 0, 0, 0, &result);

            // 写入队列
            xQueueSend(g_xQueueMPU6050, &result, 0);
        }
        vTaskDelay(50);
    }
}
