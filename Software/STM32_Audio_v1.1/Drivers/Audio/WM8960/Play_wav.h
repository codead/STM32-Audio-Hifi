/*
 * Play_wav.h
 *
 *  Created on: Jul 20, 2022
 *      Author: Duc Huy Le
 */

#ifndef AUDIO_WM8960_PLAY_WAV_H_
#define AUDIO_WM8960_PLAY_WAV_H_
#include "stm32f4xx_hal.h"
#include "fatfs.h"
#include "stdbool.h"
#include "RingBuffer_LIFO.h"
#define WAV_BUFFER_SIZE 16 * 2500//2200
#define PLAY_SIZE 15000

//RIFF Chunk
typedef struct
{
  uint32_t ChunkID;       //CHUNK ID is "RIFF" = 0X46464952
  uint32_t ChunkSize ;    //ChunkSize = file_size - 8
  uint32_t Format;        //Format: "WAVE" = 0X45564157
}__attribute__((__packed__)) ChunkRIFF;

//fmt Chunk
typedef struct
{
  uint32_t ChunkID;       //Chunk ID is "fmt" = 0X20746D66
  uint32_t ChunkSize ;    //ChunkSize(Not including ID and Size): 20.
  uint16_t AudioFormat;   //Audio Format = 0X01,Linear PCM;0X11 is IMA ADPCM
  uint16_t NumOfChannels; //channel number; 1,mono channel; 2,double channel;
  uint32_t SampleRate;    //SampleRate;0X1F40,8Khz
  uint32_t ByteRate;      //ByteRate;
  uint16_t BlockAlign;    //Chunk align (byte);
  uint16_t BitsPerSample; //Single sample data size; 4 bits is ADPCM,set as 4
  uint16_t ByteExtraData; //extra data ; 2 ;linear PCM file don't have this.
}__attribute__((__packed__))ChunkFMT;

//fact Chunk
typedef struct
{
  uint32_t ChunkID;       //Chunk ID: "fact",0X74636166;Linear PCM don't have this.
  uint32_t ChunkSize ;    //Chunk Size(Not includeing ID and Size);This is 4.
  uint32_t FactSize;      //Transform to PCM File's size;
}__attribute__((__packed__))ChunkFACT;

//WAV File play control struct
typedef struct
{
  uint16_t audioformat;   //audio format;0X01 is Linear PCM;0X11 is IMA ADPCM.
  uint16_t nchannels;     //track number; 1,single track; 2,dual track;
  uint16_t blockalign;    //Chunk align (byte);
  uint32_t datasize;      //WAV data size;
  uint32_t totsec ;       //Play time (s);
  uint32_t cursec ;       //Current playback time (s)
  uint32_t bitrate;       //Bit rate (bit speed)
  uint32_t samplerate;    //sampling rate
  uint16_t bps;           //bit numbers,such as 16bit,24bit,32bit
  uint32_t datastart;     //Starting position of Data frame(Offset in the file)
}__attribute__((__packed__)) wavctrl;

//Data Chunk
typedef struct
{
  uint32_t ChunkID;       //chunk ID: "data" = 0X5453494C
  uint32_t ChunkSize ;    //Chunk Size(Not includeing ID and Size)
}__attribute__((__packed__))ChunkDATA;

//I2S CallBack function Flag
typedef enum
{
  I2S_No_CallBack = 0,
  I2S_Half_Callback = 1,
  I2S_Callback = 2,
  I2S_Waiting_Half_Callback = 3,
  I2S_Waiting_Callback = 4,
}I2S_CallBack_Flag;
typedef enum
{
  AUTO = 0,
  NEXT_SONG,
  BACK_TO_PREVIOUS_SONG,
  SELECT_SONG,
}NEW_SONG_EVENT;
typedef struct
{
  bool PlaySong;
  bool EndSong;
  bool PauseSong;
  NEW_SONG_EVENT NewSongEvent;
  char PlayList[255][50];
  uint16_t MaxSong;
  uint16_t CurrentSong;
  uint16_t PreviousSong;
}AUDIO;
extern uint32_t WAV_OFFSET;
extern FIL WAV_File;
extern uint8_t WAV_Buffer[WAV_BUFFER_SIZE];
extern uint32_t WAV_LastData;
//extern uint8_t Play_Flag;
//extern uint8_t End_Flag;
extern volatile I2S_CallBack_Flag I2S_Flag;
//extern char Audio.PlayList[255][50];
//extern uint16_t Audio.MaxSong;
//extern uint16_t Audio.CurrentSong;

extern AUDIO Audio;
extern uint16_t *WAV_Buffer_16;
void Audio_CreatePreviousSongList(void);
LIFO_STATUS Audio_AddPreviousSong(void);
LIFO_STATUS Audio_GetPreviousSong(void);
FRESULT ScanWavefiles(char* path);
uint8_t PlayWaveFile(void);
uint8_t Get_WAV_Message(char* fname,wavctrl* wavx);
FRESULT Fill_WAV_Buffer(uint8_t *BUFF, uint16_t size);
void Volum_Control(uint8_t ctrl);
void Key_Control(void);



#endif /* AUDIO_WM8960_PLAY_WAV_H_ */
