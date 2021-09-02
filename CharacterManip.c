#include "includes.h"
#include "LETRA8X12.h" 
#include "config.h"
#include "system.h"




//construye el mapa de bits 

//void CharToDot(uint32_t ch2,int Posx,int Posy,int Font,uint8_t basura )
void CharToDot(uint32_t ch2,int Posx,int Posy,int Font,uint8_t buf[] )
{ 
   uint32_t ascii;
     //uint8_t buf[BITMAP_SIZE];
    
     memset(cBitMap_buf,' ',sizeof(cBitMap_buf));
    
   ascii=ch2;
     memset(&cBitMap_buf[0*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+0], 1);
     memset(&cBitMap_buf[1*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+1], 1);
     memset(&cBitMap_buf[2*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+2], 1);
     memset(&cBitMap_buf[3*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+3], 1);
     memset(&cBitMap_buf[4*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+4], 1);
     memset(&cBitMap_buf[5*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+5], 1); 
     memset(&cBitMap_buf[6*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+6], 1);
     memset(&cBitMap_buf[7*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+7], 1);
     memset(&cBitMap_buf[8*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+8], 1);
     memset(&cBitMap_buf[9*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+9], 1);
     memset(&cBitMap_buf[10*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+10], 1);
     memset(&cBitMap_buf[11*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+11], 1);
     memset(&cBitMap_buf[12*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+12], 1);
     memset(&cBitMap_buf[13*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+13], 1);
     memset(&cBitMap_buf[14*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+14], 1);
     memset(&cBitMap_buf[15*SPEC12832ColumnNum/8+(Posy)+Posx], Fnt8L12[16*ascii+15], 1);
   SPEC12832DisplayPattern(cBitMap_buf); //Imprime el patrón de bits
    
}
void CharToDot_2(uint32_t ch2,int Posx,int Posy,int Font,uint8_t buf[])
{ 
   uint32_t ascii;
    
   ascii=ch2;
     memset(&buf[0*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+0], 1);
     memset(&buf[1*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+1], 1);
     memset(&buf[2*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+2], 1);
     memset(&buf[3*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+3], 1);
     memset(&buf[4*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+4], 1);
     memset(&buf[5*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+5], 1); 
     memset(&buf[6*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+6], 1);
     memset(&buf[7*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+7], 1);
     memset(&buf[8*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+8], 1);
     memset(&buf[9*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+9], 1);
     memset(&buf[10*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+10], 1);
     memset(&buf[11*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+11], 1);
     memset(&buf[12*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+12], 1);
     memset(&buf[13*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+13], 1);
     memset(&buf[14*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+14], 1);
     memset(&buf[15*SPEC12832ColumnNum/8+(Posy)+Posx], ~Fnt8L12[16*ascii+15], 1);
   SPEC12832DisplayPattern(buf); //Imprime el patrón de bits
    
}



