/*
 * IO_Expand.c
 *
 *  Created on: Aug 13, 2022
 *      Author: Duc Huy Le
 */

#include "IO_Expand.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_gpio.h"
#define USE_SX1509B  // Select IO expander device
#ifdef USE_SX1509B
#include "SX1509B.h"
#endif
#define IO_BTN_LEFT          IO_EXPAND_PIN_2
#define IO_BTN_DOWN          IO_EXPAND_PIN_3
#define IO_LED_DOWN          IO_EXPAND_PIN_4
#define IO_BTN_RIGHT         IO_EXPAND_PIN_5
#define IO_LED_LEFT          IO_EXPAND_PIN_6
#define IO_LED_RIGHT         IO_EXPAND_PIN_7
#define IO_BTN_CENTRE        IO_EXPAND_PIN_8
#define IO_BTN_UP            IO_EXPAND_PIN_9
#define IO_LED_CENTRE        IO_EXPAND_PIN_12
#define IO_LED_UP            IO_EXPAND_PIN_13

#ifdef USE_SX1509B
extern uint32_t SX1509B_CLK;
uint8_t IO_Expand_WritePin(uint16_t pin, IO_Expand_PinState state)
{
  uint8_t res = HAL_OK;
  uint16_t tempRegDir;
  uint16_t tempRegData;
  uint16_t tempPullUp;
  uint16_t tempPullDown;
  res = SX1509B_ReadRegSequence(RegDirB, &tempRegDir);

  if ((0xFFFF ^ tempRegDir) & pin) // If the pin is an output, write high/low
  {
    res |= SX1509B_ReadRegSequence(RegDataB, &tempRegData);
    if (state == PIN_SET)
    {
      tempRegData |= pin;
    }
    else
    {
      tempRegData &= ~pin;
    }
    res |= SX1509B_WriteRegSequence(RegDataB, tempRegData);
  }
  else // Otherwise the pin is an input, pull-up/down
  {
    res |= SX1509B_ReadRegSequence(RegPullUpB, &tempPullUp);
    res |= SX1509B_ReadRegSequence(RegPullDownB, &tempPullDown);

    if (state == PIN_SET) // if HIGH, do pull-up, disable pull-down
    {
      tempPullUp |= pin;
      tempPullDown &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempPullUp);
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempPullDown);
    }
    else // If LOW do pull-down, disable pull-up
    {
      tempPullDown |= pin;
      tempPullUp &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempPullUp);
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempPullDown);
    }
  }
  return res;
}
uint8_t IO_Expand_ConfigPinMode(IO_Expand_Typedef IO_Expand_InitStruct)
{
  uint8_t res = HAL_OK;
  uint16_t tempRegDir;
  uint16_t tempRegInputDisable;
  uint16_t tempRegPullUp;
  uint16_t tempRegPullDown;
  uint8_t tempRegMisc;
  uint16_t tempRegLEDDriverEnable;
  uint8_t tempRegDebounceConfig;
  uint16_t tempRegDebounceEnable;
  uint16_t pin = IO_Expand_InitStruct.Pin;
  IO_EXPAND_MODE mode = IO_Expand_InitStruct.Mode;
  uint32_t pull = IO_Expand_InitStruct.Pull;
  LinOrLog linLog = IO_Expand_InitStruct.LinOrLog;
  IO_EXPAND_CLOCK ioClock = IO_Expand_InitStruct.IO_Clock;
  // If mode Input
  if (mode & IO_EXPAND_MODE_INPUT)
  {
    // config IO as input
    res |= SX1509B_ReadRegSequence(RegDirB, &tempRegDir);
    tempRegDir |= pin;
    res |= SX1509B_WriteRegSequence(RegDirB, tempRegDir);
    // config IO pull up
    if(pull & IO_EXPAND_PULL_UP)
    {
      // Enable pull-up
      res |= SX1509B_ReadRegSequence(RegPullUpB, &tempRegPullUp);
      tempRegPullUp |= pin;
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempRegPullUp);
      // Disable pull-down
      res |= SX1509B_ReadRegSequence(RegPullDownB, &tempRegPullDown);
      tempRegPullDown &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempRegPullDown);
    }
    // config IO pull down
    else if(pull & IO_EXPAND_PULL_DOWN)
    {
      // Disable pull-up
      res |= SX1509B_ReadRegSequence(RegPullUpB, &tempRegPullUp);
      tempRegPullUp &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempRegPullUp);
      // Enable pull-down
      res |= SX1509B_ReadRegSequence(RegPullDownB, &tempRegPullDown);
      tempRegPullDown |= pin;
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempRegPullDown);
    }
    // otherwise, no pull
    else
    {
      // Disable pull-up
      res |= SX1509B_ReadRegSequence(RegPullUpB, &tempRegPullUp);
      tempRegPullUp &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempRegPullUp);
      // Disable pull-down
      res |= SX1509B_ReadRegSequence(RegPullDownB, &tempRegPullDown);
      tempRegPullDown &= ~pin;
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempRegPullDown);
    }
    // if mode interrupt
    if (mode & IO_EXPAND_MODE_IT)
    {
      // set Debounce time = 16ms
      tempRegDebounceConfig = 0b101;
      res |= SX1509B_WriteReg(RegDebounceConfig, tempRegDebounceConfig);
      // enable interrupt

      uint8_t riseFall;
      // if both rising and falling
      if (mode & IO_EXPAND_MODE_RISING_FALLING)
      {
        riseFall = CHANGE;
      }
      // if rising event
      else if(mode & IO_EXPAND_MODE_RISING)
      {
        riseFall = RISING;
      }
      // if falling event
      else if(mode & IO_EXPAND_MODE_FALLING)
      {
        riseFall = FALLING;
      }
      for(uint8_t i = 0 ; i < 16; i++)// scan pin
      {
        if((pin & ((uint16_t)1U << i))) // check pin to enable interrupt
        {
          SX1509B_EnableInterrupt(i, riseFall);
          printf("Config pin %d as interrupt\n", i);
        }
      }
      // enable Debounce
      res |= SX1509B_ReadRegSequence(RegDebounceEnableB, &tempRegDebounceEnable);
      tempRegDebounceEnable |= pin;
      res |= SX1509B_WriteRegSequence(RegDebounceEnableB, tempRegDebounceEnable);
      // clear interrupt
      res |= SX1509B_WriteRegSequence(RegInterruptSourceB, 0xFFFF);
    }
  }
  // else mode Output
  else
  {
    // config IO as output
    res |= SX1509B_ReadRegSequence(RegDirB, &tempRegDir);
    tempRegDir &= ~pin;
    res |= SX1509B_WriteRegSequence(RegDirB, tempRegDir);
    // Disable input buffer
    res |= SX1509B_ReadRegSequence(RegInputDisableB, &tempRegInputDisable);
    tempRegInputDisable |= pin;
    res |= SX1509B_WriteRegSequence(RegInputDisableB, tempRegInputDisable);
    // Disable pull-up
    res |= SX1509B_ReadRegSequence(RegPullUpB, &tempRegPullUp);
    tempRegPullUp &= ~pin;
    res |= SX1509B_WriteRegSequence(RegPullUpB, tempRegPullUp);

    // Configure LED driver clock and mode (REG_MISC)
    res |= SX1509B_ReadReg(RegMisc, &tempRegMisc);
    if(linLog == LOG)
    {
      tempRegMisc |= (1 << 7); // set logarithmic mode bank B
      tempRegMisc |= (1 << 3); // set logarithmic mode bank A
    }
    else
    {
      tempRegMisc &= ~(1 << 7); // set linear mode bank B
      tempRegMisc &= ~(1 << 3); // set linear mode bank A
    }
    // mask only 3 bits and shift to bit position 6:4
    ioClock = (ioClock & 0x07) << 4;
    tempRegMisc |= ioClock;
    res |= SX1509B_WriteReg(RegMisc, tempRegMisc);
    // Enable LED driver operation (REG_LED_DRIVER_ENABLE)
    res |= SX1509B_ReadRegSequence(RegLEDDriverEnableB, &tempRegLEDDriverEnable);
    tempRegLEDDriverEnable |= pin;
    res |= SX1509B_WriteRegSequence(RegLEDDriverEnableB, tempRegLEDDriverEnable);
    // Config if led mode is breathe
    if(mode & IO_EXPAND_MODE_OUTPUT_BREATHE)
    {
      printf("Config ouput mode breather\n");
      for(uint8_t i = 0 ; i < 16; i++)
      {
        if((pin & ((uint16_t)1U << i)))
        {
          SX1509B_LedBreathe(i, 0xFF, 2, 2, 1, 2 << 3);
          printf("Config pin %d as led breathe\n", i);
        }
      }
    }
  }
  return res;
}
uint8_t IO_Expand_Init(void)
{
  IO_Expand_Typedef IO_Expand_InitStruct;
  uint8_t res = HAL_OK;
  HAL_GPIO_WritePin(SX1509B_GPIO_Port, SX1509B_RST_Pin, GPIO_PIN_SET);
  res |= SX1509B_RegReset();;
  if(res == HAL_OK)
  {
    printf("SX1509B_RegReset complete !!!\r\n");
  }
  else
  {
    printf("SX1509B_RegReset fail ! Error code: %d\r\n", res);
  }
  // Set clock for SX1509B IO = 2MHz, divide 7 => IO Clk =  2MHz/(2^6)
  res |= SX1509B_SetClock(INTERNAL_CLOCK_2MHZ, CLK_INPUT, 0, 7);
  SX1509B_CLK = 2000000;

  IO_Expand_InitStruct.Pin = IO_LED_DOWN | IO_LED_CENTRE | IO_LED_RIGHT | IO_LED_LEFT | IO_LED_UP;
  IO_Expand_InitStruct.Mode = IO_EXPAND_MODE_OUTPUT_BREATHE;
  IO_Expand_InitStruct.Pull = IO_EXPAND_NO_PULL;
  IO_Expand_InitStruct.IO_Clock = CLOCK_VERY_SLOW;
  IO_Expand_InitStruct.LinOrLog = LOG;
  res |= IO_Expand_ConfigPinMode(IO_Expand_InitStruct);
  res |= IO_Expand_WritePin(IO_Expand_InitStruct.Pin, PIN_RESET);

  IO_Expand_InitStruct.Pin = IO_BTN_LEFT | IO_BTN_DOWN | IO_BTN_RIGHT | IO_BTN_CENTRE | IO_BTN_UP;
  IO_Expand_InitStruct.Mode = IO_EXPAND_MODE_INPUT_IT_FALLING;
  IO_Expand_InitStruct.Pull = IO_EXPAND_PULL_UP;
  res |= IO_Expand_ConfigPinMode(IO_Expand_InitStruct);
  return res;
}


#endif
