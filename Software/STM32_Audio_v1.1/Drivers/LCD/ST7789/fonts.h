/*
 * fonts.h
 *
 *  Created on: Jul 13, 2022
 *      Author: Duc Huy Le
 */

#ifndef LCD_ST7789_FONTS_H_
#define LCD_ST7789_FONTS_H_
#ifndef __FONT_H
#define __FONT_H

#include "stdint.h"

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

//Font lib.
extern FontDef Font_7x10;
extern FontDef Font_11x18;
extern FontDef Font_16x26;
extern FontDef Font_16x28;
//16-bit(RGB565) Image lib.
/*******************************************
 *             CAUTION:
 *   If the MCU onchip flash cannot
 *  store such huge image data,please
 *           do not use it.
 * These pics are for test purpose only.
 *******************************************/

/* 128x128 pixel RGB565 image */
extern const uint16_t saber[][128];
//extern const uint16_t Bat1[];
//
//extern const uint16_t Battery_80[];
//extern const uint16_t Battery_70[];
//extern const uint16_t Battery_60[];

/* 240x240 pixel RGB565 image
extern const uint16_t knky[][240];
extern const uint16_t tek[][240];
extern const uint16_t adi1[][240];
*/
#endif


#endif /* LCD_ST7789_FONTS_H_ */
