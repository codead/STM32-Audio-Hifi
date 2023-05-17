/*
 * STC3100.c
 *
 *  Created on: Nov 13, 2022
 *      Author: Duc Huy Le
 */
 /***
 *      ____    ____  _  ____   ____    ___   __  __
 *     / ___|  / ___|(_)|  _ \ |  _ \  / _ \ |  \/  |
 *     \___ \ | |    | || | | || |_) || | | || |\/| |
 *      ___) || |___ | || |_| ||  _ < | |_| || |  | |
 *     |____/  \____||_||____/ |_| \_\ \___/ |_|  |_|
 *        (C)2018 Scidrom

  Description: Driver for STC3100, Battery monitor IC with Coulomb counter/gas gauge
  License: GNU General Public License
  Maintainer: S54MTB
*/

/******************************************************************************
  * @file    stc3100.c
  * @author  S54MTB
  * @version V1.0.0
  * @date    22-June-2018
  * @brief   Driver for STC3100, Battery monitor IC with Coulomb counter/gas gauge
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 Scidrom
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  ******************************************************************************
  */


#include "STC3100.h"
#include "i2c.h"
#include <stdio.h>
uint8_t BatteryStatus = 0;
STC3100_INFO Batt =
{
  .hi2c = &hi2c1,
  .current = 0,
#ifdef BATTERY_CHARGER
  .charging = 0,
#endif
  .rsoc = 0,
  .temperature = 0,
  .voltage = 0,
  .ids = {0}
//    &hi2c1,0,0,0,0,0,{0}
};
/**
  * @brief  Write STC3100 register(s) in blocking mode to a specific register address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  reg STC3100 Internal register address
  * @param  buf Pointer to data buffer
  * @param  len Amount of data to be sent
  * @retval HAL status
  */
static HAL_StatusTypeDef STC3100_WriteReg(uint8_t regAddress, uint8_t *buf, uint8_t len)
{
  HAL_StatusTypeDef ret;
  int i;
  uint8_t data[65];

  if (len>64) return HAL_ERROR;
  data[0] = regAddress;     // Register address
  for (i=0; i<len; i++)
  {
    data[i+1]=buf[i]; // add data
  }

  /** Target device address: The device 7 bits address must be shift at right before call interface */
  ret = HAL_I2C_Master_Transmit(&hi2c1, STC3100_I2C_ADDRESS << 1, data, len+1, 100);

  return ret;
}


/**
  * @brief  Read STC3100 register(s) in blocking mode from a specific register address
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  reg STC3100 Internal register address
  * @param  buf Pointer to data buffer
  * @param  len Amount of data to be read
  * @retval HAL status
  */
static HAL_StatusTypeDef STC3100_ReadReg(uint8_t regAddress, uint8_t *buf, uint8_t len)
{
  HAL_StatusTypeDef ret;

  /** Target device address: The device 7 bits address must be shift at right before call interface */
  // first set the register pointer to the register wanted to be read
  ret = HAL_I2C_Master_Transmit(&hi2c1, STC3100_I2C_ADDRESS << 1, &regAddress, 1, 100);
  if(ret == HAL_OK)
  {
    // receive the len x 8bit data into the receive buffer
    ret = HAL_I2C_Master_Receive(&hi2c1, STC3100_I2C_ADDRESS << 1 | 0x01, buf, len, 100);
  }
  return ret;
}


/**
 * Init the battery fuel gauge.
 */
HAL_StatusTypeDef STC3100_Init()
{
  HAL_StatusTypeDef ret = HAL_OK;
  uint8_t data[2];
  // first, check the presence of the STC3100 by reading first byte of dev. ID
//  ret = STC3100_ReadReg(STC3100_REG_ID0, &regs[0], 1);
//
//  if (ret!= 0x10) return HAL_ERROR;
  // read the REG_CTRL to reset the GG_EOC and VTM_EOC bits
  ret |= STC3100_ReadReg(STC3100_REG_CTRL, data, 1);
  // Write mode
  data[0] = 0x10;
  ret |= STC3100_WriteReg(STC3100_REG_MODE, data, 1);
  // write CTRL
  data[0] = 0x02;
  ret |= STC3100_WriteReg(STC3100_REG_CTRL, data, 1);

  ret |= STC3100_ReadReg(STC3100_REG_MODE, data, 1);
  printf("STC3100_REG_MODE = 0x%02X\n", data[0]);
  ret |= STC3100_ReadReg(STC3100_REG_CTRL, data, 1);
  printf("STC3100_REG_CTRL = 0x%02X\n", data[0]);
  if(ret == HAL_OK)
  {
    BatteryStatus = 0;// OK
  }
  else
  {
    BatteryStatus = 1; // Not OK
  }
  return ret;
}


void STC3100_ChargeInit(GPIO_TypeDef *port, uint32_t Pin)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  switch (GPIO_GET_INDEX(port))
  {
    case 0 : __HAL_RCC_GPIOA_CLK_ENABLE(); break;
    case 1 : __HAL_RCC_GPIOB_CLK_ENABLE(); break;
    case 2 : __HAL_RCC_GPIOC_CLK_ENABLE(); break;
    case 5 : __HAL_RCC_GPIOH_CLK_ENABLE(); break;
  }

  GPIO_InitStruct.Pin = Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(port, &GPIO_InitStruct);

}


/*
 * Read the battery Voltage in milivolts, return 0 if something wrong
 */
static HAL_StatusTypeDef STC3100_ReadVoltage(STC3100_INFO *bat)
{
  HAL_StatusTypeDef ret;
  uint8_t data[2];

  float volt = 0;

  ret = STC3100_ReadReg(STC3100_REG_VOLTL, data,2);
  //printf("data[1] = 0x%02X, data[0] = 0x%02X\n", data[1], data[0]);
  // The battery voltage is coded in binary format, and the LSB value is 2.44 mV.
  if (ret == HAL_OK)
  {
    volt = ((data[1] & 0x0f) << 8) + data[0];
    volt *= 2.44e-3f;
  }
  else
  {
    printf("Error: Cannot read STC3100\n");
    volt = 0xFF;
  }
  bat->voltage = volt;

  return ret;
}


/*
 * read the STC3100 unique ID, first byte must be 0x10
 */
static HAL_StatusTypeDef STC3100_ReadID(STC3100_INFO *bat)
{
  HAL_StatusTypeDef ret;
  ret = STC3100_ReadReg(STC3100_REG_ID0,bat->ids,8);
  return ret;
}


/*
 * Return the battery average current
 * Note that current can be negative signed as well
 * Or 0 if something fails.
 */
static HAL_StatusTypeDef STC3100_ReadCurrent(STC3100_INFO *bat)
{
  HAL_StatusTypeDef ret;
  uint8_t bytes[2];
  int16_t regval;
  float Acurrent = 0.0f;
/** The battery current is coded in 2�s complement 14 bit format, and the LSB value is 11.77 uV. */
  ret = STC3100_ReadReg(STC3100_REG_AIL,bytes,2);
  regval = (bytes[1]<<10) | (bytes[0] << 2);  // 16 bit signed
  regval /= 4;  // 16 to 14 bit format to get correct value
  if (ret == HAL_OK)
  {
    ///current (A) = current_code * 11.77e-6 / Rsense (Ohm)
    Acurrent = (float)regval * 11.77e-6f / STC3100_RSENSE_RESISTANCE;
  }

  bat->current = Acurrent;

  return ret;
}

/*
 * Return the battery Relative State-of-Charge
 * Or < 0 if something fails.
 */
static HAL_StatusTypeDef STC3100_ReadRosc(STC3100_INFO *bat)
{
  HAL_StatusTypeDef ret;
  uint8_t bytes[2];
  int16_t regval;
  float Ah = -10000.0f;
// The charge data is coded in 2�s complement format, and the LSB value is 6.70 uV.h.
  ret = STC3100_ReadReg(STC3100_REG_RSOCL,bytes,2);
  regval = (bytes[1]<<10) | (bytes[0] << 2);  // 16 bit signed
  regval /= 4;  // 16 to 14 bit format to get correct value

  if (ret == HAL_OK)
  {
    Ah = (float)regval * 6.70e-6f / STC3100_RSENSE_RESISTANCE;
  }

  bat->rsoc = Ah;

  return ret;
}


/*
 * Return the STC3100 chip temperature or -1K if something wrong
 */
static HAL_StatusTypeDef STC3100_ReadTemperature(STC3100_INFO *bat)
{
  HAL_StatusTypeDef ret;
  uint8_t bytes[2];
  int16_t regval;
  float tempdegC = -274.0f;
// The temperature value is coded in 2�s complement format, and the LSB value is 0.125� C.
  ret = STC3100_ReadReg(STC3100_REG_TEMPL,bytes,2);
  regval = (bytes[1]<<10) | (bytes[0] << 2);  // 16 bit signed
  regval /= 4;  // 16 to 14 bit format to get correct value

  if (ret == HAL_OK)
  {
    tempdegC = (float)regval * 0.125f;
  }

  bat->temperature = tempdegC;

  return ret;
}


/**
  * @brief Return current status of the battery charging
  * @param bat    pointer to Battery status structure
  * @retval       POWER_STATUS_CHARGING | POWER_STATUS_NOT_CHARGING
  */
uint8_t STC3100_ReadChargeStatus(STC3100_INFO *bat)
{
  // determine from charging pin or from current sign if on-board charger is not present
#ifdef BATTERY_CHARGER
  return ((HAL_GPIO_ReadPin(CHARGER_STATUS_PORT, CHARGER_STATUS_PIN) == GPIO_PIN_RESET)) ? POWER_STATUS_CHARGING : POWER_STATUS_NOT_CHARGING;
#else
  STC3100_ReadCurrent(bat);
  return (bat->current < 0.0f) ? POWER_STATUS_CHARGING : POWER_STATUS_NOT_CHARGING;
#endif
}


/**
  * @brief Get and update battery monitoring property
  * @param bat    pointer to Battery status structure
  * @param prop   specific property
  */
HAL_StatusTypeDef STC3100_Get(STC3100_INFO *bat, stc3100_battery_prop_t prop)
{
  switch (prop)
  {
    case BATTERY_PROP_PRESENT:

    break;

    case BATTERY_PROP_VOLTAGE_NOW:
      return STC3100_ReadVoltage(bat);

    case BATTERY_PROP_CURRENT_NOW:
      return STC3100_ReadCurrent(bat);

    case BATTERY_PROP_CAPACITY:
      return STC3100_ReadRosc(bat);

    case BATTERY_PROP_TEMP:
      return STC3100_ReadTemperature(bat);

    case BATTERY_PROP_UNIQUEID:
      return STC3100_ReadID(bat);
#ifdef  BATTERY_CHARGER
    case BATTERY_PROP_CHARGING:
      bat->charging = STC3100_ReadChargeStatus(bat);
    break;
#endif
  }

  return HAL_ERROR;  // unknown property

}






