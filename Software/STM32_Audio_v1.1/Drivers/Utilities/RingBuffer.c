/*
 * RingBuffer.c
 *
 *  Created on: Mar 23, 2023
 *      Author: Duc Huy Le
 */
#include "RingBuffer.h"
#ifndef NULL
#define NULL 0
#endif


int16_t RingBuffer_Init(RingBuffer* r, uint8_t* buff , int32_t size)
{
  if(r == NULL || buff == NULL || size < 2) return -1;
  r->buff_original = r->buff_read = r->buff_write = buff;
  r->fill_cnt = 0;
  r->size = size;
  return 0;
}

int16_t RingBuffer_Write(RingBuffer *r , uint8_t c)
{
  if(r->fill_cnt >= r->size ) return -1;
//  EnterCritical();
  r->fill_cnt++; // increase buffer
//  LeaveCritical();
  *r->buff_write++ = c ;
  if(r->buff_write >= r->buff_original + r->size) r->buff_write = r->buff_original; // full buffer, roll back
  return 0;
}
int16_t RingBuffer_Read(RingBuffer *r , uint8_t *c)
{
  if(r->fill_cnt <= 0) return -1;
//  EnterCritical();
  r->fill_cnt--; //decrease
//  LeaveCritical();
  *c = *r->buff_read++;
  if( r->buff_read >= r->buff_original + r->size) r->buff_read = r->buff_original;
  return 0;
}

int16_t RingBuffer_Flush(RingBuffer *r)
{
//  EnterCritical();
  r->buff_read = r->buff_write = r->buff_original;
  r->fill_cnt = 0;
//  LeaveCritical();
  return 0;
}

int16_t RingBuffer_Backup(RingBuffer *r)
{
  r->pre_buff_read = r->buff_read;
  r->pre_fill_cnt = r->fill_cnt;
  return 0;
}
int16_t RingBuffer_Recovery(RingBuffer *r)
{
  r->buff_read = r->pre_buff_read;
  r->fill_cnt = r->pre_fill_cnt;
  return 0;
}
