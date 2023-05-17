/*
 * SX1509B.h
 *
 *  Created on: Jul 24, 2022
 *      Author: Duc Huy Le
 */

#ifndef KEYPAD_SX1509B_B_H_
#define KEYPAD_SX1509B_B_H_
#include "main.h"
#include <stdbool.h>
// These are used for setting LED driver to linear or log mode:
#define LINEAR 0
#define LOGARITHMIC 1

// These are used for clock config:
#define INTERNAL_CLOCK_2MHZ 2
#define EXTERNAL_CLOCK 1

#define CLK_OUTPUT 1
#define CLK_INPUT 0

#define EXTIO_INPUT 0x00
#define EXTIO_INPUT_PULLUP 0x01
#define EXTIO_OUTPUT 0x02
#define EXTIO_ANALOG_OUTPUT 0x03 // To set a pin mode for PWM output
#define EXTIO_OUTPUT_BREATHE 0x04
#define LOW 0
#define HIGH 1
#define CHANGE 0x01
#define FALLING 0x02
#define RISING 0x04
/*!
 * SX1509B registers addresses
 */
#define RegReset                                    0x7D

#define RegInputDisableB                            0x00
#define RegInputDisableA                            0x01
#define RegLongSlewB                                0x02
#define RegLongSlewA                                0x03
#define RegLowDriveB                                0x04
#define RegLowDriveA                                0x05
#define RegPullUpB                                  0x06
#define RegPullUpA                                  0x07
#define RegPullDownB                                0x08
#define RegPullDownA                                0x09
#define RegOpenDrainB                               0x0A
#define RegOpenDrainA                               0x0B
#define RegPolarityB                                0x0C
#define RegPolarityA                                0x0D
#define RegDirB                                     0x0E
#define RegDirA                                     0x0F
#define RegDataB                                    0x10
#define RegDataA                                    0x11
#define RegInterruptMaskB                           0x12
#define RegInterruptMaskA                           0x13
#define RegSenseHighB                               0x14
#define RegSenseLowB                                0x15
#define RegSenseHighA                               0x16
#define RegSenseLowA                                0x17
#define RegInterruptSourceB                         0x18
#define RegInterruptSourceA                         0x19
#define RegEventStatusB                             0x1A
#define RegEventStatusA                             0x1B
#define RegLevelShifter1                            0x1C
#define RegLevelShifter2                            0x1D
#define RegClock                                    0x1E
#define RegMisc                                     0x1F
#define RegLEDDriverEnableB                         0x20
#define RegLEDDriverEnableA                         0x21
#define RegDebounceConfig                           0x22
#define RegDebounceEnableB                          0x23
#define RegDebounceEnableA                          0x24
#define RegKeyConfig1                               0x25
#define RegKeyConfig2                               0x26
#define RegKeyData1                                 0x27
#define RegKeyData2                                 0x28
#define RegTOn0                                     0x29
#define RegIOn0                                     0x2A
#define RegOff0                                     0x2B
#define RegTOn1                                     0x2C
#define RegIOn1                                     0x2D
#define RegOff1                                     0x2E
#define RegTOn2                                     0x2F
#define RegIOn2                                     0x30
#define RegOff2                                     0x31
#define RegTOn3                                     0x32
#define RegIOn3                                     0x33
#define RegOff3                                     0x34
#define RegTOn4                                     0x35
#define RegIOn4                                     0x36
#define RegOff4                                     0x37
#define RegTRise4                                   0x38
#define RegTFall4                                   0x39
#define RegTOn5                                     0x3A
#define RegIOn5                                     0x3B
#define RegOff5                                     0x3C
#define RegTRise5                                   0x3D
#define RegTFall5                                   0x3E
#define RegTOn6                                     0x3F
#define RegIOn6                                     0x40
#define RegOff6                                     0x41
#define RegTRise6                                   0x42
#define RegTFall6                                   0x43
#define RegTOn7                                     0x44
#define RegIOn7                                     0x45
#define RegOff7                                     0x46
#define RegTRise7                                   0x47
#define RegTFall7                                   0x48
#define RegTOn8                                     0x49
#define RegIOn8                                     0x4A
#define RegOff8                                     0x4B
#define RegTOn9                                     0x4C
#define RegIOn9                                     0x4D
#define RegOff9                                     0x4E
#define RegTOn10                                    0x4F
#define RegIOn10                                    0x50
#define RegOff10                                    0x51
#define RegTOn11                                    0x52
#define RegIOn11                                    0x53
#define RegOff11                                    0x54
#define RegTOn12                                    0x55
#define RegIOn12                                    0x56
#define RegOff12                                    0x57
#define RegTRise12                                  0x58
#define RegTFall12                                  0x59
#define RegTOn13                                    0x5A
#define RegIOn13                                    0x5B
#define RegOff13                                    0x5C
#define RegTRise13                                  0x5D
#define RegTFall13                                  0x5E
#define RegTOn14                                    0x5F
#define RegIOn14                                    0x60
#define RegOff14                                    0x61
#define RegTRise14                                  0x62
#define RegTFall14                                  0x63
#define RegTOn15                                    0x64
#define RegIOn15                                    0x65
#define RegOff15                                    0x66
#define RegTRise15                                  0x67
#define RegTFall15                                  0x68
#define RegHighInputB                               0x69
#define RegHighInputA                               0x6A

typedef union
{
  uint16_t PortTotal;
  struct
  {
    uint8_t PortA;
    uint8_t PortB;
  }PortSelect;
}SX1509B_IO_Control;
uint8_t SX1509B_WriteReg(uint8_t reg, uint8_t data);
uint8_t SX1509B_WriteRegSequence(uint8_t reg, uint16_t data);
uint8_t SX1509B_ReadReg(uint8_t reg, uint8_t *data);
uint8_t SX1509B_ReadRegSequence(uint8_t reg, uint16_t *data);
uint8_t SX1509B_WritePin(uint8_t pin, uint8_t highLow);
uint8_t SX1509B_HardReset(void);
uint8_t SX1509B_RegReset(void);
uint8_t SX1509B_WriteReg(uint8_t reg, uint8_t data);
uint8_t SX1509B_ReadReg(uint8_t reg, uint8_t *data);
uint8_t SX1509B_IOConfigOutput(uint16_t data);
uint8_t SX1509B_ConfigPinMode(uint8_t pin, uint8_t mode, uint8_t initialLevel);
uint8_t SX1509B_SetClock(uint8_t oscSource /*= 2*/, uint8_t oscPinFunction /*= 0*/, uint8_t oscFreqOut /*= 0*/, uint8_t oscDivider /*= 1*/);
//uint8_t  SX1509B_LedBreathe(uint8_t pin, uint16_t pwmHigh, uint16_t pwmLow, uint16_t timeRise, uint16_t timeOn, uint16_t timeFall, uint16_t timeOff);
uint8_t SX1509B_Init(void);
uint8_t SX1509B_WriteRegSequence(uint8_t reg, uint16_t data);
uint8_t SX1509B_EnableInterrupt(uint8_t pin, uint8_t riseFall);
bool SX1509B_CheckInterrupt(uint8_t pin);
bool SX1509B_SearchInterruptPin(uint8_t *detectPin);
uint8_t SX1509B_DebounceConfig(uint8_t configValue);
uint8_t SX1509B_DebounceTime(uint8_t ms);
uint8_t  SX1509B_LedBreathe(uint8_t pin, uint8_t IOnX, uint8_t TRiseX, uint8_t TOnX, uint8_t TFallX, uint8_t OffX);
#endif /* KEYPAD_SX1509B_B_H_ */
