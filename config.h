/**
  ******************************************************************************
  * @file    config.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Archivo de cabecera con definiciones generales del sistema
  *
  ******************************************************************************
  */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>
#include "spec12832.h"

#pragma anon_unions //Define para poder usar estructuras y uniones anonimas.
#pragma pack(1)

//******************************************************************************//
//                         DEFINICIONES GENERALES                         			//
//******************************************************************************//

#define FIRMWARE_VERSION_COUNTRY    "VE"
#define FIRMWARE_VERSION_MAYOR      1      
#define FIRMWARE_VERSION_MINOR      8
#define HARDWARE_VERSION_ID      		1

#define MAX_DISPLAY		16
#define MARGEN  			2 
#define PLU_PROG 	 0xA5
#define PLU_NOPROG 0xFF

//#define MARCA		0x5A

//**************Estructura de banderas del sistema**************//
typedef struct {
    union {
        struct {
        unsigned bTecAlfabetico: 1;     /* Indicador de teclado alfabetico activo */
				unsigned bTecNumerico: 1;      /* Flag para mostrar descuento o recargo en pantalla*/
	
        unsigned bShiftActivo: 1;       /* Indicador de funciones secundarias */
        unsigned bIngresoDoble: 1;		/* Indicador de ingreso doble */
        unsigned bMenuAtivo: 1;			/* Indicador de menu activo */
        unsigned bMenuOtrosActivo: 1;   /* Indicador de menu de pago activo */
        unsigned bAckRec: 1;			/* Ack recibido */
        unsigned bNackRec: 1;			/* NAck recibido */
        unsigned bTimeout: 1;			/* Timeout en espera de ACK */
        //0////////////////////////
        unsigned bDecimal: 1;			/* Indicador de punto decimal */
        unsigned bTotalParcial: 1;		/* Ingreso de Modo de pago */
        unsigned bPorMonto: 1;			/* Ingreso de procentaje para descuento o recargo */
        unsigned bFondoRetiro: 1;		/* Fondo */
        unsigned bDeccomp: 1;			/* Indicador de digitos de campo completos */
        unsigned bWaitData: 1;
        unsigned bFechaNumero: 1;		/* Indicador de rango por fecha o numerico */
        unsigned bAnulacion: 1;		    /* Indicador de anulacion activa */
        //1////////////////////////////////
        unsigned bCmdRecive: 1;
        unsigned bModPC: 1;
        unsigned bTerceralinea: 1;
        unsigned bErase: 1;
        unsigned bCalc: 1;
        unsigned bAns: 1;
        unsigned bKeyboardScan;
        unsigned bNoUsado: 1;        
        //2//////////////////////////
        unsigned bDisplayTick: 1;
        unsigned bDisplayRotate: 1;
        unsigned bNoUsados: 6;
        };
        uint8_t cFlag[4]; 

    };
} tSYSTEMBITS;

typedef struct {
    union {
        struct{
            uint16_t tasa0;
            uint16_t tasa1;
            uint16_t tasa2;
            uint16_t tasa3;
        };
        uint16_t valor[4];
    };
} tTASAS_VAR;

typedef struct {
    union {
        struct{
            int32_t lExistencia[30];
            uint8_t cNoUsado[7];
            uint8_t cCRC;
          };
        uint8_t cValor[128];
    };
} tInventario;

/* Estructura de configuraciones generales */
typedef struct {
    union {
        struct {
            uint8_t     cMarca;                 /* 01*/
            uint16_t 	iClaveX;                /* 02 */
            uint16_t 	iClaveZ;                /* 02 */
            uint16_t 	iClaveS;                /* 02 */
            uint16_t    iClaveCajero[4];        /* 08 */
            struct {
                unsigned bPrecioFijo: 1;                
                unsigned bCodigoPLU: 1;
                unsigned bMensajeComercial :1;
								unsigned bMensajeOk :1;
								unsigned bConfigVenta :2;
                unsigned noUsados: 3;
            } cFlag;                            /* 01 */
        };
        uint8_t cValor[64];
    };
} tCONFIG_VAR;  //64

/* Estructura de control general */
typedef struct {
    uint8_t cEst;	/* Estado actual */
    uint8_t cOp;	/* Operacion actual */
} tCONTROL_VAR;

/* Estructura de PLU en memoria de trabajo */
typedef struct {
    union {
        struct  {
            uint8_t cMarca;             /* 01 */
            uint8_t cDepAsociado; 			/* 01 */
            uint64_t wPrecio;		    		/* 08 */
            uint8_t cCodigo[31];				/* 31 */
            uint8_t cDescripcion[81];   /* 79 */
        };                              /* 128 */
        uint8_t cValor[128];
    };
} tPLU_INFO;

/* Estructura de Departamento en memoria de trabajo */
typedef struct {
	union {
		struct {
			uint8_t cMarca;                 /* 01 */
			uint8_t cTasa;  				/* 01 */
      uint64_t wPrecioDepartamento;	/* 08 */
			uint64_t wPrecioLimite;		    /* 08 */
			uint8_t cDescripcion[40];	    /* 41 */
		};									/* 58 */
		uint8_t cValor[64];
	};
} tDEP_INFO;

/* Estructura de acceso directo */
typedef struct {
	union {
		struct {
			uint8_t cMarca;  			/* 01 */
            uint32_t wTeclaAsociada;    /* 04 Acceso directo */
			uint32_t wNumero_Asociado;  /* 04 Indica el numero de PLU asociado */
            uint8_t cTipoTecla;  	    /* 01 Indica si se trata de un PLU o un Departamento */
		};                              /* 10 */
		uint8_t cValor[16];
	};
} tDIRECTO_INFO;

// Estructura del mensaje comercial
typedef struct {
    union {
        struct {
            uint8_t cMarca;             /* 01 */
            uint8_t cMensaje[127];       /* 127 - Maximo 126 caracteres */
        };                              
        uint8_t cValor[128];
    };
} tMSG_COM;

//Estructura de informacion del monto que se esta ingresando
typedef struct {
	union {
		struct {
			uint8_t	sEnteros[10];			//Buffer de Digitos Enteros
			uint8_t sDecimales[4];		//Buffer de Digitos Decimales
			uint8_t cContadorEnteros;	//Contador de Enteros
			uint8_t cContadorDecimal;	//Contador de Decimales
			uint8_t cMaxEnteros;			//Cantidad de enteros permitida
			uint8_t cMaxDecimales;		//Cantidad de decimales permitidos
			uint64_t wEnteros;	//Contador de Enteros
			uint64_t wDecimal;	//Contador de Decimales
		};													//57
		uint8_t cValor[57];
	};
} tMONTO;

//Estructura de informacion del plu en curso
typedef struct {
	union {
		struct {
			uint8_t   cMarca;						//01
			uint8_t 	cTasa;  					//01
			uint16_t 	iIndex;  					//02
			uint64_t 	wCantidad;				//08
			uint64_t 	wPrecio;					//08
			uint8_t 	cDescripcion[41];	//41
		};														//61
		struct {
			uint8_t 	cModo;  					//01
			uint64_t 	wMonto;						//04
		};
		struct {
			uint8_t   cCodigo[31];      //31 //wpCodigo
			uint64_t 	wInicio;  				//08
			uint64_t 	wFin;							//08
			uint8_t   cNoUsado;					//01
		};														//48
		struct {
			uint64_t 	wIndex;  					//08
			uint8_t 	cContadorCh;  		//01
			uint8_t 	cChHex;  					//01
			uint8_t 	cPosRot;  				//01
			uint8_t 	cPosRotMax;  			//01
			uint8_t 	cTeclaBase;  			//01
			uint8_t 	cTexto[41];				//41
		};                          	//54
		uint8_t cValor[128];	            
	};
} tPLU;

//Estructura de PLU en memoria de trabajo
typedef struct {
	union {
		struct {
			uint8_t 	cPosCaracter;  		//01		//Indica la posicion en la que se esta escribiendo
			uint8_t 	cLinea; 					//01		//Indica la linea en la que se esta escribiendo
		};														//02
		uint8_t cValor[2];
	};
} tDISPLAY_INFO;

typedef struct {

    uint8_t	sEnteros[10];			//Buffer de Digitos Enteros
    uint8_t sDecimales[3];		//Buffer de Digitos Decimales
    uint8_t sNumerocomp[12];   //Numero completo sin la coma decimal
    uint8_t cContadorEnteros;	//Contador de Enteros
    uint8_t cContadorDecimal;	//Contador de Decimales
    uint8_t cMaxEnteros;			//Cantidad de enteros permitida
    uint8_t cMaxDecimales;		//Cantidad de decimales permitidos
    uint64_t wEnteros;	//Contador de Enteros
    uint64_t wDecimal;	//Contador de Decimales
    uint8_t Op;
    uint8_t tot;
		unsigned bCalc: 1; 		/* Indicador de calculadora activa */
		unsigned bCalc2: 1; 	/* Indicador de una operacion de calculo en proceso  */
	  unsigned bCalc3: 1;		/* Permite Realizar una operacion con el resultado de la anterior  */
} tCALC;

typedef struct
{
    uint8_t	sEnteros[10];			//Buffer de Digitos Enteros
    uint8_t sDecimales[4];		//Buffer de Digitos Decimales
    uint8_t sResultado[30];		//Buffer de digitos del resultado
    uint8_t cContadorEnteros;	//Contador de Enteros
    uint8_t cContadorDecimal;	//Contador de Decimales
    long long int wResultado;			//Resultado sin decimales
    uint64_t wEnteros;	//Contador de Enteros
    uint64_t wDecimal;	//Contador de Decimales
	
		uint8_t bzerokey;
	  unsigned bAnularFactura: 1;
		unsigned bAnularPlu: 1;
		unsigned bAnulacionActiva:1;	
		unsigned bDone:1;
		unsigned bVenta:1;
		
		unsigned bBorrarPlu:1;
		unsigned bBorrarACC:1;
		unsigned bTipo:1;
		
} tAnswer;

typedef struct {
    union {
        struct  {
            uint16_t iIndex;        /* 02 */
            uint32_t wCantidad; 		/* 04 */
            uint8_t cTipo;		   		/* 01 */
            uint8_t cMarca;					/* 01 */
//					uint64_t wPrecio;		    /* 08 */
        };                          /* 08 */
        uint8_t cValor[8];
    };
} tPLU_VENTA;

typedef  struct{
	union {
		struct{
			uint8_t cContador; /*  1 */   
		};
		 uint8_t cValor[1];
	};
} tPLU_CONTADOR;

typedef struct {
	uint16_t iIndex;      /* 02 */
  uint32_t wCantidad; 	/* 04 */
  uint8_t cTipo;		   	/* 01 */
  uint8_t cMarca;				/* 01 */
//						uint64_t wPrecio;		    /* 08 */
} tPLU_VENTA_INFO;

//Datos maximos en memoria de trabajo
#define MAX_PLU		3000
#define MAX_DEP		99
#define MAX_TASAS	3
#define MARCA		0x5A

#define STX 	0x02
#define ACK		0x06
#define NACK	0x15
#define ETX		0x03
#define ENQ     0x05


//Posibles estados del sistema
#define EST_ESPERA 				0x00
#define EST_FACTURA 			0x01
#define EST_NOTA_DEBITO 		0x02
#define EST_NOTA_CREDITO 		0x03
#define EST_FONDO_RETIRO		0x04
#define EST_REPORTES_MF 		0x05
#define EST_REPORTES_EJ			0x06
#define EST_REPORTES_PLU    0x10
#define EST_REPORTES_DEP    0x11
#define EST_PROGRAMACION		0x07
#define EST_PROGRAMACION_PLU	0x08
#define EST_PROGRAMACION_DEP	0x09
#define EST_PROGRAMACION_DIR	0x0A

#define EST_CALCULADORA					0x0B
#define EST_BORRAR_TODO_PLU   	0x0C
#define EST_IMP_REPORT_PLU 			0x0D
#define EST_IMP_REPORT_DEP      0x0E
#define EST_BORRAR_ACCESOS			0x0F
//#define EST_BORRAR_TODO_ACC			0x0F
#define EST_NO_OP               0x10

//Posibles operaciones
#define OP_NO					0x00		//Sin Operacion Abierta
//Operaciones de venta
#define OP_INGRESO			    0x01		//Ingresando un dato
#define OP_CANTIDAD 			0x02		//Ingresando cantidad
#define OP_PRECIO 				0x03		//Ingresando el precio
#define OP_PLU 					0x04		//Ingresando un numero de PLU o Departamento
//Operaciones de pagos y fondo retiro
#define OP_MODO 				0x05
#define OP_MONTO 				0x06
//Operaciones de descuento recargo
#define OP_REC_POR 				0x07
#define OP_DESC_POR 			0x08
#define OP_REC_MONTO 			0x09
#define OP_DESC_MONTO 			0x0A
//Operaciones de reportes
#define OP_RANGO_FECHA			0x0B
#define OP_RANGO_NUMERO			0x0C
//Operaciones de Programacion
#define OP_PROG_FECHA			0x0D
#define OP_PROG_HORA			0x0E
#define OP_PROG_MEDIOS			0x0F
#define OP_PROG_FLAGS			0x10
#define OP_PROG_TASAS			0x11
#define OP_PROG_PLU				0x12
#define OP_PROG_DEP				0x13
#define OP_PROG_TECLA			0x14
#define OP_PROG_TECLA_TIPO		0x15
#define OP_PROG_PRECIO			0x16
#define OP_PROG_PLU_TASAS		0x17
#define OP_PROG_PLU_EXI		0x3D
#define OP_PROG_EXI		0x3E
#define OP_CLAVE_X				0x18
#define OP_CLAVE_Z_EJ			0x19
#define OP_CLAVE_S				0x1A
#define OP_CAJERO				0x1B
#define OP_PROG_CAJERO			0x1C
#define OP_PROG_CLAVE			0x1D
#define OP_PROG_CONFIG			0x1E
#define OP_PROG_MENSAJE			0x1F

#define OP_PROG_CAJERO_NOMBRE   0x22
//Oraciones otros modos
#define OP_MENU	 				0x23
#define OP_MENU2     		0x40
#define OP_PC	 				0x24
#define OP_INFO					0x25
//Operaciones de Ingreso de Texto
#define OP_INGRESO_ID			0x26
#define OP_INGRESO_NOMBRE		0x27
#define OP_INGRESO_DIRECCION	0x28
#define OP_INGRESO_NUMERO_FO	0x29
#define OP_INGRESO_FECHA_FO		0x2A
#define OP_INGRESO_CONTROL_FO	0x2B
////Operaciones de Calculadora
//wplanchez
#define OP_SUMA					0x2C
#define OP_RESTA				0x2D
#define OP_MULTIPLICACION 		0x2E
#define OP_DIVISION 			0x2F
#define OP_TOTALIZAR 			0x30
#define OP_CALC           0x31

#define OP_RANGO_PLU_NUMERO  0x32
#define OP_NUMERO_PLU_ELIMINAR 0x33

#define OP_RANGO_ACC_NUMERO  0x34
#define OP_NUMERO_ACC_ELIMINAR 0x35

/* Operaciones de Imprimir Reporte PLUs */

#define	OP_INICIO 			0x36    	/* Indicador Inicar operacion*/
#define OP_PROCENSANDO  0x37     /* Indicador de enviando informacion PLU a la print  */
#define OP_REPETIR			0x38      /* Indicador de no se envio una informacion correctamente y se debe repetir el proceso */
#define OP_FINALIZADO		0x39       /* Indicador para cerrar el documento de impresion, limpiar variables y salir */

#define	OP_RANGO_NUMERO2 0x3A
#define OP_RANGO_NUMERO_INGRESO_DESDE 0x3B
#define OP_RANGO_NUMERO_INGRESO_HASTA 0x3C

#endif //END __CONFIG_H

