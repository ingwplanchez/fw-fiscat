#include "includes.h"

//#ifdef INCLUDE_CUSTOMER_LCD
#include "dotlcd.h"
#include "dotlcd_c.h"
//#include "lcd.h"

static CUSTLCDDISPLAY custLcdDisplay[CUSTLCDLINE],custLcdDisplay_Bak[CUSTLCDLINE];
uint8_t custDotLcdMap2[CUSTOMER_LCD_DOT_ROW][(CUSTOMER_LCD_DOT_COL+7)/8];

static CURSORPOS custCursorPos;
#define CUST_CURSORTIME  25           //光标闪动 300ms
static uint8_t direct_update = 0;
static uint8_t logo_update = 0;
void custlcd_set_logoupdate(void)
{
    logo_update = 1;
}
void custlcd_set_direct_update(void)
{
    direct_update = 1;
}

CUSTLCDDISPLAY *getCustLcdDisplay(void)
{
    return custLcdDisplay;
}
/*
uint8_t *getCustLcdDotMap(void)
{
    return custDotLcdMap2;
}
*/

//函  数:反白一行LCD字符点阵
//参  数:无
//返  回:无
static void cust_inversePattern(uint16_t line)
{
    uint16_t i;
    uint16_t width;
    if ((custLcdDisplay[line].propStart >= custLcdDisplay[line].propEnd) || (custLcdDisplay[line].propEnd > CUSTLCDBUFLEN))
    {
        return;
    }
//    printf("custLcdDisplay[line].propStart=%d custLcdDisplay[line].propEnd=%d line=%d\n\r",custLcdDisplay[line].propStart,custLcdDisplay[line].propEnd,line);
    width = (custLcdDisplay[line].propEnd - custLcdDisplay[line].propStart) * CUSTOMER_LCD_FONT_WIDTH / 8;
    for(i=0 ; i<CUSTOMER_LCD_FONT_HIGH-1 ; i++)
    {
        memXL(&custDotLcdMap2[line*(CUSTOMER_LCD_FONT_HIGH+CUSTOMER_LCD_FONT_LINESPACE)+i][custLcdDisplay[line].propStart*CUSTOMER_LCD_FONT_WIDTH/8],width);
    }
}
//函  数:设置光标
//参  数:flag 标志
//返  回:无
static void cust_setCursorMap(uint8_t flag)
{
    uint16_t line,pos;
    line = custCursorPos.line*(CUSTOMER_LCD_FONT_HIGH+CUSTOMER_LCD_FONT_LINESPACE)-CUSTOMER_LCD_FONT_LINESPACE + CUSTOMER_LCD_FONT_HIGH-1;
    pos = custCursorPos.pos*CUSTOMER_LCD_FONT_WIDTH/8;
    if (flag)
    {
        custDotLcdMap2[line][pos] = 0xff;
//        custDotLcdMap2[line][pos+1] = 0xff;
//        memset(&custDotLcdMap2[custCursorPos.line*(CUSTOMER_LCD_FONT_HIGH+CUSTOMER_LCD_FONT_LINESPACE)-CUSTOMER_LCD_FONT_LINESPACE][custCursorPos.pos*CUSTOMER_LCD_FONT_WIDTH],0xff,CUSTOMER_LCD_FONT_WIDTH);
//        memset(&custDotLcdMap2[custCursorPos.line*(CUSTOMER_LCD_FONT_HIGH+CUSTOMER_LCD_FONT_LINESPACE)-CUSTOMER_LCD_FONT_LINESPACE+1][custCursorPos.pos*CUSTOMER_LCD_FONT_WIDTH],0xff,CUSTOMER_LCD_FONT_WIDTH);
    }
    else
    {
        custDotLcdMap2[line][pos] = 0x0;
    }

}

//函  数:更新DOTLCD显示
//参  数:无
//返  回:无
extern void updateCustDotLcd()
{
    uint8_t immupdate = 0;
    uint16_t line;
    uint8_t updateflag;
    updateflag = FALSE;
    set_lcd_type(1);
    if(direct_update)
    {
        #if defined(INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832)
        SPEC12832DisplayPattern(&custDotLcdMap2[0][0],0,0);
        direct_update = 0;
        return ;
        #endif
    }
    if(logo_update)
    {
        for(line = 0 ; line < CUSTLCDLINE ; line++)
        {
            if (memcmp((uint8_t *)&custLcdDisplay[line],(uint8_t *)&custLcdDisplay_Bak[line],sizeof(CUSTLCDDISPLAY)) != 0 )
            {
                immupdate = 1;
                logo_update = 0;
                break;
            }
        }
    }
    for(line = 0 ; line < CUSTLCDLINE ; line++)
    {
        if (memcmp((uint8_t *)&custLcdDisplay[line],(uint8_t *)&custLcdDisplay_Bak[line],sizeof(CUSTLCDDISPLAY)) != 0 || immupdate)
        {
            fillFontPartent(&custDotLcdMap2[line*(CUSTOMER_LCD_FONT_HIGH+CUSTOMER_LCD_FONT_LINESPACE)][0],sizeof(custDotLcdMap2[0]),&custLcdDisplay[line].buffer[0],CUSTLCDBUFLEN,1,0);
            if (custLcdDisplay[line].propStart < custLcdDisplay[line].propEnd)
            {
                //有反白
                cust_inversePattern(line);
            }
            memcpy((uint8_t *)&custLcdDisplay_Bak[line],(uint8_t *)&custLcdDisplay[line],sizeof(CUSTLCDDISPLAY));
            updateflag = TRUE;
        }
    }
    if (custCursorPos.line < CUSTLCDLINE)
    {   //有光标
        if ((++custCursorPos.timeCnt % CUST_CURSORTIME) == 0)
        {
            if ((custCursorPos.timeCnt / CUST_CURSORTIME) & 0x01)
            {
                //光标亮
                cust_setCursorMap(TRUE);
            }
            else
            {
                //光标黑
                cust_setCursorMap(FALSE);
            }
            updateflag = TRUE;
        }
    }
    else
    {
        custCursorPos.timeCnt = 0;
    }
    if (updateflag)
    {
#ifdef INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832
        SPEC12832DisplayPattern(&custDotLcdMap2[0][0],0,0);
#endif
    }
}
//函  数:初始化点阵LCD
//参  数:无
//返  回:无
extern void initCustDotLcd()
{
    memset((uint8_t *)&custLcdDisplay_Bak,0,sizeof(custLcdDisplay_Bak));
    clearCustDotLcd();
#ifdef INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832
    SPEC12832Init();
#endif
}
extern void simpaleInitCustDotLcd(void)
{
//    memset((uint8_t *)&custLcdDisplay_Bak,0,sizeof(custLcdDisplay_Bak));
//    clearCustDotLcd();
#ifdef INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832
    SPEC12832Init();
    SPEC12832DisplayPattern(&custDotLcdMap2[0][0],0,0);
#endif
}
//函  数:清空DOT LCD显示屏
//参  数:无
//返  回:无
extern void clearCustDotLcd()
{
    int line;
    custCursorPos.line = 0xff;
    for(line=0 ; line<CUSTLCDLINE ;line++)
    {
        clearCustDotLcdLine(line);
    }
}
extern void clearCustDotLcdLine_bak(uint16_t line)
{
    if (line < CUSTLCDLINE)
    {
        custLcdDisplay_Bak[line].propStart = 0;
        custLcdDisplay_Bak[line].propEnd = 0;
        memset(custLcdDisplay_Bak[line].buffer,' ',CUSTLCDBUFLEN);
    }
}
extern void clearCustDotLcd_bak()
{
    int line;
    for(line=0 ; line<CUSTLCDLINE ;line++)
    {
        clearCustDotLcdLine_bak(line);
    }
}
//函  数:清空DOT LCD显示行
//参  数:无
//返  回:无
extern void clearCustDotLcdLine(uint16_t line)
{
    if (line < CUSTLCDLINE)
    {
        custLcdDisplay[line].propStart = 0;
        custLcdDisplay[line].propEnd = 0;
        memset(custLcdDisplay[line].buffer,' ',CUSTLCDBUFLEN);
    }
}
//函  数:送一个字符到DOT LCD
//参  数:line   行号
//       charidx 字符序号
//       ch      字符
//返  回:无
extern void putCustDotLcdChar(uint16_t line,uint16_t charidx,uint8_t ch)
{
    uint16_t pos;
    pos = line*CUSTLCDBUFLEN + charidx;
    if (pos < (CUSTLCDLINE * CUSTLCDBUFLEN))
    {
        line = pos/CUSTLCDBUFLEN;
        charidx = pos % CUSTLCDBUFLEN;
        custLcdDisplay[line].buffer[charidx] = ch;
    }
}
//函  数:设置LCD反白的位置
//参  数:line   行号
//       startIdx 开始字符序号
//       endIdx   结束字符序号
//返  回:无
extern void setCustDotLcdReserver(uint16_t line,uint16_t startIdx,uint16_t endIdx)
{
    if (line < CUSTLCDLINE)
    {
        custLcdDisplay[line].propStart = startIdx;
        custLcdDisplay[line].propEnd = endIdx;
    }
}
extern void scollUpOneCustLcdLine(void)
{
    uint16_t i;
    for(i=0 ; i<(CUSTLCDLINE-1) ;i++)
    {
        custLcdDisplay[i] = custLcdDisplay[i+1];
    }
}

//函  数:设置光标位置
//参  数:line   行号
//       pos    位置
//返  回:无
extern void setCustDotLcdCursor(uint16_t line,uint16_t pos)
{
    custCursorPos.line = line;
    custCursorPos.pos = pos;
}
extern void showCustDotMap(uint8_t *dotmap)
{
#ifdef INCLUDE_CUSTOMER_LCD_TYPE_SPEC12832
        SPEC12832DisplayPattern(dotmap,0,0);
#endif
}

extern void openCustDotLcdBackLight(uint8_t para)
{
    if(para)
    {
        LPC_GPIO2->DATA |= (1<<0);
    }
    else
    {
        LPC_GPIO2->DATA &= ~(1<<0);
    }
}

//#endif



