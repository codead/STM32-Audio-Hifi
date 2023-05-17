/*
 * Setting.c
 *
 *  Created on: Feb 19, 2023
 *      Author: Duc Huy Le
 */
#include "Setting.h"
#include <stdio.h>
#include <stdbool.h>
#include "Display.h"
__SETTING Setting;
extern DISPLAY Display;
__ICONS Toggle_Button_On_Icons =
{
    50, 36, Toggle_Button_On_Data
};
__ICONS Toggle_Button_Off_Icons =
{
    50, 36, Toggle_Button_Off_Data
};
void Setting_Init(void)
{
  // setting event, will be overwrited by read flash
  Setting.Music_RandomSong = true;
  Setting.Music_LoopOneSong = false;
  Setting.Always_Display = true;
  // object 1
  Setting.Object[0].ID = 0;
  Setting.Object[0].Event = &Setting.Music_RandomSong;
  Setting.Object[0].Name = "Random Songs";
  if(*((bool*)(Setting.Object[0].Event)) == true)
  {
    Setting.Object[0].Icons = &Toggle_Button_On_Icons;
  }
  else
  {
    Setting.Object[0].Icons = &Toggle_Button_Off_Icons;
  }
  // object 2
  Setting.Object[1].ID = 1;
  Setting.Object[1].Event = &Setting.Music_LoopOneSong;
  Setting.Object[1].Name = "Repeat Songs";
  if(*((bool*)(Setting.Object[1].Event)) == true)
  {
    Setting.Object[1].Icons = &Toggle_Button_On_Icons;
  }
  else
  {
    Setting.Object[1].Icons = &Toggle_Button_Off_Icons;
  }
  // object 3
  Setting.Object[2].ID = 2;
  Setting.Object[2].Event = &Setting.Always_Display;
  Setting.Object[2].Name = "Always Display";
  if(*((bool*)(Setting.Object[2].Event)) == true)
  {
    Setting.Object[2].Icons = &Toggle_Button_On_Icons;
  }
  else
  {
    Setting.Object[2].Icons = &Toggle_Button_Off_Icons;
  }
  for(uint8_t i = 0; i < SETTING_MAX_ELEMENT; i++)
  {
    Setting.Object[i].ID = i;
  }
//  // object 3
//  Setting.Object[3].ID = 3;
//  // object 4
//  Setting.Object[4].ID = 4;
//  // object 5
//  Setting.Object[5].ID = 5;
//  // object 6
//  Setting.Object[6].ID = 6;
  for(uint8_t i = 0; i < SCREEN_SETTING_MAX_LIST; i++)
  {

    Display.ListLCDScreenSetting[i] = Setting.Object[i].ID;

  }
}
void Setting_ToggleEvent(uint8_t id)
{
  if(id >= SETTING_MAX_ELEMENT)
  {
    printf("Out of range\n");
    return;
  }
  else
  {
    if(*((bool*)(Setting.Object[id].Event)) == true)
    {
      *((bool*)(Setting.Object[id].Event)) = false;
//      Setting.Object[id].Icons = &Toggle_Button_Off_Icons;
    }
    else
    {
      *((bool*)(Setting.Object[id].Event)) = true;
//      Setting.Object[id].Icons = &Toggle_Button_On_Icons;
    }
    Setting_SyncEvent(id);
//    printf("Setting.Music_RandomSong = %d\n", Setting.Music_RandomSong);
//    printf("Setting.Music_LoopOneSong = %d\n", Setting.Music_LoopOneSong);
//    printf("Setting.Always_Display = %d\n", Setting.Always_Display);
  }
}
void Setting_SyncEvent(uint8_t id)
{
  if(id >= SETTING_MAX_ELEMENT)
  {
    printf("Out of range\n");
    return;
  }
  else
  {
    if(*((bool*)(Setting.Object[id].Event)) == true)
    {
      Setting.Object[id].Icons = &Toggle_Button_On_Icons;
    }
    else
    {
      Setting.Object[id].Icons = &Toggle_Button_Off_Icons;
    }
  }
}
