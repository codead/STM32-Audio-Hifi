/*
 * Database.c
 *
 *  Created on: Mar 4, 2023
 *      Author: Duc Huy Le
 */
#include "Database.h"
#include "W25Qxx.h"
#include "Setting.h"
#include "Display.h"
#include "Play_wav.h"
extern __SETTING Setting;
extern AUDIO Audio;
extern volatile DISPLAY Display;
void Database_LoadDataSetting(void)
{
  uint8_t database[DATABASE_SETTING_SIZE] = {0};
  W25Qxx_Read(DATABASE_SETTING_BASE_ADDR, database, DATABASE_SETTING_SIZE);
  for(uint8_t id = 0; id < DATABASE_SETTING_SIZE; id++)
  {
    if(database[id] == 0)
    {
      *((bool*)(Setting.Object[id].Event)) = false;

    }
    else
    {
      *((bool*)(Setting.Object[id].Event)) = true;
    }
    Setting_SyncEvent(id);
  }
}
void Database_LoadDataMusic(void)
{
  uint8_t database[DATABASE_MUSIC_SIZE] = {0};
  W25Qxx_Read(DATABASE_MUSIC_BASE_ADDR, database, DATABASE_MUSIC_SIZE);
  Audio.CurrentSong = ((uint16_t)database[1]) << 8 | database[0];
  if(Audio.CurrentSong >= Audio.MaxSong)
  {
    Audio.CurrentSong = 0;
    Database_SaveDataMusic();
  }
//  Audio.PreviousSong = Audio.CurrentSong;
//  Audio_CreatePreviousSongList();
//  Display.CursorListMusic = Audio.CurrentSong;
}
void Database_LoadData(void)
{
  Database_LoadDataSetting();
  Database_LoadDataMusic();
}
void Database_SaveDataSetting(uint8_t id)
{
  uint8_t database[DATABASE_SETTING_SIZE] = {0};
  // Save data before erase flash
  W25Qxx_Read(DATABASE_SETTING_BASE_ADDR, database, DATABASE_SETTING_SIZE);
  // Erase data sector
  W25Qxx_Erase_Sector(DATABASE_SETTING_BASE_ADDR);
  // Preparing data
  if(*((bool*)(Setting.Object[id].Event)) == false)
  {
    database[id] = 0;
  }
  else
  {
    database[id] = 1;
  }
  // Save to flash
  W25Qxx_Write(DATABASE_SETTING_BASE_ADDR, database, DATABASE_SETTING_SIZE);
  // Reload data from flash
  Database_LoadDataSetting();
}
void Database_SaveDataMusic(void)
{
  uint8_t database[DATABASE_MUSIC_SIZE] = {0};
  // Save data before erase flash
  W25Qxx_Read(DATABASE_MUSIC_BASE_ADDR, database, DATABASE_MUSIC_SIZE);
  // Erase data sector
  W25Qxx_Erase_Sector(DATABASE_MUSIC_BASE_ADDR);
  // Preparing data
  database[0] = (uint8_t)(Audio.CurrentSong & 0x00FF);
  database[1] = (uint8_t)((Audio.CurrentSong & 0xFF00) >> 8);
  // Save to flash
  W25Qxx_Write(DATABASE_MUSIC_BASE_ADDR, database, DATABASE_MUSIC_SIZE);
  // Reload data from flash
  Database_LoadDataMusic();
}
