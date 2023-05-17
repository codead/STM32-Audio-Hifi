/*
 * Utilities.h
 *
 *  Created on: Nov 17, 2022
 *      Author: Duc Huy Le
 */

#ifndef UTILITIES_UTILITIES_H_
#define UTILITIES_UTILITIES_H_

#define USE_SYSTEM_TIMER        0
#define USE_MANUAL_TIMER        1

#define SELECT_RANDOM     USE_SYSTEM_TIMER

#if (SELECT_RANDOM == USE_MANUAL_TIMER)
void IncreaseCounter(void);
#endif
uint32_t RandomVariable(uint32_t min, uint32_t max);
uint32_t RandomVariableWithoutValue(uint32_t min, uint32_t max, uint32_t value);
uint8_t BatteryPercentCalculation(float voltage);
#endif /* UTILITIES_UTILITIES_H_ */
