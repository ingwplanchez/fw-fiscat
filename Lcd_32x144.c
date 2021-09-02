#include "includes.h"

#ifdef INCLUDE_LCD_32x144

#include "Fnt8x12.h"
#include "Fnt5x7.h"

static UFix08 FontBuf[16];                                  // ×Ö¿âµãÕó»º³åÇø

static void GetFont8L12(UAuto08 ch,UAuto08 Prop)
{
    memcpy(FontBuf,&Fnt8L12[ch*16],16);
    if (Prop)
    {
        for (ch=0 ; ch<16 ; ch++)
        {
            FontBuf[ch] ^= 0xff;
        }
    }
}

static void ChangeFont8L12(void)
{
    UAuto16 x;
    UAuto08 i;
    UFix08 *pt;

    pt = FontBuf;
    for (i=0 ; i<8 ; i++)
    {
        x = (pt[0] * 0x100 + pt[1]) >> 4;
        *pt++ = x / 0x100;
        *pt++ = x;
    }
}

static void GetFont5x7(UAuto08 ch)
{
    memcpy(FontBuf, &Char5x7[ch * 6],6);
}

extern void LcdCharToLcdDot_32x144(void)
{
    UAuto08 i,j;

    memset(DotLcdDotBuf,0,sizeof(DotLcdDotBuf));
    for (i=0 ; i<MLCDBUFLEN ; i++)
    {
        GetFont8L12(DotLcdBuf[0][i],LcdPropBuf[0][i]);
        ChangeFont8L12();
        for (j=0 ; j<8 ; j++)
        {
            DotLcdDotBuf[0][(i<<3)+j] = FontBuf[(j<<1)+1];
            DotLcdDotBuf[1][(i<<3)+j] = FontBuf[j<<1];
        }
    }
    for (i=0 ; i<MLCDBUFLEN ; i++)
    {
        GetFont8L12(DotLcdBuf[1][i],LcdPropBuf[1][i]);
        for (j=0 ; j<8 ; j++)
        {
            DotLcdDotBuf[1][(i<<3)+j] |= (FontBuf[(j<<1)+1] & 0xf0);
            DotLcdDotBuf[2][(i<<3)+j] = FontBuf[j<<1];
        }
    }
    for (i=0 ; i<SoftLcdBufLen; i++)
    {
        GetFont5x7(SoftLcdBuf[i]);
        
        for (j=0 ; j<6 ; j++)
        {
            DotLcdDotBuf[3][i*6+j] = FontBuf[j];
        }
    }
}

#endif

