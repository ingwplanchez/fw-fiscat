#ifndef __DISPLAY_H_
#define __DISPLAY_H_

#include "general.h"

#define _LINEA_1 _DEV_VFD
#define _LINEA_2 _DEV_MULTILINE
#define _LINEA_3 _DEV_MULTILINE+1

#define _LENTH_DISPLAY 	18

#define MAX_WIDE_DISPLAY 9999999990000L

//=======================Variables de Control===========================
extern char lock_disp_clock;
//===============================================================
extern void disp_init(void);
extern void disp_clear(void);
extern void disp_event(char linea);
extern void disp_enviar(char *buf, char linea);
extern void disp_clock(void);
extern void disp_clock_lcd(void);
extern void disp_inicio(void);
extern void disp_alarma(void);
extern void disp_monto(char *buf, INT64 monto);
extern void handler_lcd_buffer(BYTE tipo,char * buff,INT16 len);
extern void disp_high_light(char _linea, char _enable);
extern void disp_set_menu(char * _buf);

#endif


