#ifndef __INC_FONT_H__
#define __INC_FONT_H__

#include "includes.h"
#define     FONTTYPE_8X16        1
//#define     FONTTYPE_10X24       2
//#define     FONTTYPE_12X24       3
//#define     FONTTYPE_11X24       4
//8X16字库
#define     FONT8X16_WIDTHPOINT     8       //宽点
#define     FONT8X16_HIGHPOINT      16      //高点
#define     FONT8X16_SIZE_BYTE      16      //字库大小
//10X24字库
#define     FONT10X24_WIDTHPOINT     10       //宽点
#define     FONT10X24_HIGHPOINT      24      //高点
#define     FONT10X24_SIZE_BYTE      48      //字库大小
//11X24字库
#define     FONT11X24_WIDTHPOINT     11       //宽点
#define     FONT11X24_HIGHPOINT      24      //高点
#define     FONT11X24_SIZE_BYTE      48      //字库大小
//12X24字库
#define     FONT12X24_WIDTHPOINT     12       //宽点
#define     FONT12X24_HIGHPOINT      24      //高点
#define     FONT12X24_SIZE_BYTE      48      //字库大小



extern void fillFontPartent(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fonttype,uint16_t fontflag);
extern void readLogoData(uint8_t *buf,uint16_t logoidx,uint32_t addr,uint16_t len);

#endif

