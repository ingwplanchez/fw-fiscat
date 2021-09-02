#include "includes.h"
#define DOUBLECODE      0x01
//#define ZOOMENBALE_

#ifdef ZOOMENBALE_
static uint8_t const FontWidthZoolCode_2[0x10]={0x00,0x03,0x0c,0x0f,0x30,0x33,0x3c,0x3f,0xc0,0xc3,0xcc,0xcf,0xf0,0xf3,0xfc,0xff};  // 4bit --> 1Byte
#endif

#ifdef FONTTYPE_8X16
#ifndef INCLUDE_SPIFLASH
#include "8x16_c.fnt"
#endif
static void fillFont_8X16(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fontflag)
{
    uint16_t i,j;
    uint8_t ch = 0;
    BOOL widthzoom;
#ifndef INCLUDE_SPIFLASH
    const uint8_t *fonttbl;
#else
    uint8_t *fonttbl;
    uint8_t fontbuf[FONT8X16_SIZE_BYTE+1];
#endif
    uint8_t *tmpdotbuf;
    widthzoom = FALSE;
    memset(dotbuf,0,linedotwidth*FONT8X16_HIGHPOINT);
    //printf("ch = %c, %d\r\n",ch,asclen);
    for(i=0 ; i<asclen ;i++)
    {
        ch = *ascbuf++;
        if (ch == DOUBLECODE)
        {
            widthzoom = TRUE;
        }
        else if (ch == ' ')
        {
            widthzoom = FALSE;
        }
        else
        {
        #ifndef INCLUDE_SPIFLASH
            fonttbl = &Font8x16Tbl[ch*FONT8X16_SIZE_BYTE];
        #else
            spiFlashRead(FONT8X16_ADDR+ch*FONT8X16_SIZE_BYTE,fontbuf,FONT8X16_SIZE_BYTE);
            fonttbl = fontbuf;
        #endif
            if (widthzoom)
            {
                #ifdef ZOOMENBALE_
                tmpdotbuf = dotbuf + i*FONT8X16_WIDTHPOINT/8 - 1;
                for(j=FONT8X16_HIGHPOINT ; j ; j--)
                {
                    *tmpdotbuf     = FontWidthZoolCode_2[*fonttbl >> 4];
                    *(tmpdotbuf+1) = FontWidthZoolCode_2[*fonttbl & 0x0f];
                    tmpdotbuf += linedotwidth;
                    fonttbl++;
                }
                #endif
                widthzoom = FALSE;
            }
            else
            {
                tmpdotbuf = dotbuf + i*FONT8X16_WIDTHPOINT/8;
                for(j=FONT8X16_HIGHPOINT ; j ; j--)
                {
                    *tmpdotbuf = *fonttbl;
                    tmpdotbuf += linedotwidth;
                    fonttbl++;
                }
            }
        }
    }
    fontflag = fontflag;
}
#endif
#ifdef FONTTYPE_10X24
#include "10x24_c.fnt"
static void fillFont_10X24(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fontflag)
{
    uint16_t i,j;
    uint8_t ch;
    BOOL widthzoom;
    const uint8_t *fonttbl;
    uint8_t *tmpdotbuf;
    widthzoom = FALSE;
    memset(dotbuf,0,linedotwidth*FONT10X24_HIGHPOINT);
    for(i=0 ; i<asclen ;i++)
    {
        ch = *ascbuf++;
        if (ch == DOUBLECODE)
        {
            widthzoom = TRUE;
        }
        else if (ch == ' ')
        {
            widthzoom = FALSE;
        }
        else
        {
            fonttbl = &Font10x24Tbl[ch*FONT10X24_SIZE_BYTE];
            if (widthzoom)
            {
                switch((i-1) & 0x03)
                {
                    case 0:
                        tmpdotbuf = dotbuf + ((i-1)/4)*5;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf     = FontWidthZoolCode_2[*fonttbl >> 4];
                            *(tmpdotbuf+1) = FontWidthZoolCode_2[*fonttbl & 0x0f];
                            fonttbl++;
                            *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl >> 6]<<4;
                            tmpdotbuf += linedotwidth;
                            fonttbl++;
                        }
                        break;
                    case 1:
                        tmpdotbuf = dotbuf + ((i-1)/4)*5 + 1;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 5];
                            *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 1) & 0x0f];
                            *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x01)] << 6;
                            fonttbl++;
                            *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 6] << 4;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 2:
                        tmpdotbuf = dotbuf + ((i-1)/4)*5 + 2;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 6];
                            *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 2) & 0x0f];
                            *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x03)] << 4;
                            fonttbl++;
                            *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 6];
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 3:
                        tmpdotbuf = dotbuf + ((i-1)/4)*5 + 3;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 3) & 0x0f];
                            *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x07)] << 2;
                            fonttbl++;
                            *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf + 3) |= FontWidthZoolCode_2[(*fonttbl >> 6) & 0x01] << 6;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                }

            }
            else
            {
                switch(i & 0x03)
                {
                    case 0:
                        tmpdotbuf = dotbuf + (i/4)*5;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf = *fonttbl++;
                            *(tmpdotbuf+1) = *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 1:
                        tmpdotbuf = dotbuf + (i/4)*5 + 1;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 2);
                            *(tmpdotbuf+1)  =  (*fonttbl << 6);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 2);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 2:
                        tmpdotbuf = dotbuf + (i/4)*5 + 2;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 4);
                            *(tmpdotbuf+1)  =  (*fonttbl << 4);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 4);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 3:
                        tmpdotbuf = dotbuf + (i/4)*5 + 3;
                        for(j=FONT10X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 6);
                            *(tmpdotbuf+1)  =  (*fonttbl << 2);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 6);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                }
            }
        }
    }
    fontflag = fontflag;
}
#endif
#ifdef FONTTYPE_11X24
#include "11x24_c.fnt"
static void fillFont_11X24(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fontflag)
{
    uint16_t i,j;
    uint8_t ch;
    BOOL widthzoom;
    const uint8_t *fonttbl;
    uint8_t *tmpdotbuf;
    widthzoom = FALSE;
    memset(dotbuf,0,linedotwidth*FONT11X24_HIGHPOINT);
    for(i=0 ; i<asclen ;i++)
    {
        ch = *ascbuf++;
        if (ch == DOUBLECODE)
        {
            widthzoom = TRUE;
        }
        else if (ch == ' ')
        {
            widthzoom = FALSE;
        }
        else
        {
            fonttbl = &Font11x24Tbl[ch*FONT11X24_SIZE_BYTE];
            if (widthzoom)
            {
                switch((i-1) & 0x07)
                {
                    case 0:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf     = FontWidthZoolCode_2[*fonttbl >> 4];
                            *(tmpdotbuf+1) = FontWidthZoolCode_2[*fonttbl & 0x0f];
                            fonttbl++;
                            *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl >> 5] << 2;
                            tmpdotbuf += linedotwidth;
                            fonttbl++;
                        }
                        break;
                    case 1:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 1;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf+1)    = FontWidthZoolCode_2[(*fonttbl >> 3) & 0x0f];
                            *(tmpdotbuf+2)    = FontWidthZoolCode_2[*fonttbl & 07] << 2;
                            fonttbl++;
                            *(tmpdotbuf+2)   |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf+3)    = FontWidthZoolCode_2[(*fonttbl >> 4) & 0x0f] << 4;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 2:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 2;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 6];
                            *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 2) & 0x0f];
                            *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x03)] << 4;
                            fonttbl++;
                            *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 6];
                            *(tmpdotbuf + 3)  = FontWidthZoolCode_2[(*fonttbl >> 5) & 0x01] << 6;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 3:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 4;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf    |= FontWidthZoolCode_2[*fonttbl >> 5];
                            *(tmpdotbuf+1) = FontWidthZoolCode_2[(*fonttbl >> 1)& 0x0f];
                            *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl & 0x01] << 6;
                            fonttbl++;
                            *(tmpdotbuf+2)|= FontWidthZoolCode_2[*fonttbl >> 5];
                            tmpdotbuf += linedotwidth;
                            fonttbl++;
                        }
                        break;
                    case 4:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11+5;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf     = FontWidthZoolCode_2[*fonttbl >> 4];
                            *(tmpdotbuf+1) = FontWidthZoolCode_2[*fonttbl & 0x0f];
                            fonttbl++;
                            *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl >> 5] << 2;
                            tmpdotbuf += linedotwidth;
                            fonttbl++;
                        }
                        break;
                    case 5:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 6;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf+1)    = FontWidthZoolCode_2[(*fonttbl >> 3) & 0x0f];
                            *(tmpdotbuf+2)    = FontWidthZoolCode_2[*fonttbl & 07] << 2;
                            fonttbl++;
                            *(tmpdotbuf+2)   |= FontWidthZoolCode_2[*fonttbl >> 7];
                            *(tmpdotbuf+3)    = FontWidthZoolCode_2[(*fonttbl >> 4) & 0x0f] << 4;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 6:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 8;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 6];
                            *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 2) & 0x0f];
                            *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x03)] << 4;
                            fonttbl++;
                            *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 6];
                            *(tmpdotbuf + 3)  = FontWidthZoolCode_2[(*fonttbl >> 5) & 0x01] << 6;
                            fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 7:
                        tmpdotbuf = dotbuf + ((i-1)/8)*11 + 9;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf    |= FontWidthZoolCode_2[*fonttbl >> 5];
                            *(tmpdotbuf+1) = FontWidthZoolCode_2[(*fonttbl >> 1)& 0x0f];
                            *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl & 0x01] << 6;
                            fonttbl++;
                            *(tmpdotbuf+2)|= FontWidthZoolCode_2[*fonttbl >> 5];
                            tmpdotbuf += linedotwidth;
                            fonttbl++;
                        }
                        break;

                }

            }
            else
            {
                switch(i & 0x07)
                {
                    case 0:
                        tmpdotbuf = dotbuf + (i/8)*11;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf = *fonttbl++;
                            *(tmpdotbuf+1) = *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 1:
                        tmpdotbuf = dotbuf + (i/8)*11 + 1;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 3);
                            *(tmpdotbuf+1)  =  (*fonttbl << 5);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 3);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 2:
                        tmpdotbuf = dotbuf + (i/8)*11 + 2;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 6);
                            *(tmpdotbuf+1)  =  (*fonttbl << 2);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 6);
                            *(tmpdotbuf+2)  =  (*fonttbl << 2);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 3:
                        tmpdotbuf = dotbuf + (i/8)*11 + 4;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 1);
                            *(tmpdotbuf+1)  =  (*fonttbl << 7);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 1);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 4:
                        tmpdotbuf = dotbuf + (i/8)*11 + 5;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 4);
                            *(tmpdotbuf+1)  =  (*fonttbl << 4);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 4);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 5:
                        tmpdotbuf = dotbuf + (i/8)*11 + 6;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 7);
                            *(tmpdotbuf+1)  =  (*fonttbl << 1);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 7);
                            *(tmpdotbuf+2)  =  (*fonttbl << 1);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 6:
                        tmpdotbuf = dotbuf + (i/8)*11 + 8;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 2);
                            *(tmpdotbuf+1)  =  (*fonttbl << 6);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 2);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                    case 7:
                        tmpdotbuf = dotbuf + (i/8)*11 + 9;
                        for(j=FONT11X24_HIGHPOINT ; j ; j--)
                        {
                            *tmpdotbuf      |= (*fonttbl >> 5);
                            *(tmpdotbuf+1)  =  (*fonttbl << 3);
                            fonttbl++;
                            *(tmpdotbuf+1)  |= (*fonttbl >> 5);
                            *fonttbl++;
                            tmpdotbuf += linedotwidth;
                        }
                        break;
                }
            }
        }
    }
    fontflag = fontflag;
}
#endif
#ifdef FONTTYPE_12X24
#include "12x24_c.fnt"
static void fillFont_12X24(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fontflag)
{
    uint16_t i,j;
    uint8_t ch;
    BOOL widthzoom;
    const uint8_t *fonttbl;
    uint8_t *tmpdotbuf;
    widthzoom = FALSE;
    memset(dotbuf,0,linedotwidth*FONT12X24_HIGHPOINT);
    for(i=0 ; i<asclen ;i++)
    {
        ch = *ascbuf++;
        if (ch == DOUBLECODE)
        {
            widthzoom = TRUE;
        }
        else if (ch == ' ')
        {
            widthzoom = FALSE;
        }
        else
        {
            if (fontflag)
            {
                fonttbl = &FontBold12x24Tbl[ch*FONT12X24_SIZE_BYTE];
            }
            else
            {
                fonttbl = &Font12x24Tbl[ch*FONT12X24_SIZE_BYTE];
            }
            if (widthzoom)
            {
                tmpdotbuf = dotbuf + i*FONT12X24_WIDTHPOINT/8 - 1;
                if (i & 0x01)
                {
                    for(j=FONT12X24_HIGHPOINT ; j ; j--)
                    {
                        *tmpdotbuf     = FontWidthZoolCode_2[*fonttbl >> 4];
                        *(tmpdotbuf+1) = FontWidthZoolCode_2[*fonttbl & 0x0f];
                        fonttbl++;
                        *(tmpdotbuf+2) = FontWidthZoolCode_2[*fonttbl >> 4];
                        tmpdotbuf += linedotwidth;
                        fonttbl++;
                    }
                }
                else
                {
                    for(j=FONT12X24_HIGHPOINT ; j ; j--)
                    {
                        *tmpdotbuf       |= FontWidthZoolCode_2[*fonttbl >> 6];
                        *(tmpdotbuf + 1)  = FontWidthZoolCode_2[(*fonttbl >> 2) & 0x0f];
                        *(tmpdotbuf + 2)  = FontWidthZoolCode_2[(*fonttbl & 0x03)] << 4;
                        fonttbl++;
                        *(tmpdotbuf + 2) |= FontWidthZoolCode_2[*fonttbl >> 6];
                        *(tmpdotbuf + 3) |= FontWidthZoolCode_2[(*fonttbl >> 4) & 0x03] << 4;
                        fonttbl++;
                        tmpdotbuf += linedotwidth;
                    }
                }
                widthzoom = FALSE;
            }
            else
            {
                tmpdotbuf = dotbuf + (i*FONT12X24_WIDTHPOINT)/8;
                if (i & 0x01)
                {
                    for(j=FONT12X24_HIGHPOINT ; j ; j--)
                    {
                        *tmpdotbuf      |= (*fonttbl >> 4);
                        *(tmpdotbuf+1)  =  (*fonttbl << 4);
                        fonttbl++;
                        *(tmpdotbuf+1)  |= (*fonttbl >> 4);
                        *fonttbl++;
                        tmpdotbuf += linedotwidth;
                    }
                }
                else
                {
                    for(j=FONT12X24_HIGHPOINT ; j ; j--)
                    {
                        *tmpdotbuf = *fonttbl++;
                        *(tmpdotbuf+1) = *fonttbl++;
                        tmpdotbuf += linedotwidth;
                    }
                }
            }
        }
    }
}
#endif

//函  数:取点阵字库的字模
//       要求dotbuf先清0
//参  数:dotbuf 点阵字库
//       ascbuf 字符
//       asclen 字符长度
//       fontype 字库类型
//返  回:无
extern void fillFontPartent(uint8_t *dotbuf,uint16_t linedotwidth,uint8_t *ascbuf,uint16_t asclen,uint16_t fonttype,uint16_t fontflag)
{
    switch(fonttype)
    {
#ifdef FONTTYPE_8X16
        case FONTTYPE_8X16:
            fillFont_8X16(dotbuf,linedotwidth,ascbuf,asclen,fontflag);
            break;
#endif
#ifdef FONTTYPE_10X24
        case FONTTYPE_10X24:
            fillFont_10X24(dotbuf,linedotwidth,ascbuf,asclen,fontflag);
            break;
#endif
#ifdef FONTTYPE_12X24
        case FONTTYPE_12X24:
            fillFont_12X24(dotbuf,linedotwidth,ascbuf,asclen,fontflag);
            break;
#endif
#ifdef FONTTYPE_11X24
        case FONTTYPE_11X24:
            fillFont_11X24(dotbuf,linedotwidth,ascbuf,asclen,fontflag);
            break;
#endif
        default:
            break;
    }
}

