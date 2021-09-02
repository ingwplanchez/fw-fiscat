#ifndef __INC_DOTLCD_H__
#define __INC_DOTLCD_H__

#include "includes.h"


//#define INCLUDE_MULTILINE_LCD
#define INCLUDE_DOTLCD_TYPE_SPEC12832


//#ifdef INCLUDE_MULTILINE_LCD

#if ((CONDITION == 10) || (CONDITION == 20))
//#define INCLUDE_DOTLCD_TYPE_CDT12864                    //DOT LCD 类型
#define DOTLCD_FONT         FONTTYPE_8X16   //使用的字库
#elif (CONDITION == 30) || (CONDITION == 40)
//#define INCLUDE_DOTLCD_TYPE_CDT12864                    //DOT LCD 类型
#define DOTLCD_FONT         FONTTYPE_8X16//使用的字库
#elif (CONDITION == 50) || (CONDITION == 60)
#if  defined(MAINLCD_12864)
//#define INCLUDE_DOTLCD_TYPE_CDT12864                    //DOT LCD 类型
#else
#define INCLUDE_DOTLCD_TYPE_SPEC12832                    //DOT LCD 类型
#endif
#define DOTLCD_FONT         FONTTYPE_8X16//使用的字库
#endif

#ifdef INCLUDE_DOTLCD_TYPE_UG2864A
#include "ug2864A.h"
#define DOTLCD_DOT_ROW      UG2864RowNum
#define DOTLCD_DOT_COL      UG2864ColumnNum
#elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
#include "cdt12864.h"
#define DOTLCD_DOT_ROW      CDT12864RowNum
#define DOTLCD_DOT_COL      CDT12864ColumnNum
#elif defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
#include "spec12832.h"
#define DOTLCD_DOT_ROW      SPEC12832RowNum
#define DOTLCD_DOT_COL      SPEC12832ColumnNum

#endif

#if (DOTLCD_FONT == FONTTYPE_8X16)
#define MLCD_FONT_HIGH      FONT8X16_HIGHPOINT  //字库高度
#define MLCD_FONT_WIDTH     FONT8X16_WIDTHPOINT //字库宽度
#define MLCD_FONT_BYTESIZE  FONT8X16_SIZE_BYTE  //字库大小
#define MLCD_FONT_LINESPACE 0                   //行间距
#elif (DOTLCD_FONT == FONTTYPE_10X24)
#define MLCD_FONT_HIGH      FONT10X24_HIGHPOINT  //字库高度
#define MLCD_FONT_WIDTH     FONT10X24_WIDTHPOINT //字库宽度
#define MLCD_FONT_BYTESIZE  FONT10X24_SIZE_BYTE  //字库大小
#define MLCD_FONT_LINESPACE 0                   //行间距
#elif (DOTLCD_FONT == FONTTYPE_12X24)
#define MLCD_FONT_HIGH      FONT12X24_HIGHPOINT  //字库高度
#define MLCD_FONT_WIDTH     FONT12X24_WIDTHPOINT //字库宽度
#define MLCD_FONT_BYTESIZE  FONT12X24_SIZE_BYTE  //字库大小
#define MLCD_FONT_LINESPACE 0                   //行间距
#endif

#define MLCDLINE    (DOTLCD_DOT_ROW/MLCD_FONT_HIGH)
#define MLCDBUFLEN  (DOTLCD_DOT_COL/MLCD_FONT_WIDTH)

typedef struct
{
    uint16_t    propStart;
    uint16_t    propEnd;
    uint8_t   buffer[MLCDBUFLEN];
}DOTLCDDISPLAY;

typedef struct
{
    uint8_t   line;
    uint8_t   pos;
    uint16_t   timeCnt;
}CURSORPOS;

extern DOTLCDDISPLAY *getMainLcdDisplay(void);
//extern uint8_t *getMainLcdDotMap(void);
extern void openDotLcdBackLight(uint8_t para);
extern void updateDotLcd(void);
extern void clearDotLcd(void);
extern void clearDotLcd_bak(void);
extern void clearDotLcdLine(uint16_t line);
extern void setDotLcdBackLight(uint8_t level);
extern void initDotLcd(void);
extern void putDotLcdChar(uint16_t line,uint16_t charidx,uint8_t ch);
extern void setDotLcdReserver(uint16_t line,uint16_t startIdx,uint16_t endIdx);
extern void setDotLcdCursor(uint16_t line,uint16_t pos);
extern void showDotMap(uint8_t *dotmap);
extern void getDotLcdData(DOTLCDDISPLAY *lcd);
extern void setDotLcdData(DOTLCDDISPLAY *lcd);
extern void scollUpOneLcdLine(void);
extern void dotlcd_set_logoupdate(void);
extern void dotlcd_set_direct_update(void);
extern void simpaleInitDotLcd(void);
#endif
//#endif

