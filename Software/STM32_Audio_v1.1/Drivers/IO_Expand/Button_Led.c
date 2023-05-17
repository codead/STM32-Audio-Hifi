/*
 * Button_Led.c
 *
 *  Created on: Jul 25, 2022
 *      Author: Duc Huy Le
 */
#include "Button_Led.h"

#include "main.h"
#include <stdio.h>
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdbool.h>
#include <time.h>
#include "SX1509B.h"
#include "Utilities.h"
#include "Display.h"
#include "Draw.h"
#include "STC3100.h"
#include "Play_wav.h"
#include "Database.h"
extern AUDIO Audio;
extern DISPLAY Display;
extern STC3100_INFO Batt;
extern __SETTING Setting;
extern uint8_t SCREEN_MUSIC_MAX_LIST;
extern uint8_t BatteryStatus;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  uint8_t detectPin = 0;
  bool buttonPressed;
  if(GPIO_Pin == BAT_CHARGE_Pin)
  {

    if(HAL_GPIO_ReadPin(GPIOB, BAT_CHARGE_Pin) == 0)
    {
      printf("Battery is changing...\n");
    }
    else
    {
      printf("Battery is discharged...\n");
    }
    //STC3100_Get(&Batt, BATTERY_PROP_VOLTAGE_NOW);
    if(BatteryStatus == 1)
    {
      STC3100_Init();
      BatteryStatus = 2;
    }
    Draw_BatteryLevel(200, 5, BatteryPercentCalculation(Batt.voltage));
  }
  else if(GPIO_Pin == GPIO_PIN_2)
  {
    printf("Searching button...\n");
    buttonPressed = SX1509B_SearchInterruptPin(&detectPin);
    if(buttonPressed == true)
    {
      // update LCD back light
      if(Setting.Always_Display == false) // will turn off LCD after 20s
      {
        Display.LCDBackLightTime = 20;
        if(Display.LCDBacklightStatus == false)
        {
          Display.LCDBacklightStatus = true;
          LCD_BackLight_On();
          // clear after read
          (void)SX1509B_WriteRegSequence(RegInterruptSourceB, 0xFFFF);
          return; // the first time press button will do nothing except turn on back light
        }
      }
      if(detectPin == BTN_LEFT)
      {
        printf("Button BTN_LEFT is pressed\n");
        if(Display.Mode != CREATE_PLAY_MUSIC && Display.Mode != PLAY_MUSIC)
        {
          Display.Mode = CREATE_PLAY_MUSIC;
          Display.NeedToResetFullCircleProgress = true;
        }
        else if(Display.Mode == CREATE_PLAY_MUSIC || Display.Mode == PLAY_MUSIC)
        {
          Audio.PlaySong = false;
          Audio.EndSong = true;
          Audio.NewSongEvent = BACK_TO_PREVIOUS_SONG;
//          Display.NeedToUpdateListMusic = true;
        }
      }
      else if(detectPin == BTN_RIGHT)
      {
        printf("Button BTN_RIGHT is pressed\n");
        if(Display.Mode == CREATE_PLAY_MUSIC || Display.Mode == PLAY_MUSIC)
        {
          //Display.Mode = CREATE_PLAY_MUSIC;
          Audio.PlaySong = false;
          Audio.EndSong = true;
          Audio.NewSongEvent = NEXT_SONG;
        }
      }
      else if(detectPin == BTN_CENTRE)
      {
        printf("Button BTN_CENTRE is pressed\n");
        if(Display.Mode == LIST_MUSIC)
        {
//          Audio.CurrentSong = Display.CursorListMusic;
//          Database_SaveDataMusic();
          Audio.PlaySong = false;
          Audio.EndSong = true;
          Audio.NewSongEvent = SELECT_SONG;
          Display.NeedToUpdateListMusic = true;
        }
        else if(Display.Mode == SETTING)
        {
          Setting_ToggleEvent(Display.CursorLCDScreenSetting);
          Database_SaveDataSetting(Display.CursorLCDScreenSetting);
          Display.NeedToUpdateSetting = true;
        }
      }
      else if(detectPin == BTN_UP)
      {
        printf("Button BTN_UP is pressed\n");
//        if(Display.Mode != LIST_MUSIC && Display.Mode != CREATE_LIST_MUSIC)
        // Mode Playing Music
        if(Display.Mode == PLAY_MUSIC)
        {
          //Display.CursorListMusic = Audio.CurrentSong;
          Display.Mode = CREATE_LIST_MUSIC;
          Display.NeedToUpdateListMusic = true;
        }
        // Mode List Music
        else if(Display.Mode == LIST_MUSIC)
        {
          if(Display.CursorListMusic > 0)
          {
            Display.CursorListMusic--;
          }
          else
          {
            Display.CursorListMusic = Audio.MaxSong - 1; // roll back cursor to last of list
          }
          Display.CursorLCDScreenMusic--;
          if(Display.CursorLCDScreenMusic == 255)
          {
            Display.CursorLCDScreenMusic = 0; // cannot decrease cursor LCD
            ListLCDScreen_RollUp(); // roll Up list
          }
          Display.NeedToUpdateListMusic = true;
        }
        // Mode Setting
        else if(Display.Mode == SETTING)
        {

          Display.CursorLCDScreenSetting--;
          if(Display.CursorLCDScreenSetting == 255)
          {
            Display.CursorLCDScreenSetting = 0; // cannot decrease cursor LCD
            if(SETTING_MAX_ELEMENT > SCREEN_SETTING_MAX_LIST)
            {
              if(Display.CursorListSetting > 0) // if cursor not point to head list
              {
                ListLCDScreen_RollUp(); // roll up list
              }
            }
          }
          if(Display.CursorListSetting > 0)
          {
            Display.CursorListSetting--;
          }
          else
          {
            Display.CursorListSetting = 0; // stay in head of list
          }
          Display.NeedToUpdateSetting = true;
        }
      }
      else if(detectPin == BTN_DOWN)
      {
        printf("Button BTN_DOWN is pressed\n");
        // Mode List Music
        if(Display.Mode == LIST_MUSIC)
        {
          Display.CursorListMusic++;
          if(Display.CursorListMusic >= Audio.MaxSong)
          {
            Display.CursorListMusic = 0;// roll back cursor to head of list
          }
          Display.CursorLCDScreenMusic++;
          if(Display.CursorLCDScreenMusic >= SCREEN_MUSIC_MAX_LIST)
          {
            Display.CursorLCDScreenMusic = SCREEN_MUSIC_MAX_LIST - 1; // // cannot increase cursor LCD
            ListLCDScreen_RollDown();
          }
          Display.NeedToUpdateListMusic = true;
        }
        else if(Display.Mode == PLAY_MUSIC)
        {
          Display.Mode = CREATE_SETTING;
          Display.NeedToUpdateSetting = true;
        }
        // Mode Setting
        else if(Display.Mode == SETTING)
        {
          Display.CursorListSetting++;
          Display.CursorLCDScreenSetting++;
          if(Display.CursorLCDScreenSetting >= SCREEN_SETTING_MAX_LIST)
          {
            Display.CursorLCDScreenSetting = SCREEN_SETTING_MAX_LIST - 1; // cannot increase cursor LCD
            if(SETTING_MAX_ELEMENT > SCREEN_SETTING_MAX_LIST)
            {
              if(Display.CursorListSetting < SETTING_MAX_ELEMENT) // if cursor not point to last of list
              {
                ListLCDScreen_RollDown();
              }
            }
          }
          if(Display.CursorListSetting >= SETTING_MAX_ELEMENT)
          {
            Display.CursorListSetting = SETTING_MAX_ELEMENT - 1; // stay cursor at last of list
          }
          Display.NeedToUpdateSetting = true;
        }
      }
      else
      {
        printf("Hardware is error\n");
      }
    }
    // clear after read
    (void)SX1509B_WriteRegSequence(RegInterruptSourceB, 0xFFFF);
  }
  else
  {
    // reverse
  }
}
