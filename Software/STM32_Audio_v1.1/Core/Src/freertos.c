/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "WM8960.h"
#include "Play_wav.h"
#include "STC3100.h"
#include "Display.h"
#include "Draw.h"
#include "Utilities.h"
#include "SX1509B.h"
#include "IO_Expand.h"
#include "Button_Led.h"
//#include "../../TouchGFX/App/app_touchgfx.h"
//#include "../../TouchGFX/target/TouchGFX_DataTransfer.h"
#include <stdio.h>
#include "Setting.h"
#include "Database.h"
#include "RingBuffer_LIFO.h"
extern DISPLAY Display;
extern __SETTING Setting;
extern AUDIO Audio;
extern uint8_t BatteryStatus;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//#define USE_DEFAULT_TASK
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern STC3100_INFO Batt;
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osSemaphoreId binSemaphoreDisplayHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId playAudioTaskHandle;
osThreadId batteryMonitoringTaskHandle;
osThreadId updateScreenLCDTaskHandle;
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
void StartPlayAudioTask(void const * argument);
void StartBatteryMonitoringTask(void const * argument);
void StartUpdateScreenLCDTask(void const * argument);
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of binSemaphoreDisplay */
  osSemaphoreDef(binSemaphoreDisplay);
  binSemaphoreDisplayHandle = osSemaphoreCreate(osSemaphore(binSemaphoreDisplay), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
#ifdef USE_DEFAULT_TASK
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
#endif
  /* add threads, ... */
  osThreadDef(batteryMonitoringTask, StartBatteryMonitoringTask, osPriorityNormal, 1, 1 * 1024);
  batteryMonitoringTaskHandle = osThreadCreate(osThread(batteryMonitoringTask), NULL);
  /* add threads, ... */
  osThreadDef(playAudioTask, StartPlayAudioTask, osPriorityNormal, 1, 2 * 1024);
  playAudioTaskHandle = osThreadCreate(osThread(playAudioTask), NULL);
  /* add threads, ... */
  osThreadDef(updateScreenLCDTask, StartUpdateScreenLCDTask, osPriorityNormal, 1, 1 * 1024);
  updateScreenLCDTaskHandle = osThreadCreate(osThread(updateScreenLCDTask), NULL);

  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
#ifdef USE_DEFAULT_TASK
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  for(;;)
  {

//    SX1509B_ConfigPinMode(LED_CENTRE, EXTIO_OUTPUT_BREATHE, LOW);
//    SX1509B_WritePin(LED_CENTRE, LOW);
    printf("1. Default task running...\n");
    //MX_TouchGFX_Process();
    osDelay(20000);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#endif
void StartUpdateScreenLCDTask(void const * argument)
{
  for(;;)
  {
    //printf("4. Update screen LCD task running...\n");
    Display_Update();
    //MX_TouchGFX_Process();
    osDelay(10);
  }
}
void StartPlayAudioTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    printf("2. Play Audio task running...\n");
    if(Display.Mode == INITIALIZE || Display.Mode == CREATE_PLAY_MUSIC || Display.Mode == PLAY_MUSIC)
    {
      osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
      Display.Mode = CREATE_PLAY_MUSIC;
      osSemaphoreRelease(binSemaphoreDisplayHandle);
    }
    PlayWaveFile();
    osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
    if(Audio.NewSongEvent != BACK_TO_PREVIOUS_SONG) // Update if event is not back to previous song
    {
      Audio.PreviousSong = Audio.CurrentSong;// Save the previous song
      Audio_AddPreviousSong(); // add to previous list
    }
    else
    {
      Audio_GetPreviousSong(); // get the latest previous song
      Audio.CurrentSong = Audio.PreviousSong;
    }
    // Check event for new song
    if(Audio.NewSongEvent == SELECT_SONG)// If User select song
    {
      Audio.CurrentSong = Display.CursorListMusic;
      Audio.NewSongEvent = AUTO;
    }
    else if(Audio.NewSongEvent == NEXT_SONG) // If User want to next song
    {
      if(Setting.Music_RandomSong == true) // If setting random song
      {
        if(Audio.MaxSong)
        {
          Audio.CurrentSong = RandomVariableWithoutValue(0, Audio.MaxSong, Audio.CurrentSong); // random song
        }
      }
      else // If setting next +1 song
      {
        Audio.CurrentSong++;// point to next song
        if(Audio.CurrentSong >= Audio.MaxSong)
        {
          Audio.CurrentSong = 0;
        }
      }
      Audio.NewSongEvent = AUTO;
    }
    else if(Audio.NewSongEvent == AUTO)// If setting NewSongEvent == AUTO
    {
      if(Setting.Music_LoopOneSong == true)// If auto and loop one song
      {
        // nothing to do, dont need to point to next song
      }
      else// If just auto
      {
        if(Setting.Music_RandomSong == true) // If setting random song
        {
          if(Audio.MaxSong)
          {
            Audio.CurrentSong = RandomVariableWithoutValue(0, Audio.MaxSong, Audio.CurrentSong); // random song
          }
        }
        else //
        {
          Audio.CurrentSong++;// point to next song
          if(Audio.CurrentSong >= Audio.MaxSong)
          {
            Audio.CurrentSong = 0;
          }
        }
      }
    }
    else // If setting NewSongEvent == BACK_TO_PREVIOUS_SONG
    {
      // No need update because This is handle by above
    }
    if(Display.Mode == LIST_MUSIC)
    {
      Display.NeedToUpdateListMusic = true;// Update list music
    }
    Database_SaveDataMusic();
    osSemaphoreRelease(binSemaphoreDisplayHandle);

    osDelay(100);
  }

  /* USER CODE END StartDefaultTask */
}
void StartBatteryMonitoringTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  HAL_StatusTypeDef res = HAL_OK;
  /* Infinite loop */
  for(;;)
  {
    printf("3. Battery monitoring task running...\n");
    res = STC3100_Get(&Batt, BATTERY_PROP_VOLTAGE_NOW);
#ifdef BATTERY_MONITORING_CURRENT
    res |= STC3100_Get(&Batt, BATTERY_PROP_CURRENT_NOW);
    res |= STC3100_Get(&Batt, BATTERY_PROP_UNIQUEID);
#endif
    if(res == HAL_OK)
    {
      //printf("STC3100 ID = %d\n", Batt.ids[0]);
      printf("Battery voltage = %f, current = %f\n", Batt.voltage , Batt.current);
      Draw_BatteryLevel(200, 5, BatteryPercentCalculation(Batt.voltage));
    }
    else
    {
      BatteryStatus = 1;
    }
    osDelay(10000);
  }

  /* USER CODE END StartDefaultTask */
}
/* USER CODE END Application */
