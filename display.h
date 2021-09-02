/**
  ******************************************************************************
  * @file    display.h
  * @author  THE FACTORY HKA C.A.
  * @version v01r00
  * @date    Marzo-2015
  * @brief   Libreria de manejo del display (funciones graficas)
  *          
  ******************************************************************************
  */	
	
#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdint.h>


//******************************************************************************//
//                         DEFINICIONES GENERALES                         			//
//******************************************************************************//
#define FONDO_NEGRO		    0
#define FONDO_BLANCO	    1

#define LINEA_SUPERIOR      0
#define LINEA_INFERIOR		1
#define AMBAS_LINEAS 		2

#define DISPLAY_OPERADOR	0
#define DISPLAY_CLIENTE		1
#define DISPLAY_AMBOS 		2

//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//											PROTOTIPOS DE FUNCIONES PUBLICAS												//
//******************************************************************************//

void display_print(uint8_t *pText, uint8_t cLine, uint8_t cFondo, uint8_t nDisplay);
void display_message_ini(void);
void display_erase_line(uint8_t cLine, uint8_t nDisplay);
void mensaje_display(uint16_t nrodemensaje, uint8_t nDisplay);
void display_print_un_caracter(uint8_t caracter,int posx, uint8_t cLine, uint8_t nDisplay);
void display_ini(void);
void CharToDot(uint8_t ch, size_t posx, size_t posy,uint8_t cFondo);
void CharToDot_3(uint8_t ch, size_t posx, uint8_t cFondo3);
void display_print2(uint8_t *pText, uint8_t cLine,uint16_t lcdini, uint16_t lcdfin,uint8_t cFondo2);
void Display_rotateWrite(char *source, uint8_t line);
void Display_rotateTick(void);
void Display_rotateSet(void);
void Display_rotateUnSet(void);

#endif 
