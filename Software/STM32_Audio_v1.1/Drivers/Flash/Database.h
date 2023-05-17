/*
 * Database.h
 *
 *  Created on: Mar 4, 2023
 *      Author: Duc Huy Le
 */

#ifndef FLASH_DATABASE_H_
#define FLASH_DATABASE_H_

#include "W25Qxx.h"
#include "Setting.h"
#define DATABASE_SIZE                             ((uint32_t)W25QXX_FLASH_SIZE)
#define DATABASE_BASE_ADDRESS                     ((uint32_t)0x00)
#define DATABASE_SECTOR_SIZE                       ((uint32_t)W25QXX_SECTOR_SIZE)
/* Setting */
#define DATABASE_SETTING_SIZE                     SETTING_MAX_ELEMENT
#define DATABASE_SETTING_BASE_ADDR                ((uint32_t)DATABASE_BASE_ADDRESS)
#define DATABASE_SETTING_RANDOM_SONG_ADDR         ((uint32_t)(DATABASE_SETTING_BASE_ADDR))
#define DATABASE_SETTING_LOOP_ONE_SONG_ADDR       ((uint32_t)(DATABASE_SETTING_BASE_ADDR + 1))
#define DATABASE_SETTING_ALWAYS_DISPLAY_ADDR      ((uint32_t)(DATABASE_SETTING_BASE_ADDR + 2))
/* Music */
#define DATABASE_MUSIC_SIZE                       1* sizeof(uint16_t)
#define DATABASE_MUSIC_BASE_ADDR                  ((uint32_t)(DATABASE_BASE_ADDRESS + DATABASE_SECTOR_SIZE * 1))
#define DATABASE_MUSIC_LAST_SONG_ADDR             ((uint32_t)(DATABASE_MUSIC_BASE_ADDR))
void Database_LoadDataSetting(void);
void Database_LoadDataMusic(void);
void Database_LoadData(void);
void Database_SaveDataSetting(uint8_t id);
void Database_SaveDataMusic(void);
#endif /* FLASH_DATABASE_H_ */
