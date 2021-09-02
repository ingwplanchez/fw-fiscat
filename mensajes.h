#ifndef __MENSAJES_H_
#define __MENSAJES_H_

#include <stdint.h>


//***************************************************************************************************************//
//                                                                                                      Definiciones                                               //
//***************************************************************************************************************//

#define SIN_MENSAJE             -1
#define MENSAJE_MARCA           0
#define MENSAJE_RANGO_NUM1      1
#define MENSAJE_RANGO_NUM2      2
#define MENSAJE_RANGO_FECHA1    3
#define MENSAJE_RANGO_FECHA2    4
#define MENSAJE_MONTO           5
#define MENSAJE_TIPO            6
#define MENSAJE_DESC            7
#define MENSAJE_REC             8
#define MENSAJE_FECHA           9
#define MENSAJE_HORA            10
#define MENSAJE_TASAS           11

#define MENSAJE_NOMBRE          12
#define MENSAJE_ID              13
#define MENSAJE_DIRECCION       14
#define MENSAJE_FACTURA         15
#define MENSAJE_FECHA_FAC       16
#define MENSAJE_CONTROL_FAC     17
#define MENSAJE_ABIERTO         18

#define MENSAJE_ENVIO           19
#define MENSAJE_CMD_OK          20
#define MENSAJE_TIMEOUT         21
#define MENSAJE_ERROR_INGRESO   22
#define MENSAJE_ERROR_COMANDO   23

#define MENSAJE_NUMERO          24
#define MENSAJE_VALOR           25
#define MENSAJE_DEP             26
#define MENSAJE_PLU             27
#define MENSAJE_TASA            28
#define MENSAJE_PRECIO          29
#define MENSAJE_PRECIO_MAX      30
#define MENSAJE_CODIGO          31
#define MENSAJE_TECLA           32

#define MENSAJE_MODO_PC         33
#define MENSAJE_CAJERO          34

#define MENSAJE_CLAVE          	35

#define MENSAJE_CALCULADORA			36
#define MENSAJE_ERROR_DIV       37
#define MENSAJE_SUMA						38
#define MENSAJE_RESTA						39
#define MENSAJE_MULTIPLICACION	40
#define MENSAJE_DIVISION				41
#define MENSAJE_PORCENTAJE			42
#define MENSAJE_RESULTADO				43

#define		MENSAJE_BORRADO_EXITO 44
#define		MENSAJE_BORRADO_TODOS	45
#define		MENSAJE_BORRADO_ERROR 46
	
#define   MENSAJE_PROG_EXITO 47
#define   MENSAJE_PROG_ERROR 48
#define 	MENSAJE_IMP_REPORT_PLU 49
#define   MENSAJE_IMP_REPORT_DEP 50
#define 	MENSAJE_EXISTENCIA          51
#define 	MENSAJE_STOCK_CERO          52
extern uint8_t mensaje_leer(uint8_t *buf, uint8_t Numero);


#endif

