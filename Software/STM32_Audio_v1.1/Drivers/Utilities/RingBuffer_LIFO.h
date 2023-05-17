/*
 * RingBuffer_LIFO.h
 *
 *  Created on: Mar 24, 2023
 *      Author: Duc Huy Le
 */

#ifndef UTILITIES_RINGBUFFER_LIFO_H_
#define UTILITIES_RINGBUFFER_LIFO_H_
#include <stdint.h>
#include <stdlib.h>
typedef enum
{
  IS_NOT_INITIALIZED = 0,
  IS_EMPTY,
  IS_FULLY,
  IS_CONTAIN,
}LIFO_STATUS;
typedef struct
{
  uint16_t *BaseAddress;
  volatile uint16_t *Cursor;
  volatile uint32_t Counter;
  uint32_t Size;
  LIFO_STATUS Status;

}RingBuffer_LIFO;

LIFO_STATUS RingBuffer_LIFO_Init(RingBuffer_LIFO *r, uint16_t *buff, uint32_t size);
LIFO_STATUS RingBuffer_LIFO_Push(RingBuffer_LIFO *Buffer, uint16_t data);
LIFO_STATUS RingBuffer_LIFO_Pop(RingBuffer_LIFO *Buffer, uint16_t *data);
LIFO_STATUS RingBuffer_LIFO_Status(RingBuffer_LIFO *Buffer);
#endif /* UTILITIES_RINGBUFFER_LIFO_H_ */
