#ifndef __LCD_H__
#define __LCD_H__
#include "includes.h"

#define MAINLCD         0x01
#define CUSTLCD         0x02
extern void initLcd(void);
extern void lcd_displayString(uint8_t lcdtype,uint8_t texttype,uint8_t row,uint8_t col,uint8_t *str);
extern void lcd_displayMiddleStr(uint8_t lcdtype,uint8_t line,uint8_t *str);
extern int lcd_paint(uint8_t lcdtype,uint8_t startrow,uint8_t startcol,uint8_t width,uint8_t height,uint8_t *dotdat);
extern int lcd_recHightlight(uint8_t lcdtype,uint8_t startrow,uint8_t startcol,uint8_t width,uint8_t height);
extern void display_logo(void);
extern uint8_t testLcd(uint8_t lcdtype);

#endif

