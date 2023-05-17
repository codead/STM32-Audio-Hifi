/*
 * Setting.h
 *
 *  Created on: Feb 19, 2023
 *      Author: Duc Huy Le
 */

#ifndef SETTING_SETTING_H_
#define SETTING_SETTING_H_
#include <stdio.h>
#include <stdbool.h>
#include "Icons.h"
//extern const uint16_t Toggle_Button_On_Data[];
//extern const uint16_t Toggle_Button_Off_Data[];
#define SETTING_MAX_ELEMENT 3
typedef struct
{
  uint16_t Width;
  uint16_t Height;
  const uint16_t *Pixel;
}__ICONS;
typedef struct
{
  uint8_t ID;
  char *Name;
  __ICONS *Icons;
  volatile void *Event;
}__ELEMENT;

typedef struct
{
  volatile bool Music_RandomSong;
  volatile bool Music_LoopOneSong;
  volatile bool Always_Display;
  __ELEMENT Object[SETTING_MAX_ELEMENT];
}__SETTING;


void Setting_Init(void);
void Setting_ToggleEvent(uint8_t id);
void Setting_SyncEvent(uint8_t id);
#endif /* SETTING_SETTING_H_ */
