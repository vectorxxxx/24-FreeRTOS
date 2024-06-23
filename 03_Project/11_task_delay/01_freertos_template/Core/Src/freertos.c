/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static StackType_t g_puxStackOfLightTask[128];
static StaticTask_t g_TCBOfLightTask;
static TaskHandle_t xLightTaskHandle;

static StackType_t g_puxStackOfColorTask[128];
static StaticTask_t g_TCBOfColorTask;
static TaskHandle_t xColorTaskHandle;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

struct TaskPrintInfo {
    uint8_t x;
    uint8_t y;
    char name[16];
};

static struct TaskPrintInfo g_Task1Info = {0, 0, "Task1:"};
static struct TaskPrintInfo g_Task2Info = {0, 3, "Task2:"};
static struct TaskPrintInfo g_Task3Info = {0, 6, "Task3:"};
static uint8_t g_LCDCanUse = 1;

void LCDPrintTask(void *params)
{
    struct TaskPrintInfo *pInfo = params;
    uint32_t cnt = 0;
    int len = 0;

    uint64_t t1, t2;
    TickType_t preTime = xTaskGetTickCount(); 
    
    while(1)
    {
        if (g_LCDCanUse)
        {
            g_LCDCanUse = 0;
            len = LCD_PrintString(pInfo->x, pInfo->y, pInfo->name);
            LCD_PrintSignedVal(len, pInfo->y, cnt++);
            g_LCDCanUse = 1;
            
            // 随机延迟一段时间
            mdelay(cnt & 0x3);
        }
        
        t1 = system_get_ns();
        vTaskDelay(500);    // 500000000ns
        //vTaskDelayUntil(&preTime, 500);
        t2 = system_get_ns();
        LCD_ClearLine(pInfo->x, pInfo->y + 2);
        LCD_PrintSignedVal(pInfo->x, pInfo->y + 2, t2 - t1);
    }
}

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  TaskHandle_t pxSoundTaskHandle;
  BaseType_t xSoundTaskType;

  LCD_Init();
  LCD_Clear();
  
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  //defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  /* 声 */
//  extern void PlayMusic(void *params);
//  xSoundTaskType = xTaskCreate(PlayMusic, "SoundTask", 128, NULL, osPriorityNormal, &pxSoundTaskHandle);
  
  /* 光 */
//  xLightTaskHandle = xTaskCreateStatic(Led_Test, "LightTask", 128, NULL, osPriorityNormal, g_puxStackOfLightTask, &g_TCBOfLightTask);
  
  /* 色 */
//  xColorTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorTask", 128, NULL, osPriorityNormal, g_puxStackOfColorTask, &g_TCBOfColorTask);
  
  xTaskCreate(LCDPrintTask, "task1", 128, &g_Task1Info, osPriorityNormal, NULL);
  //xTaskCreate(LCDPrintTask, "task2", 128, &g_Task2Info, osPriorityNormal, NULL);
  //xTaskCreate(LCDPrintTask, "task3", 128, &g_Task3Info, osPriorityNormal, NULL);

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */

/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  LCD_Init();
  LCD_Clear();
  
  for(;;)
  {
    //Led_Test();
    //LCD_Test();
	//MPU6050_Test(); 
	//DS18B20_Test();
	//DHT11_Test();
	//ActiveBuzzer_Test();
	//PassiveBuzzer_Test();
	//ColorLED_Test();
	IRReceiver_Test();
	//IRSender_Test();
	//LightSensor_Test();
	//IRObstacle_Test();
	//SR04_Test();
	//W25Q64_Test();
	//RotaryEncoder_Test();
	//Motor_Test();
	//Key_Test();
	//UART_Test();
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

