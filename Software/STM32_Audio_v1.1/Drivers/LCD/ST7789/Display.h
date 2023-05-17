/*
 * Display.h
 *
 *  Created on: Jan 2, 2023
 *      Author: Duc Huy Le
 */

#ifndef LCD_ST7789_DISPLAY_H_
#define LCD_ST7789_DISPLAY_H_

#include <stdio.h>
#include <stdbool.h>
#include "Setting.h"
#define SCREEN_MAX_LIST 5
#define SCREEN_SETTING_MAX_LIST ((SETTING_MAX_ELEMENT <= SCREEN_MAX_LIST) ? (SETTING_MAX_ELEMENT) : (SCREEN_MAX_LIST))
typedef enum
{
  MAIN = 0,
  INITIALIZE,
  CREATE_PLAY_MUSIC,
  PLAY_MUSIC,
  CREATE_LIST_MUSIC,
  LIST_MUSIC,
  CREATE_SETTING,
  SETTING,

}LCD_MODE;
typedef struct
{
  // LCD Mode
  LCD_MODE Mode;
  // Music List
  bool NeedToUpdateListMusic;
  uint32_t CurrentTime;
  uint16_t CursorListMusic;
  uint8_t  CursorLCDScreenMusic;
  uint16_t ListLCDScreenMusic[SCREEN_MAX_LIST];
  // Music Playing
  volatile uint32_t DurationTime;
  bool NeedToUpdateCurrentTime;
  bool NeedToResetFullCircleProgress;
  // Music Volume
  uint16_t TimeForDisplayVolume;
  bool NeedToInvisibleVolume;
  // Setting
  bool NeedToUpdateSetting;
  uint8_t CursorListSetting;
  uint8_t CursorLCDScreenSetting;
  uint8_t ListLCDScreenSetting[SCREEN_MAX_LIST];
  // Backlight
  bool LCDBacklightStatus;
  uint8_t LCDBackLightTime;
}DISPLAY;

void Display_Update(void);
void LCD_BackLight_On(void);
void LCD_BackLight_Off(void);
void ListMusic_Init(void);
void ListLCDScreen_RollUp(void);
void ListLCDScreen_RollDown(void);
#endif /* LCD_ST7789_DISPLAY_H_ */
