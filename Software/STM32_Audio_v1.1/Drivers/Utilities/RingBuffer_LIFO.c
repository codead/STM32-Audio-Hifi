/*
 * RingBuffer_LIFO.c
 *
 *  Created on: Mar 24, 2023
 *      Author: Duc Huy Le
 */

#include <RingBuffer_LIFO.h>
#include <stdio.h>
LIFO_STATUS RingBuffer_LIFO_Init(RingBuffer_LIFO *Buffer, uint16_t *buff, uint32_t size)
{
  if(Buffer == NULL || buff == NULL || size < 2)
  {
    Buffer->Status = IS_NOT_INITIALIZED;
  }
  else
  {
    Buffer->BaseAddress = buff;
    Buffer->Cursor = Buffer->BaseAddress - 1;
    Buffer->Counter = 0;
    Buffer->Size = size;
    Buffer->Status = IS_EMPTY;
  }
  if(Buffer->Status != IS_NOT_INITIALIZED)
  {
    printf("Buffer is Init OK\n");
  }
  return Buffer->Status;
}

LIFO_STATUS RingBuffer_LIFO_Push(RingBuffer_LIFO *Buffer, uint16_t data)
{
  // Push data to buffer
  Buffer->Cursor++;
  if(Buffer->Cursor >= Buffer->BaseAddress + Buffer->Size)
  {
    Buffer->Cursor = Buffer->BaseAddress; //Full buffer, roll back
  }
  *Buffer->Cursor = data;

  // Increase counter of buffer
  Buffer->Counter++;
  // Check current size
  if(Buffer->Counter >= Buffer->Size)
  {
    Buffer->Counter = Buffer->Size; // Keep counter buffer do not over
    Buffer->Status = IS_FULLY; // Notice fully
  }
  else
  {
    Buffer->Status = IS_CONTAIN;
  }

  return Buffer->Status;
}

LIFO_STATUS RingBuffer_LIFO_Pop(RingBuffer_LIFO *Buffer, uint16_t *data)
{
  if(Buffer->Status != IS_EMPTY)
  {
    // Pop data from buffer
    *data = *Buffer->Cursor;
    Buffer->Cursor--;
    if(Buffer->Cursor < Buffer->BaseAddress)
    {
      Buffer->Cursor = Buffer->BaseAddress + Buffer->Size - 1;
    }
    // Decrease counter of buffer
    Buffer->Counter--;
    // Check current size
    if(Buffer->Counter == 0)
    {
      Buffer->Status = IS_EMPTY; // Notice fully
    }
  }
  return Buffer->Status;
}

LIFO_STATUS RingBuffer_LIFO_Status(RingBuffer_LIFO *Buffer)
{
  return Buffer->Status;
}

