/*
 * Play_wav.c
 *
 *  Created on: Jul 20, 2022
 *      Author: Duc Huy Le
 */
#include "Play_wav.h"

#include "string.h"
#include "WM8960.h"
#include "i2s.h"
#include "i2c.h"
#include "gpio.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2s.h"
#include <stdio.h>
#include "Display.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "RingBuffer_LIFO.h"
extern DISPLAY Display;
extern TIM_HandleTypeDef htim2;
extern osThreadId playAudioTaskHandle;
extern osSemaphoreId binSemaphoreDisplayHandle;
extern uint8_t SCREEN_MUSIC_MAX_LIST;
uint8_t CloseFileFlag;    //1:already open file have to close it.

uint8_t EndFileFlag;      //1:reach the wave file end;
                          //2:wait for last transfer;
                          //3:finish transfer stop dma.

__IO uint8_t FillBufFlag; //0:fill first half buf;
                          //1:fill second half buf;
                          //0xFF:do nothing.

FIL WAV_File;             //File struct

wavctrl WaveCtrlData;     //Play control struct

uint32_t Audio_LastBytes; //The WAV file last bytes to play

uint8_t WAV_Buffer[WAV_BUFFER_SIZE];  //The buffer to ache WAV data
//uint8_t WAV_HalfBuffer[WAV_BUFFER_SIZE / 2]; // //The buffer to preparing for WAV_Buffer
uint16_t *WAV_Buffer_16 = (uint16_t *)WAV_Buffer;

uint32_t WAV_OFFSET;      //Offset of record FIL pointer in WAV file

uint32_t WAV_LastData;    //The size last data to be played

volatile I2S_CallBack_Flag I2S_Flag; //I2S CallBack function flag.
AUDIO Audio =
{
  .PlayList = {{0}},
  .MaxSong = 0,
  .NewSongEvent = AUTO,
  .CurrentSong = 0,
  .PreviousSong = 0,
  .PlaySong = false,
  .EndSong = false,
  .PauseSong = false
};
RingBuffer_LIFO PreviousSong_List;
uint16_t ListBuff[10];
_Alignas(8) uint8_t TempBuf[WAV_BUFFER_SIZE/2];
uint8_t *WAV_HalfBuffer = (uint8_t*) TempBuf;

void Audio_CreatePreviousSongList(void)
{
  RingBuffer_LIFO_Init(&PreviousSong_List, ListBuff, sizeof(ListBuff) / 2);
  // Add at least one previous song to avoid empty
  RingBuffer_LIFO_Push(&PreviousSong_List, Audio.PreviousSong);
}
LIFO_STATUS Audio_AddPreviousSong(void)
{
  LIFO_STATUS res;
  res = RingBuffer_LIFO_Push(&PreviousSong_List, Audio.PreviousSong);
  return res;
}
LIFO_STATUS Audio_GetPreviousSong(void)
{
  LIFO_STATUS res;
  res = RingBuffer_LIFO_Pop(&PreviousSong_List, &Audio.PreviousSong);
  return res;
}
/**
  * @brief  Scan the WAV files that set the path.
  * @param  path: Path to scan.
  * @retval None
  */
FRESULT ScanWavefiles(char* path) {

  FRESULT res;
  FILINFO fno;
  DIR dir;
  uint16_t i,j;

  res = f_opendir(&dir, path);    //Open the directory
  if(res != FR_OK)  {
    printf("f_opendir error !\r\n");
    return res;
  }

  for(i=0;;i++) {                 //Scan the files in the directory
    res = f_readdir(&dir, &fno);  //read a item
    if(res != FR_OK)  {
      printf("f_readdir error !\r\n");
      return res;
    }
    if(fno.fname[0] == 0)         //scan to the end of the path
      break;

    for(j=0;j<_MAX_LFN;j++) {
      if(fno.fname[j] == '.')     //Check if the type of the file is WAV
      break;
    }

    if(((fno.fname[j+1] == 'w')||(fno.fname[j+1] == 'W'))
        &&((fno.fname[j+2] == 'a')||(fno.fname[j+2] == 'A'))
        &&((fno.fname[j+3] == 'v')||(fno.fname[j+3] == 'V'))) //The file is WAV
    {
      strcpy(Audio.PlayList[i], path);     //Copy type of file is WAV
      strcat(Audio.PlayList[i],"/");       // Add '/' to the buffer
      strcat(Audio.PlayList[i],fno.fname); // Add file name to the buffer
      printf("%s\r\n", Audio.PlayList[i]); // print the whole file path to the UART
    }
  }
  res = f_closedir(&dir);             //Close the directory
  if(res != FR_OK)  {
    printf("f_closedir error !\r\n");
    return res;
  }

  Audio.MaxSong = i;

  printf("Scan WAV Files complete ! Audio.CurrentSong = %d\r\n",Audio.MaxSong);
  SCREEN_MUSIC_MAX_LIST = (Audio.MaxSong >= SCREEN_MAX_LIST) ? SCREEN_MAX_LIST : Audio.MaxSong;
  return res;
}


uint8_t Get_WAV_Message(char* fname, wavctrl* wavx) {

  uint8_t res = 0;
  UINT br = 0;

  ChunkRIFF *riff;
  ChunkFMT *fmt;
  ChunkFACT *fact;
  ChunkDATA *data;

  res = f_open(&WAV_File, (TCHAR *)fname, FA_READ);     //Open the file
  if(res == FR_OK) {

    CloseFileFlag = 1;

    f_read(&WAV_File, TempBuf, WAV_BUFFER_SIZE/2, &br); //Read WAV_BUFFER_SIZE/2 bytes data

    riff = (ChunkRIFF *)TempBuf;      //Get RIFF Chunk

    if(riff->Format == 0x45564157)  { //Format = "WAV"

      fmt = (ChunkFMT *)(TempBuf+12); //Get FMT Chunk
      if(fmt->AudioFormat==1||fmt->AudioFormat==3)        //Linear PCM or 32 bits WAVE
      {
        fact=(ChunkFACT *)(TempBuf+12+8+fmt->ChunkSize);  //Read FACT chunk

        if((fact->ChunkID == 0x74636166)||(fact->ChunkID==0X5453494C))
          wavx->datastart=12+8+fmt->ChunkSize+8+fact->ChunkSize;  //When there is fact/LIST Chunk.
        else
          wavx->datastart=12+8+fmt->ChunkSize;
        data = (ChunkDATA *)(TempBuf+wavx->datastart);
        if(data->ChunkID==0X61746164) {           //Read DATA Chunk success
          wavx->audioformat=fmt->AudioFormat;     //Audio Format
          wavx->nchannels=fmt->NumOfChannels;     //channel number
          wavx->samplerate=fmt->SampleRate;       //Sample Rate
          wavx->bitrate=fmt->ByteRate*8;          //Byte Rate
          wavx->blockalign=fmt->BlockAlign;       //Block Align
          wavx->bps=fmt->BitsPerSample;           //number of chunk, 8/16/24/32 bits
          wavx->datasize=data->ChunkSize;         //Size of audio data chunk
          wavx->datastart=wavx->datastart+8;      //data stream start offest
          wavx->totsec = data->ChunkSize / fmt->ByteRate;
          // fixed
          wavx->datastart=280;
          printf("WAV.audioformat:%d\n",wavx->audioformat);
          printf("WAV.nchannels:%d\n",wavx->nchannels);
          printf("WAV.samplerate:%ld\n",wavx->samplerate);
          printf("WAV.bitrate:%ld\n",wavx->bitrate);
          printf("WAV.blockalign:%d\n",wavx->blockalign);
          printf("WAV.bps:%d\n",wavx->bps);
          printf("WAV.datasize:%ld\n",wavx->datasize);
          printf("WAV.datastart:%ld\n",wavx->datastart);
          printf("WAV.totsec %ld:%02ld\n",wavx->totsec / 60, wavx->totsec % 60);
        }
        else  {
          printf("Not find data chunk !!\r\n");
          printf("data->ChunkID = 0x%lx\r\n",data->ChunkID);
          res = 4;
        }
      }
      else  {
        printf("Not linear PCM, not support !!\r\n");
        res = 3;
      }
    }
    else  {
      printf("Not WAV file !!\r\n");
      res = 2;
    }
  }
  else  {
    printf("Get_WAV_Message.f_open error !!\r\n");
    res = 1;
  }
  WAV_LastData = wavx->datasize;

  return res;
}

/**
  * @brief  Open the WAV file, get the message of the file.
  * @param  BUFF: the pointer of the buffer to cached data.
  * @param  size: the byte mumber of data.
  * @retval None
  */
FRESULT Fill_WAV_Buffer(uint8_t *BUFF, uint16_t size) {

  uint32_t NeedReadSize=0;
  uint32_t ReadSize;
  uint32_t i;
  uint8_t *p;
  float *f;
  int sound;
  FRESULT res = FR_OK;
  //It has been read last time, return.
  if(EndFileFlag==1)
    return FR_OK;

  if(WaveCtrlData.nchannels==2) {
    if(WaveCtrlData.bps == 16)          //16-bit audio,read data directly
    {

      res = f_read(&WAV_File,BUFF,size,(UINT*)&ReadSize);
    }
    else if(WaveCtrlData.bps==24)       //24-bit audio, adjust the order between the read data and the DMA cache
    {
      printf("WaveCtrlData.bps = %d\r\n",WaveCtrlData.bps);
      NeedReadSize=(size/4)*3;                                  //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      p=TempBuf;
      ReadSize=(ReadSize/3)*4;                                  //Size of data after fill
      //printf("%d\r\n",ReadSize);
      for(i=0;i<ReadSize;)
      {
        BUFF[i]=p[1];
        BUFF[i+1]=p[2];
        BUFF[i+2]=0;
        BUFF[i+3]=p[0];
        i+=4;
        p+=3;
      }
    }
    else if(WaveCtrlData.bps == 8)      //8-bit audio, data need to be transformed to 16-bit mode before play
    {
      printf("WaveCtrlData.bps = %d\r\n",WaveCtrlData.bps);
      NeedReadSize=size/2;                                      //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      p=TempBuf;
      ReadSize=ReadSize*2;                                      //Size of data after fill
      for(i=0;i<ReadSize;)
      {
        BUFF[i]=0;
        BUFF[i+1]=*p+0x80;
        p++;
        i=i+2;
      }
    }
    else if(WaveCtrlData.bps == 32)     //32bit WAVE, floating-point numbers [(-1) ~ 1] to represent sound
    {
      printf("WaveCtrlData.bps = %d\r\n",WaveCtrlData.bps);
      res = f_read(&WAV_File,TempBuf,size,(UINT*)&ReadSize);          //Read data
      f=(float*)TempBuf;
      for(i=0;i<ReadSize;)
      {
        //printf("f=%f\r\n",*f);
        sound=0x7FFFFFFF*(*f);
        BUFF[i]=(uint8_t)(sound>>16);
        BUFF[i+1]=(uint8_t)(sound>>24);
        BUFF[i+2]=(uint8_t)(sound);
        BUFF[i+3]=(uint8_t)(sound>>8);
        f++;
        i=i+4;
      }
    }
    else  {
      printf("WaveCtrlData.bps = %d\r\n",WaveCtrlData.bps);
      printf("Error !!\r\n");
    }
  }
  //Signal channel��adjust to dual channel data for playback
  else
  {
    if(WaveCtrlData.bps==16)
    {
      NeedReadSize=size/2;                                      //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      p=TempBuf;
      ReadSize=ReadSize*2;                                      //Size of data after fill
      for(i=0;i<ReadSize;)
      {
        BUFF[i]=p[0];
        BUFF[i+1]=p[1];
        BUFF[i+2]=p[0];
        BUFF[i+3]=p[1];
        i+=4;
        p=p+2;
      }
    }
    else if(WaveCtrlData.bps==24)                                 //24-bit audio
    {
      NeedReadSize=(size/8)*3;                                  //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      p=TempBuf;
      ReadSize=(ReadSize/3)*8;                                  //Size of data after fill
      for(i=0;i<ReadSize;)
      {
        BUFF[i]=p[1];
        BUFF[i+1]=p[2];
        BUFF[i+2]=0;
        BUFF[i+3]=p[0];
        BUFF[i+4]=p[1];
        BUFF[i+5]=p[2];
        BUFF[i+6]=0;
        BUFF[i+7]=p[0];
        p+=3;
        i+=8;
      }
    }
    else if(WaveCtrlData.bps==8)                                //8-bit audio
    {
      NeedReadSize=size/4;                                      //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      p=TempBuf;
      ReadSize=ReadSize*4;                                      //Size of data after fill
      for(i=0;i<ReadSize;)
      {
        BUFF[i]=0;
        BUFF[i+1]=*p+0x80;
        BUFF[i+2]=0;
        BUFF[i+3]=*p+0x80;
        i+=4;
        p++;
      }
    }
    else                                                        //32-bit audio
    {
      NeedReadSize=size/2;                                      //Number of bytes to read
      res = f_read(&WAV_File,TempBuf,NeedReadSize,(UINT*)&ReadSize);  //Read data
      f=(float*)TempBuf;
      ReadSize=ReadSize*2;                                      //Size of data after fill
      for(i=0;i<ReadSize;)
      {
        sound=0x7FFFFFFF*(*f);
        BUFF[i+4] = BUFF[i]   = (uint8_t)(sound>>16);
        BUFF[i+5] = BUFF[i+1] = (uint8_t)(sound>>24);
        BUFF[i+6] = BUFF[i+2] = (uint8_t)(sound);
        BUFF[i+7] = BUFF[i+3] = (uint8_t)(sound>>8);
        f++;
        i=i+8;
      }
    }
  }
  if(ReadSize<size)   //Data is not enough, supplementary '0'
  {
    EndFileFlag=1;
    for(i=ReadSize;i<size-ReadSize;i++)
      BUFF[i] = 0;
  }
  res |= f_sync(&WAV_File);
  return res;
}

/**
  * @brief  Play the WAV file that set the path.
  * @param  path: Path of the WAV file.
  * @retval None
  */
uint8_t PlayWaveFile(void)
{
  static uint32_t stabilityTest = 0;

  uint8_t res;
  FRESULT f_res;
  uint8_t retry;
  static uint16_t current_MusicNum = 0;
  CloseFileFlag = 0;
  EndFileFlag = 0;
  FillBufFlag = 0xFF;
  Audio.PlaySong = false;
  Audio.EndSong = false;
  I2S_Flag = I2S_No_CallBack;
  osSemaphoreWait(binSemaphoreDisplayHandle, osWaitForever);
  current_MusicNum = Audio.CurrentSong;
  printf("Now Playing: %s\r\n",Audio.PlayList[current_MusicNum]);
  Get_WAV_Message(Audio.PlayList[current_MusicNum],&WaveCtrlData);    //Get the messages of the WAV file
  Display.DurationTime = WaveCtrlData.totsec;
  Display.CurrentTime = 0;
  printf("Display.DurationTime = %ld, Display.CurrentTime = %ld\n", Display.DurationTime, Display.CurrentTime);
  HAL_TIM_Base_Start_IT(&htim2);
  osSemaphoreRelease(binSemaphoreDisplayHandle);
  WAV_OFFSET = WaveCtrlData.datastart;

  /*Start Play Music*/
  f_lseek(&WAV_File, WAV_OFFSET);
  Fill_WAV_Buffer(WAV_Buffer, WAV_BUFFER_SIZE);
  WAV_OFFSET += WAV_BUFFER_SIZE;
  while(Audio.EndSong == false)
  {
    while(Audio.PlaySong == true)
    {
      //osThreadSuspend(updateScreenLCDTaskHandle);
      osThreadSetPriority(playAudioTaskHandle, osPriorityRealtime);
      retry = 5;
      do
      {
        // preparing data to transmit before DMA completed
        f_res = f_lseek(&WAV_File, WAV_OFFSET);
        f_res |= Fill_WAV_Buffer(WAV_HalfBuffer, WAV_BUFFER_SIZE / 2);
      }
      while(f_res != FR_OK && --retry);// try 5 times to make sure read buffer of SD Card
      if(f_res != FR_OK)
      {
        printf("Failed times: %ld\r\n", ++stabilityTest);
        //break;
      }
      WAV_OFFSET += WAV_BUFFER_SIZE / 2;
      I2S_Flag = I2S_No_CallBack;
      osThreadSetPriority(playAudioTaskHandle, osPriorityNormal);
      //osThreadResume(updateScreenLCDTaskHandle);
      while(I2S_Flag == I2S_No_CallBack)
      {
        // waiting for DMA transmit completed
        asm("NOP");
      }
      if(I2S_Flag == I2S_Half_Callback)
      {
        // Copy half first buffer to WAV_Buffer
        memcpy(WAV_Buffer, WAV_HalfBuffer, WAV_BUFFER_SIZE / 2);
      }
      else if(I2S_Flag == I2S_Callback)
      {
        // Copy half remaining buffer to WAV_Buffer
        memcpy(WAV_Buffer + WAV_BUFFER_SIZE / 2, WAV_HalfBuffer, WAV_BUFFER_SIZE / 2);
        WAV_LastData -= WAV_BUFFER_SIZE;
      }
      else
      {
        //Key_Control();
      }


    }
    Audio.PlaySong = true;
#ifdef AUDIO_USE_I2S
    HAL_I2S_Transmit_DMA(&hi2s5,WAV_Buffer_16, WAV_BUFFER_SIZE / 2);
#elif AUDIO_USE_SAI
    HAL_SAI_Transmit_DMA(&hsai_BlockB1,WAV_Buffer, WAV_BUFFER_SIZE/2);
#endif
  }
  printf("End Play: %s\r\n",Audio.PlayList[current_MusicNum]);
  res = f_close(&WAV_File);

  return res;
}

//uint8_t PlayWaveFile(void)  {
//
//  uint8_t res;
//  static uint16_t current_MusicNum = 0;
//  CloseFileFlag = 0;
//  EndFileFlag = 0;
//  FillBufFlag = 0xFF;
//  Audio.PlaySong = 0;
//  Audio.EndSong = false;
//  I2S_Flag = I2S_No_CallBack;
//  current_MusicNum = Audio.CurrentSong;
//  printf("Now Playing: %s\r\n",Audio.PlayList[current_MusicNum]);
//  Get_WAV_Message(Audio.PlayList[current_MusicNum],&WaveCtrlData);    //Get the messages of the WAV file
//
//  WAV_OFFSET = WaveCtrlData.datastart;
//
//  /*Start Play Music*/
//  f_lseek(&WAV_File, WAV_OFFSET);
//  Fill_WAV_Buffer(WAV_Buffer, WAV_BUFFER_SIZE);
//  WAV_OFFSET += WAV_BUFFER_SIZE;
//  while(Audio.EndSong == false)  {
//    while(Audio.PlaySong == true) {
//      if(I2S_Flag == I2S_Half_Callback) {
//        f_lseek(&WAV_File, WAV_OFFSET);
//        Fill_WAV_Buffer(WAV_Buffer,WAV_BUFFER_SIZE/2);
//        WAV_OFFSET += WAV_BUFFER_SIZE/2;
//
//        I2S_Flag = I2S_No_CallBack;
//      }
//      else if(I2S_Flag == I2S_Callback) {
//        f_lseek(&WAV_File, WAV_OFFSET);
//        Fill_WAV_Buffer((WAV_Buffer+WAV_BUFFER_SIZE/2),WAV_BUFFER_SIZE/2);
//        WAV_OFFSET += WAV_BUFFER_SIZE/2;
//        WAV_LastData -= WAV_BUFFER_SIZE;
//
//        I2S_Flag = I2S_No_CallBack;
//      }
//      else  {
//        //Key_Control();
//      }
//    }
//    Audio.PlaySong = true;
//#ifdef AUDIO_USE_I2S
//    HAL_I2S_Transmit_DMA(&hi2s5,WAV_Buffer_16, WAV_BUFFER_SIZE/2);
//#elif AUDIO_USE_SAI
//    HAL_SAI_Transmit_DMA(&hsai_BlockB1,WAV_Buffer, WAV_BUFFER_SIZE/2);
//#endif
//  }
//  printf("End Play: %s\r\n",Audio.PlayList[current_MusicNum]);
//  res = f_close(&WAV_File);
//
//  return res;
//}
/**
  * @brief  Open the WAV file, get the message of the file.
  * @param  fname: name of the file you want to get its massage.
  * @param  wavx: the struct of data control.
  * @retval None
  */
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{

  //printf("I2S_Half_Callback !!\r\n");
  I2S_Flag = I2S_Half_Callback;

}


void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{

  //printf("I2S_Callback !!\r\n");
  I2S_Flag = I2S_Callback;

  if(WAV_LastData >= WAV_BUFFER_SIZE)
  {
    HAL_I2S_Transmit_DMA(&hi2s5,WAV_Buffer_16, WAV_BUFFER_SIZE/2);
  }
  else
  {
    Audio.PlaySong = false;
    Audio.EndSong = true;
  }
}
/**
  * @brief  Check whether keys is pressed.
  * @param  None
  * @retval None
  */
//void Key_Control(void)  {
//
//  //Start_Check
//  if(Key_Start_Check() == 1)  {
//    if(Pause_Flag == 0) {
//      HAL_SAI_DMAPause(&hsai_BlockB1);
//      printf("Pause!!\r\n");
//      Pause_Flag = 1;
//    }
//    else  {
//      HAL_SAI_DMAResume(&hsai_BlockB1);
//      printf("Resume!!\r\n");
//      Pause_Flag = 0;
//    }
//  }//Start_Check End
//  else if(Key_Next_Check() == 1)  {
//    printf("Next!\r\n");
//
//    Audio.PlaySong = 0;
//    Audio.EndSong = 1;
//
//    if(Pause_Flag == 1) {
//      Pause_Flag = 0;
//    }
//    HAL_SAI_DMAStop(&hsai_BlockB1);
//  }
//  else if(Key_Last_Check() == 1)  {
//    printf("Last!\r\n");
//
//    Audio.CurrentSong -= 2;
//    if(Audio.CurrentSong < 0)
//      Audio.CurrentSong += Audio.MaxSong;
//
//    Audio.PlaySong = 0;
//    Audio.EndSong = 1;
//
//    if(Pause_Flag == 1) {
//      HAL_SAI_DMAResume(&hsai_BlockB1);
//      Pause_Flag = 0;
//    }
//  }
//}



