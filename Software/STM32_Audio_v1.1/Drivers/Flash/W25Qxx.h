/*********************************************************************************************************
*
* File                : W25Qx.h
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 5.15
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2015, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25QXX_H
#define __W25QXX_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32f7xx.h"
#include "main.h"
#include "spi.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @addtogroup W25QXX
  * @{
  */

/** @defgroup W25QXX_Exported_Types
  * @{
  */

/**
  * @}
  */

/** @defgroup W25QXX_Exported_Constants
  * @{
  */

/**
  * @brief  W25QXX Configuration
  */
#define W25QXX_SPI_PORT hspi1
extern SPI_HandleTypeDef W25QXX_SPI_PORT;
#define W25QXX_CS_PORT  W25Qxx_CS_GPIO_Port
#define W25QXX_CS_PIN   W25Qxx_CS_Pin


#define W25Qxx_Select() HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET)
#define W25Qxx_UnSelect() HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET)

#define W25X80                          1// 1MB
#define W25Q80                          1// 1MB
#define W25Q16                          2// 2MB
#define W25Q32                          4// 4MB
#define W25Q64                          8// 8MB
// choise flash //
#define W25Qxx                          W25Q80 // select W25Q80 flash

#if (W25Qxx == W25Q80 || W25Qxx == W25X80)
#define W25QXX_FLASH_SIZE                  ((uint32_t)1024*1024)//0x100000 /* 128 MBits => 16MBytes */
#define W25QXX_BLOCK_SIZE                  ((uint32_t)64*1024)//0x10000   /* 256 sectors of 64KBytes */
#define W25QXX_SECTOR_SIZE                 ((uint32_t)4*1024)//0x1000    /* 4096 subsectors of 4kBytes */
#define W25QXX_PAGE_SIZE                   ((uint32_t)256)//0x100     /* 65536 pages of 256 bytes */
#endif
#define W25QXX_DUMMY_CYCLES_READ           4
#define W25QXX_DUMMY_CYCLES_READ_QUAD      10

#define W25QXX_BULK_ERASE_MAX_TIME         250000
#define W25QXX_SECTOR_ERASE_MAX_TIME       3000
#define W25QXX_SUBSECTOR_ERASE_MAX_TIME    800
#define W25QXX_TIMEOUT_VALUE               1000

/**
  * @brief  W25QXX Commands
  */
/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

#define ENTER_QPI_MODE_CMD                   0x38
#define EXIT_QPI_MODE_CMD                    0xFF

/* Identification Operations */
#define READ_ID_CMD                          0x90
#define DUAL_READ_ID_CMD                     0x92
#define QUAD_READ_ID_CMD                     0x94
#define READ_JEDEC_ID_CMD                    0x9F

/* Read Operations */
#define READ_CMD                             0x03
#define FAST_READ_CMD                        0x0B
#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_INOUT_FAST_READ_CMD             0xEB

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11


/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define QUAD_INPUT_PAGE_PROG_CMD             0x32


/* Erase Operations */
#define SECTOR_ERASE_CMD                     0x20
#define CHIP_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75


/* Flag Status Register */
#define W25QXX_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25QXX_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25QXX_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */


#define W25Qxx_Select()      HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET)
#define W25Qxx_DeSelect()     HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET)

#define W25QXX_OK            ((uint8_t)0x00)
#define W25QXX_ERROR         ((uint8_t)0x01)
#define W25QXX_BUSY          ((uint8_t)0x02)
#define W25QXX_TIMEOUT       ((uint8_t)0x03)


uint8_t W25Qxx_Init(void);
//static void W25Qxx_Reset(void);
//static uint8_t W25Qxx_GetStatus(void);
void W25Qxx_Test(void);
uint8_t W25Qxx_WriteEnable(void);
void W25Qxx_Read_ID(uint8_t *ID);
uint8_t W25Qxx_Read(uint32_t ReadAddr, uint8_t* pData, uint32_t Size);
uint8_t W25Qxx_Write(uint32_t WriteAddr, uint8_t* pData, uint32_t Size);
uint8_t W25Qxx_Erase_Sector(uint32_t Address);
uint8_t W25Qxx_Erase_Chip(void);

/**
  * @}
  */

/** @defgroup W25QXX_Exported_Functions
  * @{
  */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __W25QXX_H */

