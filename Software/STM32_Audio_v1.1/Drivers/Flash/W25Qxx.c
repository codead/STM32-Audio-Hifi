/*********************************************************************************************************
*
* File                : W25Qxx.c
* Hardware Environment:
* Build Environment   : RealView MDK-ARM  Version: 4.20
* Version             : V1.0
* By                  :
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                          All Rights Reserved
*
*********************************************************************************************************/

#include "W25Qxx.h"
#include <stdio.h>
/**
  * @brief  This function reset the W25Qx.
  * @retval None
  */
static void W25Qxx_Reset(void)
{
  uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};

  W25Qxx_Select();
  /* Send the reset command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 2, W25QXX_TIMEOUT_VALUE);
  W25Qxx_DeSelect();

}

/**
  * @brief  Reads current status of the W25QXX.
  * @retval W25QXX memory status
  */
static uint8_t W25Qxx_GetStatus(void)
{
  uint8_t cmd[] = {READ_STATUS_REG1_CMD};
  uint8_t status;

  W25Qxx_Select();
  /* Send the read status command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /* Reception of the data */
  HAL_SPI_Receive(&W25QXX_SPI_PORT,&status, 1, W25QXX_TIMEOUT_VALUE);
  W25Qxx_DeSelect();

  /* Check the value of the register */
  if((status & W25QXX_FSR_BUSY) != 0)
  {
    return W25QXX_BUSY;
  }
  else
  {
    return W25QXX_OK;
  }
}
/**
  * @brief  Initializes the W25QXX interface.
  * @retval None
  */
uint8_t W25Qxx_Init(void)
{
  /* Reset W25Qxxx */
  W25Qxx_Reset();

  return W25Qxx_GetStatus();
}


/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @retval None
  */
uint8_t W25Qxx_WriteEnable(void)
{
  uint8_t cmd[] = {WRITE_ENABLE_CMD};
  uint32_t tickstart = HAL_GetTick();

  /*Select the FLASH: Chip Select low */
  W25Qxx_Select();
  /* Send the read ID command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Qxx_DeSelect();

  /* Wait the end of Flash writing */
  while(W25Qxx_GetStatus() == W25QXX_BUSY)
  {
    asm("nop");
  }
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
    {
      return W25QXX_TIMEOUT;
    }
  }

  return W25QXX_OK;
}

/**
  * @brief  Read Manufacture/Device ID.
  * @param  return value address
  * @retval None
  */
void W25Qxx_Read_ID(uint8_t *ID)
{
  uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};

  W25Qxx_Select();
  /* Send the read ID command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 4, W25QXX_TIMEOUT_VALUE);
  /* Reception of the data */
  HAL_SPI_Receive(&W25QXX_SPI_PORT,ID, 2, W25QXX_TIMEOUT_VALUE);
  W25Qxx_DeSelect();

}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be read
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read
  * @retval QSPI memory status
  */
uint8_t W25Qxx_Read(uint32_t ReadAddr, uint8_t* pData, uint32_t Size)
{
  uint8_t cmd[4];

  /* Configure the command */
  cmd[0] = READ_CMD;
  cmd[1] = (uint8_t)(ReadAddr >> 16);
  cmd[2] = (uint8_t)(ReadAddr >> 8);
  cmd[3] = (uint8_t)(ReadAddr);

  W25Qxx_Select();
  /* Send the read ID command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 4, W25QXX_TIMEOUT_VALUE);
  /* Reception of the data */
  if (HAL_SPI_Receive(&W25QXX_SPI_PORT, pData,Size,W25QXX_TIMEOUT_VALUE) != HAL_OK)
  {
    return W25QXX_ERROR;
  }
  W25Qxx_DeSelect();
  return W25QXX_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write,No more than 256byte.
  * @retval QSPI memory status
  */
uint8_t W25Qxx_Write(uint32_t WriteAddr, uint8_t* pData, uint32_t Size)
{
  uint8_t cmd[4];
  uint32_t end_addr, current_size, current_addr;
  uint32_t tickstart = HAL_GetTick();

  /* Calculation of the size between the write address and the end of the page */
  current_addr = 0;

  while (current_addr <= WriteAddr)
  {
    current_addr += W25QXX_PAGE_SIZE;
  }
  current_size = current_addr - WriteAddr;

  /* Check if the size of the data is less than the remaining place in the page */
  if (current_size > Size)
  {
    current_size = Size;
  }

  /* Initialize the adress variables */
  current_addr = WriteAddr;
  end_addr = WriteAddr + Size;

  /* Perform the write page by page */
  do
  {
    /* Configure the command */
    cmd[0] = PAGE_PROG_CMD;
    cmd[1] = (uint8_t)(current_addr >> 16);
    cmd[2] = (uint8_t)(current_addr >> 8);
    cmd[3] = (uint8_t)(current_addr);

    /* Enable write operations */
    W25Qxx_WriteEnable();

    W25Qxx_Select();
    /* Send the command */
    if (HAL_SPI_Transmit(&W25QXX_SPI_PORT,cmd, 4, W25QXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25QXX_ERROR;
    }

    /* Transmission of the data */
    if (HAL_SPI_Transmit(&W25QXX_SPI_PORT, pData,current_size, W25QXX_TIMEOUT_VALUE) != HAL_OK)
    {
      return W25QXX_ERROR;
    }
      W25Qxx_DeSelect();
      /* Wait the end of Flash writing */
    while(W25Qxx_GetStatus() == W25QXX_BUSY)
    {
      asm("nop");
    }
    {
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart) > W25QXX_TIMEOUT_VALUE)
      {
        return W25QXX_TIMEOUT;
      }
    }

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25QXX_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QXX_PAGE_SIZE;
  } while (current_addr < end_addr);


  return W25QXX_OK;
}

/**
  * @brief  Erases the specified block of the QSPI memory.
  * @param  BlockAddress: Block address to erase
  * @retval QSPI memory status
  */
uint8_t W25Qxx_Erase_Sector(uint32_t Address)
{
  uint8_t cmd[4];
  uint32_t tickstart = HAL_GetTick();
  cmd[0] = SECTOR_ERASE_CMD;
  cmd[1] = (uint8_t)(Address >> 16);
  cmd[2] = (uint8_t)(Address >> 8);
  cmd[3] = (uint8_t)(Address);

  /* Enable write operations */
  W25Qxx_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25Qxx_Select();
  /* Send the read ID command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 4, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Qxx_DeSelect();

  /* Wait the end of Flash writing */
  while(W25Qxx_GetStatus() == W25QXX_BUSY)
  {
    asm("nop");
  }
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_SECTOR_ERASE_MAX_TIME)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

/**
  * @brief  Erases the entire QSPI memory.This function will take a very long time.
  * @retval QSPI memory status
  */
uint8_t W25Qxx_Erase_Chip(void)
{
  uint8_t cmd[4];
  uint32_t tickstart = HAL_GetTick();
  cmd[0] = CHIP_ERASE_CMD;

  /* Enable write operations */
  W25Qxx_WriteEnable();

  /*Select the FLASH: Chip Select low */
  W25Qxx_Select();
  /* Send the read ID command */
  HAL_SPI_Transmit(&W25QXX_SPI_PORT, cmd, 1, W25QXX_TIMEOUT_VALUE);
  /*Deselect the FLASH: Chip Select high */
  W25Qxx_DeSelect();

  /* Wait the end of Flash writing */
  while(W25Qxx_GetStatus() != W25QXX_BUSY)
  {
    asm("nop");
  }
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart) > W25QXX_BULK_ERASE_MAX_TIME)
    {
      return W25QXX_TIMEOUT;
    }
  }
  return W25QXX_OK;
}

void W25Qxx_Test(void)
{
  W25Qxx_Init();
  if(W25Qxx_Erase_Sector(0) != W25QXX_OK)
  {
    printf("Not OK\n");
  }

  uint8_t ID[4] = {0};
  W25Qxx_Read_ID(ID);
  printf("W25Qxxx ID is : ");
  for(uint8_t i = 0;i < 2; i++)
  {
    printf("0x%02X ",ID[i]);
  }
  printf("\r\n\r\n");
  uint8_t data_tx[32] = {0};
  for(uint8_t i = 0; i < 32; i++)
  {
    data_tx[i] = i + 6;
  }
  uint8_t data_rx[32] = {0};
  W25Qxx_Write(0, data_tx, 32);
  W25Qxx_Read(0, data_rx, 32);
  for(uint8_t i = 0; i < 32; i++)
  {
    printf("0x%02X, ", data_rx[i]);
  }
  printf("\n Done!!!\n");
}
