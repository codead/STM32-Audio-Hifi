/*
 * Utilities.c
 *
 *  Created on: Nov 17, 2022
 *      Author: Duc Huy Le
 */
#include "tim.h"
#include <stdio.h>
#include "Utilities.h"

#if (SELECT_RANDOM == USE_MANUAL_TIMER)
volatile uint32_t variableCounter = 0;
void IncreaseCounter(void)
{
  variableCounter++;
//  printf("variableCounter = %ld\n", variableCounter);
  if(variableCounter == 99999)
  {
    variableCounter = 0;
  }
}
#endif
uint32_t RandomVariable(uint32_t min, uint32_t max)
{
  uint32_t randomVariable = 0;
#if (SELECT_RANDOM == USE_MANUAL_TIMER)
  randomVariable = variableCounter;
#elif (SELECT_RANDOM == USE_SYSTEM_TIMER)
  randomVariable = HAL_GetTick();
#endif
  if(max > min)
  {
    randomVariable = min + (randomVariable % (max - min));
  }
  else
  {
    randomVariable = min;
  }
  return randomVariable;
}
uint32_t RandomVariableWithoutValue(uint32_t min, uint32_t max, uint32_t value)
{
  uint32_t randomVariable = 0;
  do{
#if (SELECT_RANDOM == USE_MANUAL_TIMER)
    randomVariable = variableCounter;
#elif (SELECT_RANDOM == USE_SYSTEM_TIMER)
    randomVariable = HAL_GetTick();
#endif
    if(max > min)
    {
      randomVariable = min + (randomVariable % (max - min));
    }
    else
    {
      randomVariable = min;
    }
  }while(value == randomVariable);
  return randomVariable;
}
uint8_t BatteryPercentCalculation(float voltage)
{
  // 4.2v --> 100%
  // 3.5v --> 0%
  // step 0.007v --> 1%
  // 3.8v ==> (3.8 - 3.5) / 0.007
  return (voltage >= 3.5 ? (uint8_t)((voltage - 3.5) / 0.007) : 0);
}
