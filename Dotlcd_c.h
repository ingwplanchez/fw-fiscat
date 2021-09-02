#ifndef __INC_DOTLCD_C_H__
#define __INC_DOTLCD_C_H__

#include "includes.h"
#include "fillfont.h"
#define INCLUDE_CUSTOMER_LCD

#define INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832

//#ifdef INCLUDE_CUSTOMER_LCD

#if (CONDITION == 50)
#define INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832                    //DOT LCD 类型
#define CUSTOMER_LCD_FONT         FONTTYPE_8X16   //使用的字库
#endif

//#ifdef INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832
//#include "spec12832.h"
#define CUSTOMER_LCD_DOT_ROW      SPEC12832RowNum
#define CUSTOMER_LCD_DOT_COL      SPEC12832ColumnNum
//#endif

//#if (CUSTOMER_LCD_FONT == FONTTYPE_8X16)
#define CUSTOMER_LCD_FONT_HIGH      FONT8X16_HIGHPOINT  //字库高度
#define CUSTOMER_LCD_FONT_WIDTH     FONT8X16_WIDTHPOINT //字库宽度
#define CUSTOMER_LCD_FONT_BYTESIZE  FONT8X16_SIZE_BYTE  //字库大小
#define CUSTOMER_LCD_FONT_LINESPACE 0                   //行间距
//#elif (CUSTOMER_LCD_FONT == FONTTYPE_10X24)
//#define CUSTOMER_LCD_FONT_HIGH      FONT10X24_HIGHPOINT  //字库高度
//#define CUSTOMER_LCD_FONT_WIDTH     FONT10X24_WIDTHPOINT //字库宽度
//#define CUSTOMER_LCD_FONT_BYTESIZE  FONT10X24_SIZE_BYTE  //字库大小
//#define CUSTOMER_LCD_FONT_LINESPACE 0                   //行间距
//#elif (CUSTOMER_LCD_FONT == FONTTYPE_12X24)
//#define CUSTOMER_LCD_FONT_HIGH      FONT12X24_HIGHPOINT  //字库高度
//#define CUSTOMER_LCD_FONT_WIDTH     FONT12X24_WIDTHPOINT //字库宽度
//#define CUSTOMER_LCD_FONT_BYTESIZE  FONT12X24_SIZE_BYTE  //字库大小
//#define CUSTOMER_LCD_FONT_LINESPACE 0                   //行间距
//#endif

#define CUSTLCDLINE    (CUSTOMER_LCD_DOT_ROW/CUSTOMER_LCD_FONT_HIGH)
#define CUSTLCDBUFLEN  (CUSTOMER_LCD_DOT_COL/CUSTOMER_LCD_FONT_WIDTH)

typedef struct
{
    uint16_t    propStart;
    uint16_t    propEnd;
    uint8_t   buffer[CUSTLCDBUFLEN];
}CUSTLCDDISPLAY;

extern CUSTLCDDISPLAY *getCustLcdDisplay(void);
//extern uint8_t *getCustLcdDotMap(void);
extern void updateCustDotLcd(void);
extern void initCustDotLcd(void);
extern void clearCustDotLcd(void);
extern void clearCustDotLcd_bak(void);
extern void clearCustDotLcdLine(uint16_t line);
extern void putCustDotLcdChar(uint16_t line,uint16_t charidx,uint8_t ch);
extern void setCustDotLcdReserver(uint16_t line,uint16_t startIdx,uint16_t endIdx);
extern void setCustDotLcdCursor(uint16_t line,uint16_t pos);
extern void showCustDotMap(uint8_t *dotmap);
extern void scollUpOneCustLcdLine(void);
extern void openCustDotLcdBackLight(uint8_t para);
extern void custlcd_set_direct_update(void);
extern void custlcd_set_logoupdate(void);
extern void simpaleInitCustDotLcd(void);

#endif


//#endif

