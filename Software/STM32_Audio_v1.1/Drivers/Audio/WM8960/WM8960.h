/*
 * WM8960.h
 *
 *  Created on: Jul 14, 2022
 *      Author: Duc Huy Le
 */

#ifndef __WM8960_H__
#define __WM8960_H__

#include "stm32f4xx_hal.h"

#define AUDIO_USE_I2S
#define DMA_MAX_SZE     0xFFFF
#define DMA_MAX(x)      (((x) <= 0xFFFF)? (x):0xFFFF)
#define AUDIODATA_SIZE  2   /* 16-bits audio data size */

//#define AUIDO_START_ADDRESS       0 /* Offset relative to audio file header size */
#define AUIDO_START_ADDRESS       58 /* Offset relative to audio file header size */

extern uint32_t AudioRemSize;
extern uint16_t *CurrentPos;

uint8_t WM8960_Write_Reg(uint8_t reg, uint16_t dat);
uint16_t WM8960_Read_Reg(uint8_t reg);
uint8_t WM8960_Init(void);
uint8_t WM8960_Init_PLL(void);
uint32_t AudioFlashPlay(uint16_t* pBuffer, uint32_t FullSize, uint32_t StartAdd);
uint32_t AUDIO_Play(uint16_t* pBuffer, uint32_t Size);
void CalcLenghtOfWaveData(void);
#endif

