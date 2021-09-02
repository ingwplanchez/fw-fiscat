#include "system.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include "uart.h"


//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//


//**********************DECLARACION DE VARIABLES LOCALES************************//
tCONFIG_VAR tConfig={NULL};
tCONTROL_VAR tControl={NULL};
tSYSTEMBITS tSys_flag={NULL};       //Banderas generales del sistema (interno)
tDISPLAY_INFO tDisplay={NULL};
tMONTO tIngresoMonto={NULL};
tPLU tPluActual={NULL};
uint8_t tTasas[]={' ','!','"','#','$','%'};
tTASAS_VAR g_tasas;
uint8_t DotLcdMap[DOTLCD_DOT_ROW][(DOTLCD_DOT_COL+7)/8];
uint8_t custDotLcdMap[CUSTOMER_LCD_DOT_ROW][(CUSTOMER_LCD_DOT_COL+7)/8];
uint8_t cBufAux[64]={NULL};
uint8_t cBufAux2[256] = {NULL};
uint8_t cBufGen[136]={NULL};

volatile uint16_t iTimeOut=0;
tCALC tCal1={NULL}; //Valor Antes del signo de operación en modo calculador
tCALC tCal2={NULL};//Valor despues del signo de operación en modo calculadora
tCALC tFlag = {NULL}; // Bandera para limmpiar pantalla en Modo Calculadoa
tAnswer tAns={NULL};

uint64_t wCal1={NULL}; 	//Valor Antes del signo de operación en modo calculador
uint64_t wCal2={NULL};	//Valor despues del signo de operación en modo calculadora
uint64_t wAns={NULL};

uint8_t cIntentos; //Valor para llevar los intentos de repetir la impresion de los datos de un PLU por si la impresora responde FALSE.
int16_t iContador; //Valor para llevar los PLU procesados al momento de imprimir el reporte
uint8_t cIntentos; //Valor para llevar los intentos de repetir la impresion de los datos de un PLU por si la impresora responde FALSE.
uint16_t iStart;
uint16_t iEnd;

uint8_t cMenuAct;
uint8_t cMenuAux;

uint8_t cStockM;
uint32_t lCant;

uint8_t cCONT;
uint8_t cCant=1;

tPLU_VENTA_INFO tVenta={NULL};


