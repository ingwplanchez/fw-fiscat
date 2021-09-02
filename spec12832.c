#include "includes.h"
#include "spec12832.h"
//=============================================================================
static uint8_t lcd_select;

#define SPEC12832CsHigh() \
    do{ \
        if(lcd_select & LCD_SELECT_USER) GPIOSetValue(1, P1_LCDCS1, 1);    \
            if(lcd_select & LCD_SELECT_CUSTOM)      GPIOSetValue(0, P0_LCDCS0, 1);    \
    }while(0)

#define SPEC12832CsLow() \
    do{ \
        if(lcd_select & LCD_SELECT_USER) GPIOSetValue(1, P1_LCDCS1, 0);    \
            if(lcd_select & LCD_SELECT_CUSTOM) GPIOSetValue(0, P0_LCDCS0, 0);    \
    }while(0)

#define SPEC12832CmdHigh() \
    do{ \
        GPIOSetValue(1, P1_LCDCD, 1);   \
    }while(0)

#define SPEC12832CmdLow() \
    do{ \
        GPIOSetValue(1, P1_LCDCD, 0);   \
    }while(0)
//=============================================================================
#define SPEC12832Bias1_7         (1)
#define SPEC12832Bias1_9         (0)
#define SPEC12832DrivingMode1    (3)
#define SPEC12832DrivingMode2    (0)
#define SPEC12832DrivingMode3    (1)
#define SPEC12832DrivingMode4    (2)
//=============================================================================
#define SPEC12832DisplayOn(on) \
    do{ \
        SPEC12832ShiftCmd(0xae | (on)); \
    }while(0)

#define SPEC12832DisplayStartLine(line) \
    do{ \
        SPEC12832ShiftCmd(0x40 | (line)); \
    }while(0)

#define SPEC12832SetPage(page) \
    do{ \
        SPEC12832ShiftCmd(0xb0 | (page)); \
    }while(0)

#define SPEC12832SetColumn(col) \
    do{ \
        SPEC12832ShiftCmd(0x00 | (col%2)<<3); \
        SPEC12832ShiftCmd(0x10 | (col>>1)); \
    }while(0)

#define SPEC12832ADCSelect(adc) \
    do{ \
        SPEC12832ShiftCmd(0xa0 | (adc)); \
    }while(0)

#define SPEC12832DisplayReverse(rev) \
    do{ \
        SPEC12832ShiftCmd(0xa6 | (rev)); \
    }while(0)

#define SPEC12832EntiredDisplay(all) \
    do{ \
        SPEC12832ShiftCmd(0xa4 | (all)); \
    }while(0)

#define SPEC12832SetBias(bias) \
    do{ \
        SPEC12832ShiftCmd(0xa2 | (bias)); \
    }while(0)

#define SPEC12832Reset() \
    do{ \
        SPEC12832ShiftCmd(0xe2); \
    }while(0)

#define SPEC12832CommonOutput(rev) \
    do{ \
        SPEC12832ShiftCmd(0xc0 | ((rev) << 3)); \
    }while(0)

#define SPEC12832PowerControl(mode) \
    do{ \
        SPEC12832ShiftCmd(0x28 | (mode)); \
    }while(0)

#define SPEC12832VoltageRegulator(ratio) \
    do{ \
        SPEC12832ShiftCmd(0x20 | (ratio)); \
    }while(0)

#define SPEC12832ElectronicVolume(val) \
    do{ \
        SPEC12832ShiftCmd(0x81); \
        SPEC12832ShiftCmd(val); \
    }while(0)

#define SPEC12832StaticIndicator(on) \
    do{ \
        SPEC12832ShiftCmd(0xac | (on)); \
    }while(0)

#define SPEC12832PageBlink(page) \
    do{ \
        SPEC12832ShiftCmd(0xd5); \
        SPEC12832ShiftCmd(page); \
    }while(0)

#define SPEC12832DrivingMode(d) \
    do{ \
        SPEC12832ShiftCmd(0xd2); \
        SPEC12832ShiftCmd(d << 6); \
    }while(0)
//=============================================================================
static void SPEC12832ShiftCmd(uint8_t cmd)
{
    //SPISemGet();
    SPEC12832CsLow();
    SPEC12832CmdLow();
    SPIShiftDataMSB(cmd);
    SPEC12832CsHigh();
    //SPISemPut();
}
//=============================================================================

static void SPEC12832ShiftDisplayData(uint8_t *dat, uint8_t len)
{
    //SPISemGet();
    SPEC12832CsLow();
    SPEC12832CmdHigh();
    for (; len; len--)
    {
        SPIShiftDataMSB(*dat++);
    }
    SPEC12832CsHigh();
    //SPISemPut();
}
//=============================================================================
// Level = 0~9
//=============================================================================
#define SPEC12832DisplayNormal      (0x20)
#define SPEC12832DisplayStep            (2)
//=============================================================================
extern void BackLightOnOff(uint8_t on)
{
if(on)
{
GPIOSetValue(2, P2_LIGHT, 1); // Turn On LED
}
else
{
GPIOSetValue(2, P2_LIGHT, 0); // Turn Off LED
}
}
extern void SPEC12832DisplayContrast(uint8_t level)
{
    SPEC12832ElectronicVolume((SPEC12832DisplayNormal-SPEC12832DisplayStep*4)+SPEC12832DisplayStep*level);  // 亮度电压调节 00H~03FH
}
//=============================================================================
//
//=============================================================================
extern void SPEC12832Init(void)
{
    SPEC12832Select(LCD_SELECT_USER | LCD_SELECT_CUSTOM);

    /* BackLight */
    GPIOSetDir(2, P2_LIGHT, 1);     // Turn On LED
    LPC_IOCON->PIO2_0 = IOCON_PIO;
    BackLightOnOff(1);

    /* LCD CS0 */
    GPIOSetDir(0, P0_LCDCS0, 1);
    GPIOSetValue(0, P0_LCDCS0, 1);
    LPC_IOCON->PIO0_1 = IOCON_PIO;

    GPIOSetDir(1, P1_LCDCS1, 1);
    GPIOSetValue(1, P1_LCDCS1, 1);
    LPC_IOCON->PIO1_8 = IOCON_PIO;

    SPEC12832CsHigh();

    /* LCD CMD */
    GPIOSetDir(1, P1_LCDCD, 1);
    LPC_IOCON->PIO1_5 = IOCON_PIO;
    SPEC12832CmdHigh();

    OSTimeDly(10);      // delay 10ms
    SPEC12832Reset();
    OSTimeDly(10);      // delay 10ms
    SPEC12832SetBias(SPEC12832Bias1_9);

    SPEC12832CommonOutput(1);
    SPEC12832ADCSelect(0);
    SPEC12832DisplayStartLine(0);

    SPEC12832VoltageRegulator(3);    // 1+Rb/Ra
    SPEC12832DisplayContrast(0);

    SPEC12832PowerControl(4);   // power control(VC,VR,VF=1, 0, 0)
    OSTimeDly(10);
    SPEC12832PowerControl(6);   // power control(VC,VR,VF=1, 1, 0)
    OSTimeDly(10);
    SPEC12832PowerControl(7);   // power control(VC,VR,VF=1, 1, 1)
    OSTimeDly(10);

    SPEC12832EntiredDisplay(0);
    SPEC12832DisplayReverse(0);
    SPEC12832DrivingMode(SPEC12832DrivingMode2);
    SPEC12832DisplayOn(1);

}
//=============================================================================

///**
//Funcion que imprime dos lineas, 128 columnas en 4 paginas (2 pag por linea)
//@p Puntero al mapa de bits
//@note  Nota importante.
//*/
//extern void SPEC12832DisplayPattern(uint8_t *p)
//{
//  uint8_t map[SPEC12832ColumnNum];
//  uint8_t page, col;
//  uint8_t i, c, bits;
//  for(page=0; page<(SPEC12832RowNum/8); page++)
//  {
//      SPEC12832SetPage(page);
//      SPEC12832SetColumn(0);
//      memset(map, 0, sizeof(map));
//      for(bits=0; bits<8; bits++)
//      {
//          i = 0;
//          for(col=0; col<SPEC12832ColumnNum; col++)
//          {
//              if(i == 0)
//              {
//                  c = *p++;
//              }
//              if(c & (1 << (7-i)))
//              {
//                  map[col] |= (1 << bits);
//              }
//              i = (i+1) & 0x07;
//          }
//      }
//      // send data to displayer
//      SPEC12832ShiftDisplayData(map, sizeof(map));
//  }
//}

/**
Funcion que imprime un caracter 8 columnas, 2 paginas
@p Puntero al mapa de bits
@x Posicion x en el mapa
@y Posicion y en el display
@note  Nota importante.
*/
//extern void SPEC12832DisplayPattern(uint8_t *p, uint8_t x,uint8_t y)
//{
//  uint8_t map[16];
//  uint8_t page,page_ref, col;
//  uint8_t i, c, bits;
//
//  if(y==16)
//  {
//      page_ref=0;
//  }
//  else
//  {
//      page_ref=2;
//  }
//  for(page=page_ref; page<(page_ref+2); page++)
//  {
//      SPEC12832SetPage(page);
//      SPEC12832SetColumn(x);
//      memset(map, 0, sizeof(map));
//      for(bits=0; bits<8; bits++)
//      {
//          i = 0;
//          for(col=0; col<8; col++)
//          {
//              if(i == 0)
//              {
//                  c = *p++;
//              }
//              if(c & (1 << (7-i)))
//              {
//                  map[col] |= (1 << bits);
//              }
//              i = (i+1) & 0x07;
//          }
//      }
//      // send data to displayer
//      SPEC12832ShiftDisplayData(map,sizeof(map));
//  }
//}
extern void SPEC12832DisplayPattern_logo(uint8_t *p)
{
    uint8_t map[SPEC12832ColumnNum];
    uint8_t page, col;
    uint8_t i, c, bits;
    for(page=0; page<(SPEC12832RowNum/8); page++)
    {
        SPEC12832SetPage(page);
        SPEC12832SetColumn(0);
        memset(map, 0, sizeof(map));
        for(bits=0; bits<8; bits++)
        {
            i = 0;
            for(col=0; col<SPEC12832ColumnNum; col++)
            {
                if(i == 0)
                {
                    c = *p++;
                }
                if(c & (1 << (7-i)))
                {
                    map[col] |= (1 << bits);
                }
                i = (i+1) & 0x07;
            }
        }
        // send data to displayer
        SPEC12832ShiftDisplayData(map, sizeof(map));
    }
}

extern void SPEC12832DisplayPattern(uint8_t *p, uint8_t x,uint8_t y)
{
    uint8_t map[16];
    uint8_t page,page_ref, col;
    uint8_t i, c, bits;

    if(y==16)
    {
        page_ref=0;
    }
    else
    {
        page_ref=2;
    }
    for(page=page_ref; page<(page_ref+2); page++)
    {
        if(tSys_flag.bErase==0) //Si debe aparecer la tercera linea del display, no se toma la pagina 3
        {
            if(page==3)
                page++;
        }
//      if(page!=3)
//      {
        SPEC12832SetPage(page);
        SPEC12832SetColumn(x);
        memset(map, 0, sizeof(map));
        for(bits=0; bits<8; bits++)
        {
            i = 0;
            for(col=0; col<8; col++)
            {
                if(i == 0)
                {
                    c = *p++;
                }
                if(c & (1 << (7-i)))
                {
                    map[col] |= (1 << bits);
                }
                i = (i+1) & 0x07;
            }
        }
        // send data to displayer
        SPEC12832ShiftDisplayData(map,sizeof(map));
//  }
}
}


extern void shift_display( uint8_t *p,uint8_t x)
{
    uint8_t map[16];
    uint8_t  col,i,c, bits;

    SPEC12832Select(LCD_SELECT_USER);

    SPEC12832DisplayStartLine(0);
    SPEC12832SetPage(3); //Imprime unicamente los bits de las parte inferior del display
    SPEC12832SetColumn(x);
    memset(map, 0x00, sizeof(map));
    for(bits=1; bits<8; bits++)
    {
        i = 0;
        for(col=0; col<8; col++)
        {
            if(i == 0)
            {
                    c = *p++;
            }
            if(c & (1 << (7-i)))
            {
                map[col] |= (1 << bits);
            }
            i = (i+1) & 0x07;
        }
    }
        SPEC12832ShiftDisplayData(map,8);
//        SPEC12832ShiftDisplayData(map,sizeof(map));

}
//=============================================================================
extern void SPEC12832Select(uint8_t lcd)
{
    lcd_select = lcd;
}
//=============================================================================

