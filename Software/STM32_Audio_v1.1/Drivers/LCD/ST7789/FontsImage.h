/*
 * FontsImage.h
 *
 *  Created on: Jan 30, 2023
 *      Author: Duc Huy Le
 */

#ifndef LCD_ST7789_FONTSIMAGE_H_
#define LCD_ST7789_FONTSIMAGE_H_

#include "stdint.h"

typedef struct {
   const uint16_t *data;
   uint16_t width;
   uint16_t height;
   uint8_t dataSize;
   } tImage;
typedef struct {
   long int code;
   const tImage *image;
   } tChar;
typedef struct {
   int length;
   const tChar *chars;
   } tFont;

extern const tFont Font;
//void ST7789_WriteStringImage(uint16_t x, uint16_t y, const char *str, tFont font);
#endif /* LCD_ST7789_FONTSIMAGE_H_ */
