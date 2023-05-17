/*
 * Display.c
 *
 *  Created on: Jan 2, 2023
 *      Author: Duc Huy Le
 */

#include "Display.h"
#include "ST7789.h"
#include "fonts.h"
#include "FontsImage.h"
#include "Draw.h"
#include "tim.h"
#include "Play_wav.h"
#include "Display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "STC3100.h"
#include "Utilities.h"
volatile DISPLAY Display;
extern wavctrl WaveCtrlData;
extern osThreadId defaultTaskHandle;
extern osThreadId playAudioTaskHandle;
extern osThreadId batteryMonitoringTaskHandle;
extern osThreadId updateScreenLCDTaskHandle;
extern osSemaphoreId binSemaphoreDisplayHandle;
extern STC3100_INFO Batt;
//extern uint16_t Audio.MaxSong;
extern AUDIO Audio;
uint8_t SCREEN_MUSIC_MAX_LIST = 0;
void LCD_BackLight_On(void)
{
  ST7789_BACKLIGHT_On();
}
void LCD_BackLight_Off(void)
{
  ST7789_BACKLIGHT_Off();
}
void ListMusic_Init(void)
{
  bool need_roll_back = false;
  uint8_t i, j;
  Display.CursorListMusic = Audio.CurrentSong;
  Display.CursorLCDScreenMusic = 0;
  Display.LCDBacklightStatus = true;
  Display.LCDBackLightTime = 20;
  Display.Mode = CREATE_PLAY_MUSIC;
  for(i = 0; i < SCREEN_MUSIC_MAX_LIST; i++)
  {
    Display.ListLCDScreenMusic[i] = Display.CursorListMusic + i;
    if(Display.CursorListMusic + i > Audio.MaxSong - 1)
    {
      need_roll_back = true;
      break;
    }
  }
  if(need_roll_back == true)
  {
    for(j = i; j < SCREEN_MUSIC_MAX_LIST; j++)
    {
      Display.ListLCDScreenMusic[j] = j - i;
    }
  }
  Audio.PreviousSong = Audio.CurrentSong;
  Audio_CreatePreviousSongList();
}
void ListLCDScreen_RollDown(void)
{
  if(Display.Mode == LIST_MUSIC)
  {
    for(uint8_t i = 0; i < SCREEN_MUSIC_MAX_LIST; i++)
    {
      if(i < SCREEN_MUSIC_MAX_LIST - 1)
      {
        Display.ListLCDScreenMusic[i] = Display.ListLCDScreenMusic[i + 1];
      }
      else
      {
        Display.ListLCDScreenMusic[i]++;
        if(Display.ListLCDScreenMusic[i] >= Audio.MaxSong)
        {
          Display.ListLCDScreenMusic[i] = 0;
        }
      }
    }
  }
  else if(Display.Mode == SETTING)
  {
    for(uint8_t i = 0; i < SCREEN_SETTING_MAX_LIST; i++)
    {
      if(i < SCREEN_SETTING_MAX_LIST - 1)
      {
        Display.ListLCDScreenSetting[i] = Display.ListLCDScreenSetting[i + 1];
      }
      else
      {
        Display.ListLCDScreenSetting[i]++;
        if(Display.ListLCDScreenSetting[i] >= SETTING_MAX_ELEMENT)
        {
          Display.ListLCDScreenSetting[i] = SETTING_MAX_ELEMENT - 1; // keep cursor setting to last element
        }
      }
    }
  }
}
void ListLCDScreen_RollUp(void)
{
  if(Display.Mode == LIST_MUSIC)
  {
    for(int8_t i = SCREEN_MUSIC_MAX_LIST - 1 ; i > -1; i--)
    {
      if(i > 0)
      {
        Display.ListLCDScreenMusic[i] = Display.ListLCDScreenMusic[i - 1];
      }
      else
      {
        if(Display.ListLCDScreenMusic[i] == 0)
        {
          Display.ListLCDScreenMusic[i] = Audio.MaxSong - 1;
        }
        else
        {
          Display.ListLCDScreenMusic[i]--;
        }
      }
    }
  }
  else if(Display.Mode == SETTING)
  {
    for(int8_t i = SCREEN_SETTING_MAX_LIST - 1 ; i > -1; i--)
    {
      if(i > 0)
      {
        Display.ListLCDScreenSetting[i] = Display.ListLCDScreenSetting[i - 1];
      }
      else
      {
        if(Display.ListLCDScreenSetting[i] == 0)
        {
          Display.ListLCDScreenSetting[i] = 0;
        }
        else
        {
          Display.ListLCDScreenSetting[i]--;
        }
      }
    }
  }
}
void Display_Update(void)
{
  switch(Display.Mode)
  {
    case MAIN:
      break;
    case INITIALIZE:
      break;
    case CREATE_LIST_MUSIC:
      Draw_CreateListMusic();
      Draw_BatteryLevel(200, 5, BatteryPercentCalculation(Batt.voltage));
      Display.Mode = LIST_MUSIC;
    case LIST_MUSIC:
      if(Display.NeedToUpdateListMusic == true)
      {
        osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
        Draw_ListMusic();
        osSemaphoreRelease(binSemaphoreDisplayHandle);
        Display.NeedToUpdateListMusic = false;
      }
      break;
    case CREATE_PLAY_MUSIC:
      //osThreadSuspend(playAudioTaskHandle);
      osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
      Draw_CreatePlayMusic();
      Draw_BatteryLevel(200, 5, BatteryPercentCalculation(Batt.voltage));

      /*Start IT Timer */
//      HAL_TIM_Base_Start_IT(&htim2);
      //ST7789_DrawImage(0, 0, 23, 43, image_data_Font_0x38);
      Display.Mode = PLAY_MUSIC;
      osSemaphoreRelease(binSemaphoreDisplayHandle);
      //osThreadResume(playAudioTaskHandle);
    case PLAY_MUSIC:
      if(Display.NeedToUpdateCurrentTime == true)
      {
        //osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
        Draw_LineCircleProgressSmall(120, 100, 90, 20, 4, Display.CurrentTime * 100 / Display.DurationTime, GREEN);
        Draw_DigitalClock(120, 100 - 2, Display.CurrentTime);
        //osSemaphoreRelease(binSemaphoreDisplayHandle);
        Display.NeedToUpdateCurrentTime = false;
      }
      break;
    case CREATE_SETTING:
      Draw_CreateSetting();
      Draw_BatteryLevel(200, 5, BatteryPercentCalculation(Batt.voltage));
      Display.Mode = SETTING;
    case SETTING:
      if(Display.NeedToUpdateSetting == true)
      {
        osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
        Draw_ListSetting();
        osSemaphoreRelease(binSemaphoreDisplayHandle);
        Display.NeedToUpdateSetting = false;
      }
      break;
    default:
      break;
  }
}
