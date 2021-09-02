#include "includes.h"

#include "dotlcd.h"
#include "dotlcd_c.h"
#include "lcd.h"

#ifdef INCLUDE_MULTILINE_LCD

DOTLCDDISPLAY lcdDisplay[MLCDLINE],lcdDisplay_Bak[MLCDLINE];
uint8_t DotLcdMap[DOTLCD_DOT_ROW][(DOTLCD_DOT_COL+7)/8];
static CURSORPOS cursorPos;
#define CURSORTIME        25     //������� 300ms
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

//��  ��:����һ��LCD�ַ�����
//��  ��:��
//��  ��:��
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
//��  ��:���ù��
//��  ��:flag ��־
//��  ��:��
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

//��  ��:����DOTLCD��ʾ
//��  ��:��
//��  ��:��
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
                //�з���
                inversePattern(line);
            }
            memcpy((uint8_t *)&lcdDisplay_Bak[line],(uint8_t *)&lcdDisplay[line],sizeof(DOTLCDDISPLAY));
            updateflag = TRUE;
        }
    }
    if (cursorPos.line < MLCDLINE)
    {   //�й��
        if ((++cursorPos.timeCnt % CURSORTIME) == 0)
        {
            if ((cursorPos.timeCnt / CURSORTIME) & 0x01)
            {
                //�����
                setCursorMap(TRUE);
            }
            else
            {
                //����
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
//��  ��:��ʼ������LCD
//��  ��:��
//��  ��:��
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
    // ��ʼ������
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
//��  ��:��LCD����
//��  ��:para 0 ��   1 ��
//��  ��:��
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
//��  ��:���DOT LCD��ʾ��
//��  ��:��
//��  ��:��
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

//��  ��:���DOT LCD��ʾ��
//��  ��:��
//��  ��:��
extern void clearDotLcdLine(uint16_t line)
{
    if (line < MLCDLINE)
    {
        lcdDisplay[line].propStart = 0;
        lcdDisplay[line].propEnd = 0;
        memset(lcdDisplay[line].buffer,' ',MLCDBUFLEN);
    }
}

//��  ��:����LCD����
//��  ��:level �� / ��
//��  ��:��
extern void setDotLcdBackLight(uint8_t level)
{
    level = level;
}
//��  ��:��һ���ַ���DOT LCD
//��  ��:line   �к�
//       charidx �ַ����
//       ch      �ַ�
//��  ��:��
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
//��  ��:����LCD���׵�λ��
//��  ��:line   �к�
//       startIdx ��ʼ�ַ����
//       endIdx   �����ַ����
//��  ��:��
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
//��  ��:���ù��λ��
//��  ��:line   �к�
//       pos    λ��
//��  ��:��
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
