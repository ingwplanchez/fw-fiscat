#ifndef _SPEC12832_H_
#define _SPEC12832_H_
//======================================================================================================
#define SPEC12832ColumnNum      (128)
#define SPEC12832RowNum     (32)

//======================================================================================================
#define LCD_SELECT_USER     (1 << 0)
#define LCD_SELECT_CUSTOM   (1 << 1)
#define DOTLCD_DOT_ROW      SPEC12832RowNum
#define DOTLCD_DOT_COL      SPEC12832ColumnNum
#define CUSTOMER_LCD_DOT_ROW      SPEC12832RowNum
#define CUSTOMER_LCD_DOT_COL      SPEC12832ColumnNum
//======================================================================================================
extern void BackLightOnOff(uint8_t on);
extern void SPEC12832DisplayContrast(uint8_t level);
extern void SPEC12832Init(void);
extern void SPEC12832Select(uint8_t lcd);
extern void SPEC12832DisplayPattern_logo(uint8_t *p);
extern void SPEC12832DisplayPattern(uint8_t *cBitMap_buf,uint8_t Posx, uint8_t Posy); //Imprime el patrón de bits   
extern void shift_display( uint8_t *cBitMap_buf2,uint8_t Posx);
//======================================================================================================
#endif

