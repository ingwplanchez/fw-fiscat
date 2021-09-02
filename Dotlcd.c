#include "includes.h"

#include "dotlcd.h"
#include "dotlcd_c.h"
#include "lcd.h"

#ifdef INCLUDE_MULTILINE_LCD

DOTLCDDISPLAY lcdDisplay[MLCDLINE],lcdDisplay_Bak[MLCDLINE];
uint8_t DotLcdMap[DOTLCD_DOT_ROW][(DOTLCD_DOT_COL+7)/8];
static CURSORPOS cursorPos;
#define CURSORTIME        25     //光标闪动 300ms
static uint8_t direct_update = 0;
static uint8_t logo_update = 0;
void dotlcd_set_logoupdate(void)
{
    logo_update = 1;
}
void dotlcd_set_direct_update(void)
{
    direct_update = 1;
}

DOTLCDDISPLAY *getMainLcdDisplay(void)
{
    return lcdDisplay;
}

//函  数:反白一行LCD字符点阵
//参  数:无
//返  回:无
static void inversePattern(uint16_t line)
{
    uint16_t i;
    uint16_t width;
    if ((lcdDisplay[line].propStart >= lcdDisplay[line].propEnd) || (lcdDisplay[line].propEnd > MLCDBUFLEN))
    {
        return;
    }
//    printf("lcdDisplay[line].propStart=%d lcdDisplay[line].propEnd=%d line=%d\n\r",lcdDisplay[line].propStart,lcdDisplay[line].propEnd,line);
    width = (lcdDisplay[line].propEnd - lcdDisplay[line].propStart) * MLCD_FONT_WIDTH / 8;
    for(i=0 ; i<MLCD_FONT_HIGH-1 ; i++)
    {
        memXL(&DotLcdMap[line*(MLCD_FONT_HIGH+MLCD_FONT_LINESPACE)+i][lcdDisplay[line].propStart*MLCD_FONT_WIDTH/8],width);
    }
}
//函  数:设置光标
//参  数:flag 标志
//返  回:无
static void setCursorMap(BOOL flag)
{
    uint16_t line,pos;
    line = cursorPos.line*(MLCD_FONT_HIGH+MLCD_FONT_LINESPACE)-MLCD_FONT_LINESPACE + MLCD_FONT_HIGH-1;
    pos = cursorPos.pos*MLCD_FONT_WIDTH/8;
    if (flag)
    {
        DotLcdMap[line][pos] = 0xff;
    }
    else
    {
        DotLcdMap[line][pos] = 0x0;
    }

}

//函  数:更新DOTLCD显示
//参  数:无
//返  回:无
extern void updateDotLcd()
{
    uint16_t line;
    BOOLEAN updateflag;
    uint8_t immupdate = 0;
    updateflag = FALSE;
    set_lcd_type(MAINLCD);
    if(direct_update)
    {
        #if defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
        SPEC12832DisplayPattern(&DotLcdMap[0][0]);
        #elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
        CDT12864DisplayPattern(&DotLcdMap[0][0]);
        #endif
        direct_update = 0;
        return ;
    }

    if(logo_update)
    {
        for(line = 0 ; line < MLCDLINE ; line++)
        {
            if (memcmp((uint8_t *)&lcdDisplay[line],(uint8_t *)&lcdDisplay_Bak[line],sizeof(DOTLCDDISPLAY)) != 0)
            {
                immupdate = 1;
                logo_update = 0;
                break;
            }
        }
    }

    for(line = 0 ; line < MLCDLINE ; line++)
    {
        if (memcmp((uint8_t *)&lcdDisplay[line],(uint8_t *)&lcdDisplay_Bak[line],sizeof(DOTLCDDISPLAY)) != 0 || immupdate)
        {
            fillFontPartent(&DotLcdMap[line*(MLCD_FONT_HIGH+MLCD_FONT_LINESPACE)][0],sizeof(DotLcdMap[0]),&lcdDisplay[line].buffer[0],MLCDBUFLEN,DOTLCD_FONT,0);
            if (lcdDisplay[line].propStart < lcdDisplay[line].propEnd)
            {
                //有反白
                inversePattern(line);
            }
            memcpy((uint8_t *)&lcdDisplay_Bak[line],(uint8_t *)&lcdDisplay[line],sizeof(DOTLCDDISPLAY));
            updateflag = TRUE;
        }
    }
    if (cursorPos.line < MLCDLINE)
    {   //有光标
        if ((++cursorPos.timeCnt % CURSORTIME) == 0)
        {
            if ((cursorPos.timeCnt / CURSORTIME) & 0x01)
            {
                //光标亮
                setCursorMap(TRUE);
            }
            else
            {
                //光标黑
                setCursorMap(FALSE);
            }
            updateflag = TRUE;
        }
    }
    else
    {
        cursorPos.timeCnt = 0;
    }
    if (updateflag)
    {

#if defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
        SPEC12832DisplayPattern(&DotLcdMap[0][0]);
#elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
        CDT12864DisplayPattern(&DotLcdMap[0][0]);
#endif
    }
}
//函  数:初始化点阵LCD
//参  数:无
//返  回:无
extern void initDotLcd()
{
    memset((uint8_t *)&lcdDisplay_Bak,0,sizeof(lcdDisplay_Bak));
    clearDotLcd();
#ifdef INCLUDE_DOTLCD_TYPE_UG2864A
    UG2864AInit();
#elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
    CDT12864Init();
#elif defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
    SPEC12832Init(MAINLCD);
#endif
    // 初始化背光
    LPC_GPIO2->DIR |= 1<<0;
    LPC_IOCON->PIO2_0 = IOCON_PIO;
    openDotLcdBackLight(1);
}

extern void simpaleInitDotLcd(void)
{
//    memset((uint8_t *)&lcdDisplay_Bak,0,sizeof(lcdDisplay_Bak));
//    clearDotLcd();
#ifdef INCLUDE_DOTLCD_TYPE_UG2864A
    UG2864AInit();
#elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
    CDT12864Init();
#elif defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
    SPEC12832Init(MAINLCD);
    SPEC12832DisplayPattern(&DotLcdMap[0][0]);
#endif
}
//函  数:打开LCD背光
//参  数:para 0 关   1 开
//返  回:无
extern void openDotLcdBackLight(uint8_t para)
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
//函  数:清空DOT LCD显示屏
//参  数:无
//返  回:无
extern void clearDotLcd()
{
    int line;
    cursorPos.line = 0xff;
    for(line=0 ; line<MLCDLINE ;line++)
    {
        clearDotLcdLine(line);
    }
}
extern void clearDotLcdLine_bak(uint16_t line)
{
    if (line < MLCDLINE)
    {
        lcdDisplay_Bak[line].propStart = 0;
        lcdDisplay_Bak[line].propEnd = 0;
        memset(lcdDisplay_Bak[line].buffer,' ',MLCDBUFLEN);
    }
}
extern void clearDotLcd_bak()
{
    int line;
    for(line=0 ; line<MLCDLINE ;line++)
    {
        clearDotLcdLine_bak(line);
    }
}

//函  数:清空DOT LCD显示行
//参  数:无
//返  回:无
extern void clearDotLcdLine(uint16_t line)
{
    if (line < MLCDLINE)
    {
        lcdDisplay[line].propStart = 0;
        lcdDisplay[line].propEnd = 0;
        memset(lcdDisplay[line].buffer,' ',MLCDBUFLEN);
    }
}

//函  数:设置LCD背光
//参  数:level 开 / 关
//返  回:无
extern void setDotLcdBackLight(uint8_t level)
{
    level = level;
}
//函  数:送一个字符到DOT LCD
//参  数:line   行号
//       charidx 字符序号
//       ch      字符
//返  回:无
extern void putDotLcdChar(uint16_t line,uint16_t charidx,uint8_t ch)
{
    uint16_t pos;
    pos = line*MLCDBUFLEN + charidx;
    if (pos < (MLCDLINE * MLCDBUFLEN))
    {
        line = pos/MLCDBUFLEN;
        charidx = pos % MLCDBUFLEN;
        lcdDisplay[line].buffer[charidx] = ch;
    }
}
//函  数:设置LCD反白的位置
//参  数:line   行号
//       startIdx 开始字符序号
//       endIdx   结束字符序号
//返  回:无
extern void setDotLcdReserver(uint16_t line,uint16_t startIdx,uint16_t endIdx)
{
    if (line < MLCDLINE)
    {
        lcdDisplay[line].propStart = startIdx;
        lcdDisplay[line].propEnd = endIdx;
    }
}
extern void scollUpOneLcdLine(void)
{
    uint16_t i;
    for(i=0 ; i<(MLCDLINE-1) ;i++)
    {
        lcdDisplay[i] = lcdDisplay[i+1];
    }
}
//函  数:设置光标位置
//参  数:line   行号
//       pos    位置
//返  回:无
extern void setDotLcdCursor(uint16_t line,uint16_t pos)
{
    memset((uint8_t *)&lcdDisplay_Bak,0,sizeof(lcdDisplay_Bak));
    cursorPos.line = line;
    cursorPos.pos = pos;
}
extern void showDotMap(uint8_t *dotmap)
{
#ifdef INCLUDE_DOTLCD_TYPE_UG2864A
        UG2864ADisplayPattern(dotmap);
#elif defined(INCLUDE_DOTLCD_TYPE_CDT12864)
        CDT12864DisplayPattern(dotmap);
#elif defined(INCLUDE_DOTLCD_TYPE_SPEC12832)
        SPEC12832DisplayPattern(dotmap);
#endif
}

extern void getDotLcdData(DOTLCDDISPLAY *lcd)
{
    memcpy(lcd,lcdDisplay,sizeof(lcdDisplay));
}
extern void setDotLcdData(DOTLCDDISPLAY *lcd)
{
    memcpy(lcdDisplay,lcd,sizeof(lcdDisplay));
}
#endif
