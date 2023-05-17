/*
 * Volume.c
 *
 *  Created on: Nov 21, 2022
 *      Author: Duc Huy Le
 */

#include "adc.h"
#include <stdio.h>
#include <stdlib.h>
#include "WM8960.h"
#include "Draw.h"
#include "Display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
extern osSemaphoreId binSemaphoreDisplayHandle;
extern DISPLAY Display;;
#define VOLUME_ODD  1
#define HEADPHONE_VOLUME_UPDATE 1 << 8 // bit 8 of 02H, 03H register WM8960
uint32_t volumeDigital = 0; // volume value in range 0-4095 (12 bits)
uint32_t volatile volumeSet = 0;
uint32_t volumePreSet = 0;
uint16_t volumeSetToWM8960 = 0;
// Convert Digital to raw value (0-100)
// volumeDigital       raw value                        WM8960 value HP_L, HP_R, bits[6..0]
// 0                   100                              1111111b = 127
// 4095                0                                0101111b = 47 (Mute)
// z                   100 - (z * 100 / 4095) = x       y = ax + b = 0.8 * x + 47

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{

}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
  // Conversion Complete & DMA Transfer Complete As Well
  // Convert to volumeSet
  volumeSet =  100 - (volumeDigital * 100) / 4095;
  //printf("Volume Digital = %ld\n", volumeSet);
  if(abs(volumeSet - volumePreSet) <= VOLUME_ODD) return; // if ODD is insignificant, don't need to set new volume to WM8960
  volumePreSet = volumeSet;
  //Else Update HP_L and HP_R OUTPUTS volume
  volumeSetToWM8960 = volumeSet * 8 / 10 + 47;
  printf("Set volume = %ld\n", volumeSet);
  WM8960_Write_Reg(0x02, HEADPHONE_VOLUME_UPDATE | volumeSetToWM8960);  //LOUT1 Volume Set
  WM8960_Write_Reg(0x03, HEADPHONE_VOLUME_UPDATE | volumeSetToWM8960);  //ROUT1 Volume Set
  if(Display.Mode == PLAY_MUSIC)
  {
    osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
    Draw_Volume();
    osSemaphoreRelease(binSemaphoreDisplayHandle);
    Display.TimeForDisplayVolume = 0;
    Display.NeedToInvisibleVolume = true;
  }
}
