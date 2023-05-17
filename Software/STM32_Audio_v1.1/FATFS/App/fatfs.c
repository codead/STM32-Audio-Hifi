/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include <stdio.h>
#include "ST7789.h"
#include "FileManager.h"
#include "Play_wav.h"
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
extern SD_HandleTypeDef hsd;
extern DMA_HandleTypeDef hdma_sdio;
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
uint8_t BSP_SD_Init(void)
{
  uint8_t sd_state = MSD_OK;
  /* Check if the SD card is plugged in the slot */
  if (BSP_SD_IsDetected() != SD_PRESENT)
  {
    return MSD_ERROR;
  }
  printf("Detected SDcard !!!\r\n");
  /* HAL SD initialization */
  sd_state = HAL_SD_Init(&hsd);
  /* Configure SD Bus width (4 bits mode selected) */
  if (sd_state == MSD_OK)
  {

    /* Enable wide operation */
    if (HAL_SD_ConfigWideBusOperation(&hsd, SDIO_BUS_WIDE_4B) != HAL_OK)
    {
      sd_state = MSD_ERROR;
      printf("BSP SD Init failed Error code = %ld\r\n", hsd.ErrorCode);
    }
  }

  return sd_state;
}
void ScanSDCard(void)
{
  char msg[30] = {0};
  uint8_t isError = 0;
  FontDef font = Font_11x18;
  while(1)
  {
    // Detect SDCard
    while (BSP_SD_Init() == MSD_ERROR)
    {
      printf("Do not detected SDCard!!!\n");
      printf("Please insert your SDCard\n");
      ST7789_WriteStringAutoDownLine(1 , "[+] Do not detected SDCard!!!", font, RED, WHITE);
      ST7789_WriteStringAutoDownLine(0 , "[+] Please insert your SDCard.", font, RED, WHITE);
      HAL_Delay(1000);
      isError = 1;
    }
    // Mount to drive
    retSD = f_mount(&SDFatFS, (TCHAR const*)SDPath, 0);
    if(retSD != FR_OK)
    {
      printf("f_mount fail ! Error code: %d\r\n", retSD);
      ST7789_WriteStringAutoDownLine(0 , "[+] Wrong SDCard!!!", font, RED, WHITE);
      ST7789_WriteStringAutoDownLine(0 , "[+] Please change your SDCrad format.", font, RED, WHITE);
      while(BSP_SD_IsDetected() == SD_PRESENT)
      {
        HAL_Delay(1000);
      }
      continue;
    }
    else
    {
      printf("f_mount completed !!\r\n");
    }
    if(isError > 0)
    {
      ST7789_WriteStringAutoDownLine(0 , "[+] Detected SDCard", font, BLUE, WHITE);
      ST7789_WriteStringAutoDownLine(0 , "[+] Scanning WAV file...", font, BLUE, WHITE);
    }
    // Scan wav file
    ScanWavefiles(F_Music);
    if(Audio.MaxSong > 0 && isError > 0)
    {
      if(Audio.MaxSong == 1)
      {
        sprintf(msg, "[+] Found %d song", Audio.MaxSong);
        ST7789_WriteStringAutoDownLine(0 , msg, font, BLUE, WHITE);
      }
      else
      {
        sprintf(msg, "[+] Found %d songs", Audio.MaxSong);
        ST7789_WriteStringAutoDownLine(0 , msg, font, BLUE, WHITE);

      }
      ST7789_WriteStringAutoDownLine(0 , "[+] Start play music after 5s...", font, BLUE, WHITE);
      HAL_Delay(5000);
      return;
    }
    else if(Audio.MaxSong > 0)
    {
      return;
    }
    else
    {
      ST7789_WriteStringAutoDownLine(0 , "[+] Do not found any song!!!", font, RED, WHITE);
      ST7789_WriteStringAutoDownLine(0 , "[+] Please add more song.", font, RED, WHITE);
      isError = 2;
      while(BSP_SD_IsDetected() == SD_PRESENT)
      {
        HAL_Delay(1000);
      }
      continue;
    }
  }
}
/* USER CODE END Application */
