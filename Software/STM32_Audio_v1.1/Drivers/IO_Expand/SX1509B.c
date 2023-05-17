/*
 * SX1509B.c
 *
 *  Created on: Jul 24, 2022
 *      Author: Duc Huy Le
 */
#include "../IO_Expand/SX1509B.h"

#include "i2c.h"
#include "main.h"
#include <stdio.h>
#include <stdbool.h>
#include "stm32f4xx_hal_gpio.h"

#include "Button_Led.h"
#define SX1509B_I2C_ADDRESS  0x3E
uint32_t SX1509B_CLK;
const uint8_t Lin2Log[256] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 1, 1, 1, 1, 1, 1,
    2, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 3, 3, 3,
    4, 4, 4, 4, 5, 5, 5, 5,
    6, 6, 6, 6, 7, 7, 7, 7,
    8, 8, 8, 8, 9, 9, 9, 9,
    10, 10, 10, 10, 11, 11, 12, 12,
    13, 13, 13, 13, 14, 14, 14, 14,
    16, 16, 17, 17, 18, 18, 19, 19,
    20, 20, 21, 21, 22, 22, 23, 23,
    24, 24, 25, 25, 26, 26, 27, 27,
    28, 28, 30, 30, 31, 31, 32, 32,
    34, 34, 35, 35, 36, 36, 38, 38,
    39, 39, 41, 41, 42, 42, 44, 44,
    46, 46, 46, 46, 49, 49, 49, 49,
    53, 53, 53, 53, 56, 56, 56, 56,
    60, 60, 60, 60, 65, 65, 65, 65,
    69, 69, 69, 69, 73, 73, 73, 73,
    78, 78, 78, 78, 83, 83, 83, 83,
    88, 88, 88, 88, 93, 93, 93, 93,
    98, 98, 98, 98, 104, 104, 104, 104,
    110, 110, 110, 110, 116, 116, 116, 116,
    122, 122, 122, 122, 129, 129, 129, 129,
    135, 135, 135, 135, 142, 142, 142, 142,
    150, 150, 150, 150, 157, 157, 157, 157,
    165, 165, 165, 165, 172, 172, 172, 172,
    181, 181, 181, 181, 189, 189, 189, 189,
    198, 198, 198, 198, 207, 207, 207, 207,
    216, 216, 216, 216, 225, 225, 225, 225,
    235, 235, 235, 235, 245, 245, 245, 245,
    255, 255, 255, 255, 255, 255, 255, 255,
};
uint8_t SX1509B_WriteReg(uint8_t reg, uint8_t data)
{
  uint8_t res;
  uint8_t I2C_Data[2];
  I2C_Data[0] = reg;
  I2C_Data[1] = data;
  res =  HAL_I2C_Master_Transmit(&hi2c1,(SX1509B_I2C_ADDRESS << 1), I2C_Data, sizeof(I2C_Data), 1000);
  return res;
}
//  This function will write a two-byte word beginning at registerAddress
uint8_t SX1509B_WriteRegSequence(uint8_t reg, uint16_t data)
{
  uint8_t res;
  uint8_t I2C_Data[3];
  I2C_Data[0] = reg;
  I2C_Data[1] = (uint8_t)((data & 0xFF00) >> 8);
  I2C_Data[2] = (uint8_t)(data & 0x00FF);
  res =  HAL_I2C_Master_Transmit(&hi2c1,(SX1509B_I2C_ADDRESS << 1), I2C_Data, sizeof(I2C_Data), 1000);
  return res;
}
uint8_t SX1509B_ReadReg(uint8_t reg, uint8_t *data)
{
  uint8_t res;
  res =  HAL_I2C_Master_Transmit(&hi2c1,(SX1509B_I2C_ADDRESS << 1), &reg, 1, 10);
  if(res == HAL_OK)
  {
    res =  HAL_I2C_Master_Receive(&hi2c1,(SX1509B_I2C_ADDRESS << 1), data, 1, 10);
  }
  return res;
}
//  This function will read a two-byte word beginning at registerAddress
uint8_t SX1509B_ReadRegSequence(uint8_t reg, uint16_t *data)
{
  uint8_t res;
  uint16_t tempData;
  uint16_t low, high;
  res =  HAL_I2C_Master_Transmit(&hi2c1,(SX1509B_I2C_ADDRESS << 1), &reg, 1, 10);
  if(res == HAL_OK)
  {
    res =  HAL_I2C_Master_Receive(&hi2c1,(SX1509B_I2C_ADDRESS << 1), (uint8_t*)&tempData, 2, 10);
  }
  // Now tempData lsb is contain high byte portB, tempData msp is contain low byte portA
  // need to swap data
  high = (tempData & 0x00FF);
  low = ((tempData & 0xFF00) >> 8);
  *data = high << 8 | low;
  return res;
}
//uint8_t SX1509B_IOConfigOutput(uint16_t data)
//{
//  uint8_t res = HAL_OK;
//  SX1509B_IO_Control port;
//  port.PortTotal = data;
//  port.PortTotal ^= 0xFFFF; // inverted bit
//  res |= SX1509B_WriteReg(RegDirA, port.PortSelect.PortA);
//  res |= SX1509B_WriteReg(RegDirB, port.PortSelect.PortB);
//  return res;
//}
uint8_t SX1509B_HardReset(void)
{
  uint8_t res = HAL_OK;
  uint8_t regMisc = 0;
  res = SX1509B_ReadReg(RegMisc, &regMisc);
  if(res == 0)
  {
    printf("SX1509B read OK!!!\r\n");
  }
  else
  {
    printf("SX1509B read fail ! Error code: %d\r\n", res);
  }
  if (regMisc & (1 << 2))
  {
    regMisc &= ~(1 << 2);
    res = SX1509B_WriteReg(RegMisc, regMisc);
  }
  HAL_GPIO_WritePin(SX1509B_GPIO_Port, SX1509B_RST_Pin, GPIO_PIN_RESET);
  HAL_Delay(1000);
  HAL_GPIO_WritePin(SX1509B_GPIO_Port, SX1509B_RST_Pin, GPIO_PIN_SET);
  return res;
}
uint8_t SX1509B_RegReset(void)
{
  uint8_t res = HAL_OK;
  res |= SX1509B_WriteReg(RegReset, 0x12);
  res |= SX1509B_WriteReg(RegReset, 0x34);
  return res;
}
uint8_t SX1509B_SetClock(uint8_t oscSource /*= 2*/, uint8_t oscPinFunction /*= 0*/, uint8_t oscFreqOut /*= 0*/, uint8_t oscDivider /*= 1*/)
{
  // RegClock constructed as follows:
  //  6:5 - Oscillator frequency souce
  //    00: off, 01: external input, 10: internal 2MHz, 1: reserved
  //  4 - OSCIO pin function
  //    0: input, 1 ouptut
  //  3:0 - Frequency of oscout pin
  //    0: LOW, 0xF: high, else fOSCOUT = FoSC/(2^(RegClock[3:0]-1))
  uint8_t res = HAL_OK;
  uint8_t regMisc;
  oscSource = (oscSource & 0b11) << 5;    // 2-bit value, bits 6:5
  oscPinFunction = (oscPinFunction & 1) << 4; // 1-bit value bit 4
  oscFreqOut = (oscFreqOut & 0b1111);     // 4-bit value, bits 3:0
  uint8_t regClock = oscSource | oscPinFunction | oscFreqOut;
  res = SX1509B_WriteReg(RegClock, regClock);

  // Config RegMisc[6:4] with oscDivider
  // 0: off, else ClkX = fOSC / (2^(RegMisc[6:4] -1))
  //oscDivider = constrain(oscDivider, 1, 7);
  if(oscDivider < 1) oscDivider = 1;
  else if(oscDivider > 7) oscDivider = 7;
  SX1509B_CLK = 2000000.0 / (1 << (oscDivider - 1)); // Update private clock variable
  oscDivider = (oscDivider & 0b111) << 4;    // 3-bit value, bits 6:4

  res |= SX1509B_ReadReg(RegMisc, &regMisc);
  regMisc &= ~(0b111 << 4);
  regMisc |= oscDivider;
  SX1509B_WriteReg(RegMisc, regMisc);
  return res;
}
uint8_t SX1509B_WritePin(uint8_t pin, uint8_t highLow)
{
  uint8_t res = HAL_OK;
  uint16_t tempRegDir;
  uint16_t tempRegData;
  uint16_t tempPullUp;
  uint16_t tempPullDown;
  res = SX1509B_ReadRegSequence(RegDirB, &tempRegDir);

  if ((0xFFFF ^ tempRegDir) & (1 << pin)) // If the pin is an output, write high/low
  {
    res |= SX1509B_ReadRegSequence(RegDataB, &tempRegData);
    if (highLow)
    {
      tempRegData |= (1 << pin);
    }
    else
    {
      tempRegData &= ~(1 << pin);
    }
    res |= SX1509B_WriteRegSequence(RegDataB, tempRegData);
  }
  else // Otherwise the pin is an input, pull-up/down
  {
    res |= SX1509B_ReadRegSequence(RegPullUpB, &tempPullUp);
    res |= SX1509B_ReadRegSequence(RegPullDownB, &tempPullDown);

    if (highLow) // if HIGH, do pull-up, disable pull-down
    {
      tempPullUp |= (1 << pin);
      tempPullDown &= ~(1 << pin);
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempPullUp);
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempPullDown);
    }
    else // If LOW do pull-down, disable pull-up
    {
      tempPullDown |= (1 << pin);
      tempPullUp &= ~(1 << pin);
      res |= SX1509B_WriteRegSequence(RegPullUpB, tempPullUp);
      res |= SX1509B_WriteRegSequence(RegPullDownB, tempPullDown);
    }
  }
  return res;
}
uint8_t SX1509B_LedDriverInit(uint8_t pin, uint8_t freq /*= 1*/, bool log /*= false*/)
{
  uint8_t res = HAL_OK;
  uint16_t tempWord;
  uint8_t tempByte;

  // Disable input buffer
  // Writing a 1 to the pin bit will disable that pins input buffer
  res = SX1509B_ReadRegSequence(RegInputDisableB, &tempWord);
  tempWord |= (1 << pin);
  res |= SX1509B_WriteRegSequence(RegInputDisableB, tempWord);

  // Disable pull-up
  // Writing a 0 to the pin bit will disable that pull-up resistor
  res |= SX1509B_ReadRegSequence(RegPullUpB, &tempWord);
  tempWord &= ~(1 << pin);
  res |= SX1509B_WriteRegSequence(RegPullUpB, tempWord);

  // Set direction to output (REG_DIR_B)
  res |= SX1509B_ReadRegSequence(RegDirB, &tempWord);
  tempWord &= ~(1 << pin); // 0=output
  res |= SX1509B_WriteRegSequence(RegDirB, tempWord);

  // Enable oscillator (REG_CLOCK)
  res |= SX1509B_ReadReg(RegClock, &tempByte);
  tempByte |= (1 << 6);  // Internal 2MHz oscillator part 1 (set bit 6)
  tempByte &= ~(1 << 5); // Internal 2MHz oscillator part 2 (clear bit 5)
  res |= SX1509B_WriteReg(RegClock, tempByte);

  // Configure LED driver clock and mode (REG_MISC)
  res |= SX1509B_ReadReg(RegMisc, &tempByte);
  if (log)
  {
    tempByte |= (1 << 7); // set logarithmic mode bank B
    tempByte |= (1 << 3); // set logarithmic mode bank A
  }
  else
  {
    tempByte &= ~(1 << 7); // set linear mode bank B
    tempByte &= ~(1 << 3); // set linear mode bank A
  }

  // Use configClock to setup the clock divder
  if (SX1509B_CLK == 0) // Make clckX non-zero
  {
    // _clkX = 2000000.0 / (1 << (1 - 1)); // Update private clock variable
    SX1509B_CLK = 2000000.0;

    // uint8_t freq = (1 & 0x07) << 4; // freq should only be 3 bits from 6:4
    // tempByte |= freq;
  }

  freq = (freq & 0x7) << 4; // mask only 3 bits and shift to bit position 6:4
  tempByte |= freq;

  res |= SX1509B_WriteReg(RegMisc, tempByte);

  // Enable LED driver operation (REG_LED_DRIVER_ENABLE)
  res |= SX1509B_ReadRegSequence(RegLEDDriverEnableB, &tempWord);
  tempWord |= (1 << pin);
  res |= SX1509B_WriteRegSequence(RegLEDDriverEnableB, tempWord);

  // Set REG_DATA bit low ~ LED driver started
  res |= SX1509B_ReadRegSequence(RegDataB, &tempWord);
  tempWord &= ~(1 << pin);
  res |= SX1509B_WriteRegSequence(RegDataB, tempWord);
  return res;
}
uint8_t  SX1509B_LedBreathe(uint8_t pin, uint8_t IOnX, uint8_t TRiseX, uint8_t TOnX, uint8_t TFallX, uint8_t OffX)
{
//  uint16_t tempWord;
  uint8_t res = HAL_OK;
  uint8_t I2C_Data[6] = {0};
  uint8_t regStart = 0;
  if(pin == 4 || pin == 5 || pin == 6 || pin == 7 || pin == 12 || pin == 13 || pin == 14 || pin == 15)
  {
    if(pin <= 7)
    {
      regStart = RegTOn4 + (pin - 4) * 5;
    }
    else
    {
      regStart = RegTOn12 + (pin - 12) * 5;
    }
    printf("regStart = 0x%02X\n", regStart);
    I2C_Data[0] = regStart;
    I2C_Data[1] = TOnX;
    I2C_Data[2] = IOnX;
    I2C_Data[3] = OffX;
    I2C_Data[4] = TRiseX;
    I2C_Data[5] = TFallX;
    res |=  HAL_I2C_Master_Transmit(&hi2c1,(SX1509B_I2C_ADDRESS << 1), I2C_Data, sizeof(I2C_Data), 1000);
    // Set REG_DATA bit low ~ LED driver started
//    res |= SX1509B_ReadRegSequence(RegDataB, &tempWord);
//    tempWord &= ~(1 << pin);
//    res |= SX1509B_WriteRegSequence(RegDataB, tempWord);
    if(res == HAL_OK)
    {
      printf("Config Led breathe OK\n");
    }
  }
  else
  {
    res = 1;
  }

  return res;
}

uint8_t SX1509B_ConfigPinMode(uint8_t pin, uint8_t mode, uint8_t initialLevel)
{
  // The SX1509 RegDir registers: REG_DIR_B, REG_DIR_A
  //  0: IO is configured as an output
  //  1: IO is configured as an input
  uint8_t res = HAL_OK;
  uint8_t modeBit;
//  uint16_t tempRegData;
  uint16_t tempRegDir;
  if ((mode == EXTIO_OUTPUT) || (mode == EXTIO_ANALOG_OUTPUT) || (mode == EXTIO_OUTPUT_BREATHE))
  {
//    res = SX1509B_ReadRegSequence(RegDataB, &tempRegData);
//    if (initialLevel == LOW)
//    {
//      tempRegData &= ~(1 << pin);
//      res |= SX1509B_WriteRegSequence(RegDataB, tempRegData);
//    }
    modeBit = 0;
  }
  else
  {
    modeBit = 1;
  }

  res |= SX1509B_ReadRegSequence(RegDirB, &tempRegDir);
  if (modeBit)
  {
    tempRegDir |= (1 << pin);
  }

  else
  {
    tempRegDir &= ~(1 << pin);
  }
  res |= SX1509B_WriteRegSequence(RegDirB, tempRegDir);

  // If INPUT_PULLUP was called, set up the pullup too:
  if (mode == EXTIO_INPUT_PULLUP)
  {
    res |= SX1509B_WritePin(pin, HIGH);
  }
  else if (mode == EXTIO_ANALOG_OUTPUT)
  {
    res |= SX1509B_LedDriverInit(pin, 1, false);
  }
  else if(mode == EXTIO_OUTPUT_BREATHE)
  {
    res |= SX1509B_LedDriverInit(pin, 7, true);
    res |= SX1509B_LedBreathe(pin, 0xFF, 2, 2, 1, 2 << 3);
  }
  return res;
}
//uint8_t EXTIO_Init(GPIO_InitTypeDef GPIO_Init)
//{
//  //if(GPIO_Init.Mode == )
//
//}
uint8_t SX1509B_EnableInterrupt(uint8_t pin, uint8_t riseFall)
{
  uint8_t res = HAL_OK;
  uint8_t sensitivity = 0;
  uint8_t pinMask;

  uint8_t senseRegister;
  // Set REG_INTERRUPT_MASK
  uint16_t tempWord;
  res = SX1509B_ReadRegSequence(RegInterruptMaskB, &tempWord);
  tempWord &= ~(1 << pin); // 0 = event on IO will trigger interrupt
  res |= SX1509B_WriteRegSequence(RegInterruptMaskB, tempWord);

  // Set REG_SENSE_XXX
  // Sensitivity is set as follows:
  // 00: None
  // 01: Rising
  // 10: Falling
  // 11: Both
  switch (riseFall)
  {
  case CHANGE:
    sensitivity = 0b11;
    break;
  case FALLING:
    sensitivity = 0b10;
    break;
  case RISING:
    sensitivity = 0b01;
    break;
  }
  pinMask = (pin & 0x07) * 2;


  // Need to select between two words. One for bank A, one for B.
  if (pin >= 8)
  {
    senseRegister = RegSenseHighB;
  }
  else
  {
    senseRegister = RegSenseHighA;
  }
  res |= SX1509B_ReadRegSequence(senseRegister, &tempWord);
  tempWord &= ~(0b11 << pinMask);     // Mask out the bits we want to write
  tempWord |= (sensitivity << pinMask); // Add our new bits
  res |= SX1509B_WriteRegSequence(senseRegister, tempWord);
  return res;
}
uint8_t SX1509B_DebounceConfig(uint8_t configValue)
{
  // First make sure clock is configured
  uint8_t res = HAL_OK;
  uint8_t tempByte;
  res |= SX1509B_ReadReg(RegMisc, &tempByte);
  if ((tempByte & 0x70) == 0)
  {
    tempByte |= (1 << 4); // Just default to no divider if not set
    res |= SX1509B_WriteReg(RegMisc, tempByte);
  }
  res |= SX1509B_ReadReg(RegClock, &tempByte);
  if ((tempByte & 0x60) == 0)
  {
    tempByte |= (1 << 6); // default to internal osc.
    res |= SX1509B_WriteReg(RegClock, tempByte);
  }

  configValue &= 0b111; // 3-bit value
  res |= SX1509B_WriteReg(RegDebounceConfig, configValue);
  return res;
}

uint8_t SX1509B_DebounceTime(uint8_t ms)
{
  uint8_t res = HAL_OK;
  // Debounce time-to-byte map: (assuming fOsc = 2MHz)
  // 0: 0.5ms   1: 1ms
  // 2: 2ms   3: 4ms
  // 4: 8ms   5: 16ms
  // 6: 32ms    7: 64ms
  // 2^(n-1)
  uint8_t configValue = 0;
  if (SX1509B_CLK == 0)            // If clock hasn't been set up.
  // Set clock to 2MHz.
  {
    res |= SX1509B_SetClock(INTERNAL_CLOCK_2MHZ, CLK_INPUT, 0, 1);
  }

  // We'll check for the highest set bit position,
  // and use that for debounceConfig
  for (int8_t i = 7; i >= 0; i--)
  {
    if (ms & (1 << i))
    {
      configValue = i + 1;
      break;
    }
  }
  if(configValue > 7)
  {
    configValue = 7;
  }

  res |= SX1509B_DebounceConfig(configValue);
  return res;
}
uint16_t SX1509B_InterruptSource(bool clear /* =true*/)
{
  uint16_t intSource;
  (void)SX1509B_ReadRegSequence(RegInterruptSourceB, &intSource);
  if (clear)
  {
    (void)SX1509B_WriteRegSequence(RegInterruptSourceB, 0xFFFF); // Clear interrupts
  }
  return intSource;
}
bool SX1509B_CheckInterrupt(uint8_t pin)
{
  if (SX1509B_InterruptSource(true) & (1 << pin))
  {
    return true;
  }
  return false;
}
bool SX1509B_SearchInterruptPin(uint8_t *detectPin)
{
  ;
  uint16_t intSource;
  uint8_t i;
  intSource = SX1509B_InterruptSource(false);
  for(i = 0; i < 16; i++)
  {
    if(intSource & (1 << i)) break;
  }
  if(i < 16)
  {
    *detectPin = i;
    return true;
  }
  else
  {
    return false;
  }
}
uint8_t SX1509B_DebounceEnable(uint8_t pin)
{
  uint8_t res = HAL_OK;
  uint16_t debounceEnable;
  res |= SX1509B_ReadRegSequence(RegDebounceEnableB, &debounceEnable);
  debounceEnable |= (1 << pin);
  res |= SX1509B_WriteRegSequence(RegDebounceEnableB, debounceEnable);
  return res;
}
uint8_t SX1509B_Init(void)
{
  uint8_t res = HAL_OK;
  HAL_GPIO_WritePin(SX1509B_GPIO_Port, SX1509B_RST_Pin, GPIO_PIN_SET);
  res |= SX1509B_RegReset();
  //res |= SX1509B_HardReset();
  if(res == HAL_OK)
  {
    printf("SX1509B_RegReset complete !!!\r\n");
  }
  else
  {
    printf("SX1509B_RegReset fail ! Error code: %d\r\n", res);
  }
  res |= SX1509B_SetClock(INTERNAL_CLOCK_2MHZ, CLK_INPUT, 0, 1);


  res |= SX1509B_ConfigPinMode(LED_DOWN, EXTIO_ANALOG_OUTPUT, LOW);
  res |= SX1509B_ConfigPinMode(LED_UP, EXTIO_ANALOG_OUTPUT, LOW);
  res |= SX1509B_ConfigPinMode(LED_LEFT, EXTIO_ANALOG_OUTPUT, LOW);
  res |= SX1509B_ConfigPinMode(LED_RIGHT, EXTIO_ANALOG_OUTPUT, LOW);
  res |= SX1509B_ConfigPinMode(LED_CENTRE, EXTIO_ANALOG_OUTPUT, LOW);
  res |= SX1509B_WritePin(LED_RIGHT, LOW);
  res |= SX1509B_WritePin(LED_LEFT, LOW);
  res |= SX1509B_WritePin(LED_DOWN, LOW);
  res |= SX1509B_WritePin(LED_CENTRE, LOW);
  res |= SX1509B_WritePin(LED_UP, LOW);

  res |= SX1509B_ConfigPinMode(BTN_DOWN, EXTIO_INPUT_PULLUP, HIGH);
  res |= SX1509B_ConfigPinMode(BTN_UP, EXTIO_INPUT_PULLUP, HIGH);
  res |= SX1509B_ConfigPinMode(BTN_LEFT, EXTIO_INPUT_PULLUP, HIGH);
  res |= SX1509B_ConfigPinMode(BTN_RIGHT, EXTIO_INPUT_PULLUP, HIGH);
  res |= SX1509B_ConfigPinMode(BTN_CENTRE, EXTIO_INPUT_PULLUP, HIGH);

  res |= SX1509B_DebounceTime(32);
  res |= SX1509B_EnableInterrupt(BTN_DOWN, FALLING);
  res |= SX1509B_DebounceEnable(BTN_DOWN);

  res |= SX1509B_EnableInterrupt(BTN_UP, FALLING);
  res |= SX1509B_DebounceEnable(BTN_UP);

  res |= SX1509B_EnableInterrupt(BTN_LEFT, FALLING);
  res |= SX1509B_DebounceEnable(BTN_LEFT);

  res |= SX1509B_EnableInterrupt(BTN_RIGHT, FALLING);
  res |= SX1509B_DebounceEnable(BTN_RIGHT);

  res |= SX1509B_EnableInterrupt(BTN_CENTRE, FALLING);
  res |= SX1509B_DebounceEnable(BTN_CENTRE);
  HAL_Delay(2000);
  res |= SX1509B_WriteRegSequence(RegInterruptSourceB, 0xFFFF);
  //res |= SX1509B_WritePin(BTN_CENTRE, LOW);
  return res;
}

