/*
 * RingBuffer.h
 *
 *  Created on: Mar 23, 2023
 *      Author: Duc Huy Le
 */

#ifndef RINGBUFFER_RINGBUFFER_H_
#define RINGBUFFER_RINGBUFFER_H_

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
  uint8_t* buff_original; // original buffer
  uint8_t* volatile buff_read ; // read buffer
  uint8_t* volatile buff_write; // write buffer
  volatile int32_t fill_cnt; // number of filled slot
  int32_t size;  // buffer size

  uint8_t* volatile pre_buff_read;
  volatile int32_t pre_fill_cnt;

}RingBuffer;

int16_t RingBuffer_Init( RingBuffer* r, uint8_t* buff , int32_t);
int16_t RingBuffer_Write(RingBuffer *r , uint8_t c);
int16_t RingBuffer_Read(RingBuffer *r , uint8_t *c);
int16_t RingBuffer_Flush(RingBuffer *r);

int16_t RingBuffer_Backup(RingBuffer *r);
int16_t RingBuffer_Recovery(RingBuffer *r);

#endif /* RINGBUFFER_RINGBUFFER_H_ */
