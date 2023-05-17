/*
 * IO_Expand.h
 *
 *  Created on: Aug 13, 2022
 *      Author: Duc Huy Le
 */
#include <stdio.h>
#include <stdbool.h>
#ifndef IO_EXPAND_IO_EXPAND_H_
#define IO_EXPAND_IO_EXPAND_H_

#define IO_EXPAND_PIN_0     (uint16_t)1U << 0
#define IO_EXPAND_PIN_1     (uint16_t)1U << 1
#define IO_EXPAND_PIN_2     (uint16_t)1U << 2
#define IO_EXPAND_PIN_3     (uint16_t)1U << 3
#define IO_EXPAND_PIN_4     (uint16_t)1U << 4
#define IO_EXPAND_PIN_5     (uint16_t)1U << 5
#define IO_EXPAND_PIN_6     (uint16_t)1U << 6
#define IO_EXPAND_PIN_7     (uint16_t)1U << 7

#define IO_EXPAND_PIN_8     (uint16_t)1U << 8
#define IO_EXPAND_PIN_9     (uint16_t)1U << 9
#define IO_EXPAND_PIN_10    (uint16_t)1U << 10
#define IO_EXPAND_PIN_11    (uint16_t)1U << 11
#define IO_EXPAND_PIN_12    (uint16_t)1U << 12
#define IO_EXPAND_PIN_13    (uint16_t)1U << 13
#define IO_EXPAND_PIN_14    (uint16_t)1U << 14
#define IO_EXPAND_PIN_15    (uint16_t)1U << 15
typedef enum
{
  // mode output
  IO_EXPAND_MODE_OUTPUT        =         0x0001U,
  IO_EXPAND_MODE_OUTPUT_ANALOG   =       0x0002U,
  IO_EXPAND_MODE_OUTPUT_BREATHE   =      0x0004U,
  // mode input
  IO_EXPAND_MODE_INPUT    =              0x0008U,

  // mode interrupt
  IO_EXPAND_MODE_IT           =          0x0010U,
  IO_EXPAND_MODE_RISING        =         0x0020U,
  IO_EXPAND_MODE_FALLING       =         0x0040U,
  IO_EXPAND_MODE_RISING_FALLING =        0x0080U,

  IO_EXPAND_MODE_INPUT_IT_RISING = (IO_EXPAND_MODE_INPUT  | IO_EXPAND_MODE_RISING | IO_EXPAND_MODE_IT),
  IO_EXPAND_MODE_INPUT_IT_FALLING = (IO_EXPAND_MODE_INPUT | IO_EXPAND_MODE_FALLING | IO_EXPAND_MODE_IT),
}IO_EXPAND_MODE;
typedef enum
{
  // mode output
  IO_EXPAND_NO_PULL        =             0x0002U,
  IO_EXPAND_PULL_UP   =                  0x0004U,
  IO_EXPAND_PULL_DOWN   =                0x0008U,
}IO_EXPAND_PULL;

typedef enum
{
  PIN_RESET = 0,
  PIN_SET
}IO_Expand_PinState;
typedef enum
{
  LIN = 0,
  LOG
}LinOrLog;
typedef enum
{
  CLOCK_VERY_HIGH = 1,
  CLOCK_HIGH = 2,
  CLOCK_NORMAL = 4,
  CLOCK_SLOW = 5,
  CLOCK_VERY_SLOW = 7
}IO_EXPAND_CLOCK;
typedef struct
{
  uint16_t Pin;       /*!< Specifies the GPIO pins to be configured.
                           This parameter can be any value of @ref GPIO_pins_define */

  IO_EXPAND_MODE Mode;      /*!< Specifies the operating mode for the selected pins.
                           This parameter can be a value of @ref GPIO_mode_define */

  IO_EXPAND_PULL Pull;      /*!< Specifies the Pull-up or Pull-Down activation for the selected pins.
                           This parameter can be a value of @ref GPIO_pull_define */
  LinOrLog LinOrLog;

  IO_EXPAND_CLOCK IO_Clock;

}IO_Expand_Typedef;


uint8_t IO_Expand_Init(void);
#endif /* IO_EXPAND_IO_EXPAND_H_ */
