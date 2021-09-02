#include "config.h"
#include "string.h"
#include "mensajes.h"

//Se suprimen los warning: unrecognized character escape sequence
#pragma diag_suppress 192


// son los mensajes que no son modificables por el usuario
 char pMensaje[][17 + 1] =
{
    //Descriptores                              ////
    "THE FACTORY HKA ",           //00    
    "DESDE:          ",           //01  
    "HASTA:          ",           //02
    "DESDE:AAMMDD   ",            //03 
    "HASTA:AAMMDD   ",           	//04    
    "MONTO:          ",           //05
    "TIPO:           ",           //06  
    "DESC:           ",           //07
    "REC:            ",           //08  
    "FECHA: DDMMAA   ",           //09
    "HORA: HHMMSS    ",           //10
    "TASAS: tT1tT2tT3",           //11
    //Personalizacion                           
    "NOMBRE:"         ,           //12
    "RIF/CI:"         ,           //13  
    "DIRECCION:"      ,           //14    
    "FACTURA:"        ,           //15    
    "FECHA:"          ,           //16    
    "CONTROL:"        ,           //17    
    "DOC. ABIERTO!!  ",           //18  
    //Mensajes de Envio                    
    "ENVIANDO...     ",           //19
    "ENVIO EXITOSO!! ",           //20
    //Mensajes de Error                     
    "ERROR TIMEOUT   ",           //21      
    "ERROR DE INGRESO",           //22
    "ERROR DE COMANDO",           //23
    "NRO:            ",           //24        
    "VALOR:          ",           //25  
    "DEP:            ",           //26  
    "PLU:            ",           //27  
    "TASA:           ",           //28  
    "PREC:           ",           //29  
    "MAX:            ",           //30  
    "COD:            ",           //31  
    "TECLA:          ",           //32
    "*** MODO  PC ***",           //33
    "   SIN CAJERO",            	//34
    "CLAVE:",                     //35      
    "   CALCULADORA   ",					//36
    "   Math ERROR   ",					//37
		"+               ",					//38
		"-               ",					//39
		"x               ",					//40
		"/               ",					//41
		"%               ",					//42
		"=               ",					//43
		"BORRADO OK!!!",						//44
		"BORRANDO...",							//45
		"BORRADO ERROR!!!",					//46
		"PROG OK!!!",								//47
		"PROG ERROR!!!",            //48
		"  REPORTE PLU   ",         //49
		"  REPORTE DEP   ",					//50
		"STOCK:          ",					//51
		"STOCK EN CERO   "					//52
};


/**
Funcion que permite leer un texto del arreglo de mensajes
@buf Puntero al buffer donde se copiara el texto
@Numero Identificador del mensaje
@retval Resultado de la operacion
@note  Nota importante.
*/
uint8_t mensaje_leer(uint8_t *buf, uint8_t Numero)
{
    strcpy((char*) buf,(char *)pMensaje[Numero]);

    return 1;
}



