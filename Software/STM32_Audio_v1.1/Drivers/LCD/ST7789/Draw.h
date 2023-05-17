/*
 * Draw.h
 *
 *  Created on: Dec 25, 2022
 *      Author: Duc Huy Le
 */

#ifndef LCD_ST7789_DRAW_H_
#define LCD_ST7789_DRAW_H_
#include "main.h"
#include "Setting.h"
typedef struct
{
  uint16_t X;
  uint16_t Y;
}PIXEL;

void Draw_Pixel(uint16_t x, uint16_t y, uint16_t color);
void Draw_CircleProgress(uint16_t x0, uint16_t y0, uint16_t r, uint16_t alpha);
void Draw_LineCircleProgress(uint16_t x0, uint16_t y0, uint16_t r, uint16_t width, uint8_t percent, uint16_t color);
void Draw_CreateLineCircleProgress(uint16_t x0, uint16_t y0, uint16_t r, uint16_t width, uint8_t percent, uint16_t color);
void Draw_CreateLineCircleProgressFast(uint16_t x0, uint16_t y0, uint16_t r, uint16_t width, uint8_t percent, uint16_t color);
void Test_Draw_CircleProgress(void);
void Draw_CreatePlayMusic(void);
void Draw_LineCircleProgressSmall(uint16_t x0, uint16_t y0, uint16_t r, uint16_t width, uint16_t small, uint8_t percent, uint16_t color);
void Draw_DigitalClock(uint16_t x, uint16_t y, uint16_t time_sec);
void Draw_BatteryLevel(uint16_t x, uint16_t y, uint8_t percent);
void Draw_BatteryCheckProhibiton(uint16_t x, uint16_t y);
void Draw_MusicNameFast(uint16_t num, const char* str);
void Draw_CreateListMusic(void);
void Draw_ListMusic(void);
void Draw_Volume(void);
void Draw_VolumeInvisible(void);
void Draw_CreateSetting(void);
void Draw_ListSetting(void);
void Draw_SettingIcons(uint16_t x, uint16_t y, __ELEMENT obj, uint16_t color, uint16_t bgcolor);
void Draw_SettingElement(uint16_t id);
#endif /* LCD_ST7789_DRAW_H_ */
