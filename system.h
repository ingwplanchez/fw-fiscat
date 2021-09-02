/**
  ******************************************************************************
  * @file    system.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria para definición de variables globales
  *          
  ******************************************************************************
  */
    
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "mensajes.h"
#include "Timer.h"
#include "type.h"

//--------------------------------------------------------------------------------------------//
//Declaracion de Variables Globales                                                           //
//--------------------------------------------------------------------------------------------//

//----------------------------Variables Generales del Sistema---------------------------------//

extern tCONFIG_VAR tConfig;
extern tCONTROL_VAR tControl;
extern tSYSTEMBITS tSys_flag;       //Banderas generales del sistema (interno)
extern tDISPLAY_INFO tDisplay;
extern tMONTO tIngresoMonto;
extern tPLU tPluActual;
extern uint8_t tTasas[];
extern uint8_t cBufAux[64];
extern uint8_t cBufAux2[256];
extern uint8_t cBufGen[136];
extern volatile uint16_t iTimeOut;
extern uint8_t DotLcdMap[DOTLCD_DOT_ROW][(DOTLCD_DOT_COL+7)/8];
extern uint8_t custDotLcdMap[CUSTOMER_LCD_DOT_ROW][(CUSTOMER_LCD_DOT_COL+7)/8];
extern tCALC tCal1; //Valor Antes del signo de operación en modo calculador
extern tCALC tCal2; //Valor despues del signo de operación en modo calculadora
extern tCALC tFlag;
extern tAnswer tAns;

extern uint64_t wCal1;//Valor Antes del signo de operación en modo calculador
extern uint64_t wCal2;	//Valor despues del signo de operación en modo calculadora
extern uint64_t wAns;

extern uint8_t cIntentos; //Valor para llevar los intentos de repetir la impresion de los datos de un PLU por si la impresora responde FALSE.
extern int16_t iContador; //Valor para llevar los PLU procesados al momento de imprimir el reporte
extern uint8_t cIntentos; //Valor para llevar los intentos de repetir la impresion de los datos de un PLU por si la impresora responde FALSE.
extern uint16_t iStart;
extern uint16_t iEnd;

extern tTASAS_VAR g_tasas;

extern uint8_t cMenuAct;
extern uint8_t cMenuAux;

extern uint8_t cStockM;
extern uint32_t lCant;

extern uint8_t cCONT;
extern uint8_t cCant;

extern tPLU_VENTA_INFO tVenta;

//---------------------------------------------------------------------------------------------//


#endif
