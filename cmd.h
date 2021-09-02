/**
  ******************************************************************************
  * @file    cmd.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de control de comandos protocolo TFHKA
  *          
  ******************************************************************************
  */	
	
#ifndef __CMD_H
#define __CMD_H

#include "system.h"


#pragma anon_unions //Define para poder usar estructuras y uniones anonimas.
#pragma pack(1)


//******************************************************************************//
//                         DEFINICIONES GENERALES                         			//
//******************************************************************************//

typedef enum
{
/*+-------------------------------------------------------------------------+*/
/*|			Comando							|		Valor		|						Descripcion							|*/
/*+-------------------------------------------------------------------------+*/
	/*CMD_INVALIDO							=		0x00,
//comandos internos
	CMD_RESET							    =		0x01,
//comandos de registro de item
	CMD_PLU_EX								=		0x20,
	CMD_PLU_IMP1							=		0x21,
	CMD_PLU_IMP2							=		0x22,
	CMD_PLU_IMP3							=		0x23,
//comandos para abrir documentos
	CMD_NO_SALE								=		0x30,		// no venta
//comandos de pago
	CMD_PAGO_TOTAL						=		0x31,		// pago total
	CMD_PAGO_PARC							=		0x32,		// pago parcial
	CMD_SUB_IMP								=		0x33,		// subtotal con impresion
	CMD_SUB_NO_IMP						=		0x34,		// subtotal sin impresion*/
//comandos de manejo de cajero
	//CMD_CAJERO_ON							=		0x35,
	//CMD_CAJERO_OFF						=		0x36,
	//CMD_CANCEL								=		0x37,		// cancelar comprobante
	//CMD_MENSAJE								=		0x38,		// docuemnto no fiscal
	//CMD_PO_RA									=		0x39,		// paid out (retiro) / received (fondo)
	//CMD_GRPRS									=		0x3B,		// envio manual de Z
	//CMD_GPRS_CONF							=		0x3C,		// configuracion de primer z envio
	//CMD_MENS_VTA							=		0x40,   // mensaje adicional para el plu
	//CMD_MENS_DEV    					=		0x41,  	// mensaje adicional para el plu en notas de credito
	//CMD_MENS_DEB							=		0x42,   // mensaje adicional para el plu en notas de debito
	//CMD_COD_DESB							=		0x43,		// codigo de desbloqueo
	//CMD_PRINT_PROG						=		0x44,   // 'D' imprimir prog
	//CMD_PRINT_INFO						=		0x45,   // 'E' electronic journal
	//CMD_NCF										=		0x46,		// 'F' comando para el NCF
	//CMD_GENERAL								=		0x47,		// 'G' comando general
	//CMD_PRINT_GPRS						=		0x48,		// 'H' reporte gprs
	CMD_REPORTE								=		0x49,   // 'I' reporte
	CMD_CLEAR_X2_Z2 					=		0x58,		// 'X' borrar acumulados
	/*CMD_PROGRAM								=		0x50,   // 'P' programacion
	//CMD_TEST_IMAGEN						=		0x51,   // 'Q' cmd_test_imagen(void)  //hector borrar
	CMD_REPRINT								=		0x52,   // 'R' reimpresion electronic journal
	//CMD_INFO									=		0x53,		// 'S' info status
	CMD_TRAINING							=		0x54,   // 'T' Training
	//CMD_UPLOAD								=		0x55,   // 'U' Upload
	//CMD_LIST_ERROR						=		0x56,   // 'V' lista los errores que desencadeno un comando en particular
	CMD_CLEAR_X2_Z2 					=		0x58,		// 'X' borrar acumulados
	CMD_BARCODE1							=		0x59,		// codigo de barras por articulo
	//CMD_DIS_TIME							=		0x61,		// manejo del display
	//CMD_COMERCIAL							=		0x62,
	//CMD_DIS_MENS							=		0x63,
	CMD_DEVOLUCION						=		0x64,		// inicio de devolucion
	//CMD_RESTART								=		0x65,		// reset por software
	//CMD_BOOT									=		0x67,		//
	CMD_CLIENTE								=		0x69,		// ingreso de informacion del cliente
	CMD_CORRECT								=		0x6B,		// elimina la ultima operacion
	CMD_PORC_LEY							=		0x6C,		// elimina la ultima operacion
	//CMD_BOOT_GPRS							=		0x6F,		// inicio de bootloader del gprs
	CMD_DESC_PORC							=		0x70,		// descuentos porcentaje
	CMD_DESC_MONT							=		0x71,		// descuentos monto
	CMD_DESC_PORC_OR					=		0x72,		// descuentos porcentaje sobre monto original
	CMD_PO_RA_TOT							=		0x74,		// monto de fondo o retiro
	//CMD_GAVETA								=		0x77,		// abre gaveta
	//CMD_TEST_PRINT						=		0x78,		// test de impresion
	//CMD_BARCODE2							=		0x79,		// codigo de barras al pie
	//CMD_ANULAR_EX							=		0x6B,		// anulacion de plus (anterior A0)
	//CMD_ANULAR_IMP1						=		0xA1,
	//CMD_ANULAR_IMP2						=		0xA2,
	//CMD_ANULAR_IMP3						=		0xA3,
	//comandos de pago
	//CMD_PAGO_TOTAL_ANULA			=		0xE0,		// pago total
	//CMD_PAGO_PARC_ANULA				=		0xE1,		// pago parcial
	//CMD_DESC_PORC_ANULA				=		0xF0,		// anulacion de descuento por monto
	//CMD_DESC_MONT_ANULA				=		0xF1,	  // anulacion de descuento porcentaje
	//CMD_DESC_PORC_OR_ANULA		=   0xF2		// anulacion de descuento al monto original*/
	
}tCMD_TFHKA;




//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//											PROTOTIPOS DE FUNCIONES PUBLICAS												//
//******************************************************************************//

extern uint8_t cmd_send(uint8_t *cBuffer, uint16_t iLen, uint8_t cContAck,uint8_t bMensajeIni,int16_t nrodemensaje);
extern uint8_t cmd_send_recv(uint8_t *buffer, size_t length, uint8_t cmd);
extern void cmd_task(void);
extern uint8_t cal_bcc(uint8_t *pBuf, int16_t iLen);

#endif 
