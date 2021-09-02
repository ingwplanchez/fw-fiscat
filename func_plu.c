#include "func.h"
#include "func_plu.h"
#include "display.h"
#include "uart.h"
#include "cmd.h"
#include "system.h"
#include "mem.h"
#include "menu.h"

//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//


//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//
//static void verifica_tasas(void);
//static uint64_t calcula_precio(uint64_t precio, uint8_t tasa);

//******************************************************************************//
//                                                  DEFINICION DE FUNCIONES                             //
//******************************************************************************//

//==============================Ingreso de cantidades, Procesar datos, Igualar, Borrar===============================

/**
Funcion encargada de convertir los datos ingresados a un monto
@enteros número maximo de enteros permitidos en la trama
@decimales número máximo de decimales permitidos en la trama
@poslcd posicion del cursor en el display
@note  Nota importante.
*/
uint8_t func_plu_procesa_datos(void)
{
    uint8_t cResp=TRUE, cAux[16];
    uint16_t i=0;
    if(((tControl.cOp>=OP_INGRESO) && (tControl.cOp<=OP_PLU)) || (tControl.cOp==OP_SUMA)
    || (tControl.cOp==OP_DIVISION) || (tControl.cOp==OP_RESTA))
    {
        display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR,DISPLAY_OPERADOR);
        tDisplay.cPosCaracter++;

        tDisplay.cPosCaracter++;

        if(tControl.cEst==EST_CALCULADORA)
        {
            if(tCal1.Op==0xFF)
            {
                tSys_flag.bAns=1;
            }
            tControl.cOp=OP_MULTIPLICACION;
            display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR,DISPLAY_OPERADOR);
            tDisplay.cPosCaracter++;
        }
    }

    //Se convierten los string a numero
    memset(cAux,0,sizeof(cAux));
    memcpy(cAux, tIngresoMonto.sEnteros, strlen((char*)tIngresoMonto.sEnteros));
    tIngresoMonto.wEnteros=atoll((char*)cAux);
    memset(cAux,0,sizeof(cAux));
    memcpy(cAux, tIngresoMonto.sDecimales, strlen((char*)tIngresoMonto.sDecimales));
    tIngresoMonto.wDecimal=atoll((char*)cAux);

    //Se verifica que el monto ingresado sea permitido
		
    cResp = func_check_limite();

    for(i=0; i<(tIngresoMonto.cMaxDecimales-tIngresoMonto.cContadorDecimal); i++)
    {
        tIngresoMonto.wDecimal*=10LL;
    }

//    if(cResp==FALSE)func_mensaje_error_ingreso(FALSE);
    return cResp;
}

/**
Borra el caracter anterior
@poslcd Posición en el display
@iVector2ini posición inicial desde donde se quiere copiar a fuente
*/
uint8_t func_plu_borrar(uint16_t poslcd)
{
    uint8_t cResp=TRUE;

    if(tDisplay.cPosCaracter==0)return FALSE;

    //Si se esta ingresando la parte entera
    if((tSys_flag.bDecimal==0)&&(tIngresoMonto.cContadorEnteros>0))
    {
        tIngresoMonto.cContadorEnteros--;
        tIngresoMonto.sEnteros[tIngresoMonto.cContadorEnteros]=0;
        tSys_flag.bDeccomp=0;
    }
    else if((tSys_flag.bDecimal==1)&&(tIngresoMonto.cContadorDecimal>0))
    {
        tIngresoMonto.cContadorDecimal--;
        tIngresoMonto.sDecimales[tIngresoMonto.cContadorDecimal]=0;
        tSys_flag.bDeccomp=0;
    }
    else if((tSys_flag.bDecimal==1)&&(tIngresoMonto.cContadorDecimal==0))
    {
        tSys_flag.bDecimal=0;
        tSys_flag.bDeccomp=0;
        if(tIngresoMonto.cContadorEnteros>=tIngresoMonto.cMaxEnteros)tSys_flag.bDeccomp=1;
    }
    else
    {
        cResp=FALSE;
    }
    poslcd--;
    if(cResp==TRUE)display_print_un_caracter(0x20,poslcd,0,DISPLAY_OPERADOR);

    return cResp;
}

/**
Define el maximo de digitos permitidos segun la funcion
*/
 void func_plu_limite(void)
{
        switch(tControl.cOp)
        {
            case OP_NO:
            case OP_INGRESO:
								//wplanchez
                //Por defecto se permite el maximo
                tIngresoMonto.cMaxEnteros = 10;
                tIngresoMonto.cMaxDecimales = 3;
            break;
            case OP_CANTIDAD:
						case OP_PROG_PLU_EXI:
						case OP_PROG_EXI:
                //Maximos para cantidades
                tIngresoMonto.cMaxEnteros = 5;
                tIngresoMonto.cMaxDecimales = 3;
            break;
            case OP_PRECIO:
            case OP_PROG_PRECIO:
                //Maximos para Precios
                tIngresoMonto.cMaxEnteros = 8;
                tIngresoMonto.cMaxDecimales = 2;
            break;
            case OP_PLU:
                //Maximos para PLU
                tIngresoMonto.cMaxEnteros = 12;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_MULTIPLICACION:
            case OP_SUMA:
            case OP_RESTA:
            case OP_DIVISION:					
						case OP_CALC:
                //Maximos para operaciones, enteros y decimales calculadora
                tIngresoMonto.cMaxEnteros = 8;
                tIngresoMonto.cMaxDecimales = 3;
            break;
            case OP_REC_POR:
            case OP_DESC_POR:
                //Maximos Para descuentos y RecargoS
                tIngresoMonto.cMaxEnteros = 2;
                tIngresoMonto.cMaxDecimales = 2;
            break;
            case OP_REC_MONTO:
            case OP_DESC_MONTO:
                tIngresoMonto.cMaxEnteros = 8;
                tIngresoMonto.cMaxDecimales = 2;
            break;
            case OP_MODO:
            case OP_PROG_FLAGS:
            case OP_PROG_MEDIOS:
            case OP_PROG_TECLA:
            case OP_PROG_PLU_TASAS:
            case OP_PROG_DEP:
                //Maximos para modos o tipos (medio de pago)
                tIngresoMonto.cMaxEnteros = 2;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_PROG_TECLA_TIPO:
            case OP_PROG_PLU:
            case OP_CLAVE_X:
            case OP_CLAVE_Z_EJ:
            case OP_CLAVE_S:
            case OP_CAJERO:
            case OP_PROG_CAJERO:
            case OP_PROG_CLAVE:
                //Maximos para modos o tipos (medio de pago)
                tIngresoMonto.cMaxEnteros = 4;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_MONTO:
                //Maximos para montos (pago parcial/fondo/retiro)
                tIngresoMonto.cMaxEnteros = 7;
                tIngresoMonto.cMaxDecimales = 2; //TODO
            break;
            case OP_RANGO_FECHA:
            case OP_PROG_FECHA:
            case OP_PROG_HORA:
                //Maximos para rangos de fecha
                tIngresoMonto.cMaxEnteros = 6;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_RANGO_NUMERO:
							  tIngresoMonto.cMaxEnteros = 7;
                tIngresoMonto.cMaxDecimales = 0;
								break;
						case OP_NUMERO_ACC_ELIMINAR:
							  tIngresoMonto.cMaxEnteros = 2;
                tIngresoMonto.cMaxDecimales = 0;
							break;
						case OP_NUMERO_PLU_ELIMINAR:
                //Maximos para rangos de fecha
                tIngresoMonto.cMaxEnteros = 4;
                tIngresoMonto.cMaxDecimales = 0;
							break;
            case OP_PROG_CONFIG :
                tIngresoMonto.cMaxEnteros = 5;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_PROG_MENSAJE:
                tIngresoMonto.cMaxEnteros = 0;
                tIngresoMonto.cMaxDecimales = 0;
            break;
            case OP_MENU:
            default:
                tIngresoMonto.cMaxEnteros = 0;
                tIngresoMonto.cMaxDecimales = 0;
            break;
        }
 }

/**
Chequea si el numero ingresado es permitido
*/
uint8_t func_check_limite(void)
{
    uint8_t cResp=TRUE;

    //Se verifica el maximo permitido
    func_plu_limite();
    //Se verifica si el ingreso esta en los limites
    if(tIngresoMonto.cContadorEnteros>tIngresoMonto.cMaxEnteros)cResp=FALSE;
    if(tIngresoMonto.cContadorDecimal>tIngresoMonto.cMaxDecimales)cResp=FALSE;

  return cResp;
}

//==============================Inicialización===============================================
/**
Inicialización de los buffers de datos
@note  Nota importante.
*/
void func_plu_ini(uint8_t bCleanPLU)
{

    tSys_flag.bDeccomp=0;
    tSys_flag.bDecimal=0;
    //Se limpian los controladores del monto actual y el plu actual
    memset((char*)tIngresoMonto.cValor,0,sizeof(tIngresoMonto));
    if(bCleanPLU==TRUE)
    {
        tControl.cOp = OP_NO;
        tSys_flag.bAnulacion=0;
        memset((char*)&tPluActual,0,sizeof(tPluActual));
    }
}


//==============================Descuento y Recargo===============================================
/**
Función encargada de armar el comando de descuento
@note  Nota importante.
*/
void func_plu_descuento(uint8_t bDescRec, uint8_t bPorMonto)
{
    uint8_t cPos=0;
	
    //Se procesa el monto ingresado
    func_plu_procesa_datos();

    //*****Se arma el comando****//

    //Comando de descuento por porcentaje
    if(bPorMonto==0)cBufGen[cPos++]='p';
    else cBufGen[cPos++]='q';

    //Indicador de descuento - recargo
    if(bDescRec==0)cBufGen[cPos++]='-';
    else    cBufGen[cPos++]='+';

    //Monto del descuento o recargo
    if(bPorMonto==0)
    {		
        
				sprintf((char*)cBufGen+cPos,"%02lld%02lld",tIngresoMonto.wEnteros,tIngresoMonto.wDecimal);
    }
    else
    {
        sprintf((char*)cBufGen+cPos,"%07lld%02lld",tIngresoMonto.wEnteros,tIngresoMonto.wDecimal);
    }

    //Se envia el comando si Enteros != 0 y Decimales != 0 el % minimo permitido es 0,01%
		if ((tIngresoMonto.wEnteros==0)&&(tIngresoMonto.wDecimal==0))
		{
			mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
			OSTimeDly(1500);
			//func_mensaje_error_ingreso(TRUE);
		}
    else 
		{		
			switch(tConfig.cFlag.bMensajeOk)
			{
				case 0:
					cmd_send((uint8_t*)cBufGen,strlen((char*)cBufGen), 1,2,SIN_MENSAJE );
					break;
				case 1 :
					cmd_send((uint8_t*)cBufGen,strlen((char*)cBufGen), 1,2,MENSAJE_CMD_OK);
					break;
			}

		}
    //Se limpian los indicadores
		tControl.cOp=OP_NO;
		tSys_flag.bPorMonto=0;
		tSys_flag.bTecNumerico = FALSE;
 
    display_ini();
    display_message_ini();   
}

uint8_t valida_plu(uint16_t iNumRegistro)
{
	uint8_t cResp=TRUE;
	tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
	cResp = MEM_leerPLU(pPluAux->cValor, iNumRegistro);
	
	return cResp;
	
}


//=============================PLU Directos================================================


/**
Función encargada de busqueda del plu directo presionado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/

uint8_t func_lista_plu(uint16_t iNumRegistro)
{
    uint8_t cResp=TRUE;
    uint8_t cAux[128]={NULL};

    tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
    tDEP_INFO *pDepAux;

    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//
		
		cResp = MEM_leerPLU(pPluAux->cValor, iNumRegistro);
		
    tPluActual.cDescripcion[0]=0;
    pPluAux->cDescripcion[40]=0;
		
    if(tConfig.cFlag.bCodigoPLU==1)
    {
        sprintf((char*)tPluActual.cDescripcion,"|%s|", (char*)pPluAux->cCodigo);
    }
    strcat((char*)tPluActual.cDescripcion,(char*)pPluAux->cDescripcion);

    //**************SE VERIFICAN LOS DATOS*************//

    //Si no se ingreso precio se coloca el precio por defecto
    if(tControl.cOp!=OP_PRECIO && tPluActual.wPrecio==0)tPluActual.wPrecio=pPluAux->wPrecio;
    //Si se ingreso la cantidad
    if(tControl.cOp!=OP_CANTIDAD && tPluActual.wCantidad==0)tPluActual.wCantidad = 1000LL;
    //Se verifica que la cantidad sea mayor a 0
    if(tPluActual.wCantidad==0)cResp=FALSE;

    //*********SE EXTRAE LA TASA DEL DEPARTAMENTO*********// /* TODO */
    pDepAux = (tDEP_INFO *)&cAux;
    cResp &= mem_read_write(MEM_DEPARTAMENTO,pDepAux->cValor,pPluAux->cDepAsociado,MODO_LECTURA);
    tPluActual.cTasa=pDepAux->cTasa;
    return cResp;
}
/**
Función encargada de busqueda del departamento directo presionado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
uint8_t func_lista_departamento(uint16_t iNumRegistro)
{
    uint8_t cResp=TRUE;
    tDEP_INFO tDepAux;

//    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//
	
		cResp = MEM_leerDepartamento(tDepAux.cValor,iNumRegistro);
    tPluActual.cDescripcion[0]=0;
    tDepAux.cDescripcion[39]=0;
    strcat((char*)tPluActual.cDescripcion,(char*)tDepAux.cDescripcion);
    tPluActual.cTasa=tDepAux.cTasa;
    if(tDepAux.cTasa>MAX_TASAS)cResp=FALSE;

    //**************SE VERIFICAN LOS DATOS*************//

    //Si no se ingreso precio se coloca el precio por defecto
    if(tControl.cOp!=OP_PRECIO && tPluActual.wPrecio==0)tPluActual.wPrecio=tDepAux.wPrecioDepartamento;
    //Si se ingreso la cantidad
    if(tControl.cOp!=OP_CANTIDAD && tPluActual.wCantidad==0)tPluActual.wCantidad = 1000LL;
    //Se verifica que la cantidad sea mayor a 0
    if(tPluActual.wCantidad==0)cResp=FALSE;
    //Se verifica que el precio no sobrepase el maximo del departamento
    if(tPluActual.wPrecio>tDepAux.wPrecioLimite)cResp=FALSE;

    return cResp;
}

/**
Funcion que registra un plu de la tabla a partir del numero ingresado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
uint8_t func_comando_plu(void)
{
    uint8_t cResp = FALSE, cAux[128] = {NULL};
    size_t poscomand = 0;
		
		uint8_t *pbuffer;
		uint8_t *pbuffer2;
		
		tPLU_VENTA *pPluVenta = (tPLU_VENTA *)pbuffer;
		tPLU_CONTADOR *pContador = (tPLU_CONTADOR*)pbuffer2;
		
    //**************SE ARMA LA ESTRUCTURA DEL COMANDO*************//

    //Se limpia el buffer
    memset(cBufGen, 0, sizeof(cBufGen));
    //Indicador de nota de credito/debito (1)
    if(tControl.cEst == EST_NOTA_DEBITO)
    {
        //Comando(1)
        cBufGen[poscomand] = '`';
        if(tSys_flag.bAnulacion == 1) cBufGen[poscomand] |= 0x80;
        poscomand++;
        //Tasa de Impuesto (1)
        sprintf((char *)(cBufGen + poscomand), "%d", tPluActual.cTasa);
        poscomand++;
    }
    else if (tControl.cEst == EST_NOTA_CREDITO)
    {
        //Comando(1)
        cBufGen[poscomand] = 'd';
        if(tSys_flag.bAnulacion == 1) cBufGen[poscomand] |= 0x80;
        poscomand++;
        //Tasa de Impuesto (1)
        sprintf((char *)(cBufGen + poscomand), "%d", tPluActual.cTasa);
        poscomand++;
    }
    else
    {
        //Comando - Tasa de Impuesto (1)
        cBufGen[poscomand] = tTasas[tPluActual.cTasa];
        if(tSys_flag.bAnulacion == 1) cBufGen[poscomand] |= 0x80;
        poscomand++;
    }
		
    //Precio (10)
    sprintf((char *)cAux, "%010lld", tPluActual.wPrecio);
    strcat((char *)cBufGen, (char *)cAux);
    //Cantidad (8)
    sprintf((char *)cAux, "%08lld", tPluActual.wCantidad);
    strcat((char *)cBufGen, (char *)cAux);
    //Descripcion Variable
    strcat((char*)cBufGen, (char*)tPluActual.cDescripcion);
		
		lCant=(uint32_t) tPluActual.wCantidad;
		
		if(tSys_flag.bAnulacion == FALSE){
			sprintf((char *)cAux, " %llu,%03llu x %llu,%02llu", tPluActual.wCantidad / 1000llu, tPluActual.wCantidad % 1000llu,tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
			Display_rotateWrite((char *)cAux, LINEA_SUPERIOR);

		}
		else{
			sprintf((char *)cAux, " Anulado: %llu,%03llu x %llu,%02llu", tPluActual.wCantidad / 1000llu, tPluActual.wCantidad % 1000llu,tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
			Display_rotateWrite((char *)cAux, LINEA_SUPERIOR);
		}		
    sprintf((char *)cAux, " %s",tPluActual.cDescripcion);
    Display_rotateWrite((char *)cAux, LINEA_INFERIOR);
		
    //Se envia el comando
    switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				cResp = cmd_send((uint8_t*)cBufGen, strlen((char *)cBufGen), 1, 3, SIN_MENSAJE);
				break;
			case 1 :
				cResp = cmd_send((uint8_t*)cBufGen, strlen((char *)cBufGen), 1, 3, MENSAJE_CMD_OK);
				break;
		}
		
    if (cResp == TRUE)
    {	
        Display_rotateSet();
    }
    else
    {
			
        if (func_verifica_operacion_fiscal() != FALSE)
        {
            mensaje_display(MENSAJE_ABIERTO, DISPLAY_AMBOS);
        }
        else
        {

						mensaje_display(MENSAJE_ERROR_COMANDO, DISPLAY_OPERADOR);
        }
        OSTimeDly(500);
        display_message_ini();
    }
    return cResp;
}
/**
Función encargada de busqueda del plu directo presionado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
void func_plu_directo(uint8_t iPluDirecto)
{
	
    uint8_t cResp=TRUE;
    tDIRECTO_INFO *tAccDirAux = (tDIRECTO_INFO *)cBufAux;
    //No se permite esta opcion en medio de un ingreso o en un menu
    if(tControl.cOp==OP_MENU)return;

    //Se valida que este en una operacion permitida
    if(!((tControl.cOp==OP_NO)||(tControl.cOp>=OP_INGRESO && tControl.cOp<=OP_PRECIO)))return;

    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//
	
		cResp = MEM_leerAccesoDirecto(tAccDirAux->cValor,iPluDirecto);
	
	 if(cResp==TRUE){
    //Se verifica si es un plu o un departamento
    if(tAccDirAux->cTipoTecla==0) //PLU
    {
				iContador=tAccDirAux->wNumero_Asociado;
				func_venta_plu(2);
				return;
    }
    else //DEPARTAMENTO
    {
        cResp &= func_lista_departamento(tAccDirAux->wNumero_Asociado);
    }

    //**************SE VERIFICAN LOS DATOS*************//
	}
    else
    {
        func_mensaje_error_ingreso(FALSE);
        return;
    }

    //**************SE ARMA LA ESTRUCTURA DEL COMANDO*************//
    cResp=func_comando_plu();

    if(cResp==TRUE)
    {
        //Si no hay un estado abierto se abre la factura
        if(tControl.cEst==EST_ESPERA) tControl.cEst=EST_FACTURA;
    }
}

void func_venta_plu(uint8_t par){
				
		/* Se obtiene el indice del PLU */	
		
		display_erase_line(AMBAS_LINEAS,DISPLAY_CLIENTE);
		display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);

    //Se procesa la informacion del plu
   if(par==1){
		func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
		
    iContador = tIngresoMonto.wEnteros;
	 }
	 
		int32_t lStockA;
		MEM_leerInventario(&lStockA, iContador);
	
	 if(valida_plu(iContador)==TRUE)
	 {
				if(lStockA<=0){
			
					if(tConfig.cFlag.bConfigVenta==0)
							func_plu();
					if(tConfig.cFlag.bConfigVenta==1)
					{
						mensaje_display(MENSAJE_STOCK_CERO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
						display_message_ini();
					}
					if(tConfig.cFlag.bConfigVenta==2)
					{
						mensaje_display(MENSAJE_STOCK_CERO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
				tSys_flag.bMenuOtrosActivo=1;
        menu_mostrar(tVentaN, 1,LINEA_SUPERIOR);
					}
				}
			else{
				func_plu();
			}
		}
	 else{
						mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
						display_message_ini();
						func_salir(FALSE);
	 }
		
}
/**
Funcion que registra un plu de la tabla a partir del numero ingresado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
void func_plu(void)
{
	
    uint8_t cResp = FALSE;
		tAns.bTipo = 0;
	
    //Se valida si en la operacion actual se puede procesar la tecla numerica
		if(tControl.cEst==EST_CALCULADORA)
		{
			return;
		}
		
    if(tControl.cOp == OP_MENU)
    {
        return;
    }
		
    //Se actualiza la informacion del plu actual

			tPluActual.iIndex = tIngresoMonto.wEnteros;

    //Se limpia el buffer de montos
    func_plu_ini(FALSE);
    func_clean();
    tControl.cOp = OP_PLU;
		
    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//
				
			if(tPluActual.iIndex <= MAX_PLU) cResp = func_lista_plu(iContador);

				//**************SE VERIFICAN LOS DATOS*************//
				if(cResp == FALSE)
				{
						func_mensaje_error_ingreso(FALSE);
						return;
				}

				//**************SE ARMA LA ESTRUCTURA DEL COMANDO*************//
				cResp = func_comando_plu();

				if(cResp==TRUE){
					int32_t lStockA;
					MEM_leerInventario(&lStockA, iContador);
//					UART_sendf("Stock= %lu",lStockA);

					lStockA=lStockA-lCant;
					MEM_escribirInventario(lStockA, iContador);
				}
				
				if((cResp == TRUE) && (tControl.cEst < EST_FACTURA))
				{
						tControl.cEst = EST_FACTURA;
				}
				tControl.cOp = OP_NO;
	
}

void func_plu2(uint16_t index)
{
    uint8_t cResp = TRUE;
	
		tControl.cOp = OP_MENU2;
		tControl.cEst = EST_FACTURA;

    //Se valida si en la operacion actual se puede procesar la tecla numerica
		if(tControl.cEst==EST_CALCULADORA)
		{
			return;
		}
		
    if(tControl.cOp == OP_MENU)
    {
        return;
    }
		
		display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
    //Se actualiza la informacion del plu actual
    //Se limpia el buffer de montos
    func_plu_ini(FALSE);
    func_clean();
    tControl.cOp = OP_PLU;
		
    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//

					
						if(tPluActual.iIndex <= MAX_PLU) cResp = func_lista_plu(index);

				//**************SE VERIFICAN LOS DATOS*************//
				if(cResp == FALSE)
				{
						func_mensaje_error_ingreso(FALSE);
						return;
				}

				//**************SE ARMA LA ESTRUCTURA DEL COMANDO*************//
				
				tControl.cEst = EST_FACTURA;

				cResp = func_comando_plu();

				if((cResp == TRUE) && (tControl.cEst < EST_FACTURA))
				{
						tControl.cEst = EST_FACTURA;
				}
				tControl.cOp = OP_NO;
	
}

/**
Funcion que registra un departamento de la tabla a partir del numero ingresado
@
@
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
void func_departamento(void)
{
    uint8_t cResp=FALSE;
		tAns.bTipo = 1;
	
		if(tControl.cEst==EST_CALCULADORA)
		{
			return;
		}
		
    //Se valida si en la operacion actual se puede procesar la tecla numerica
    if(tControl.cOp==OP_MENU)
    {
        return;
    }
		
		display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
		
    //Se procesa la informacion del plu
    func_plu_procesa_datos();
		
		tPluActual.iIndex = tIngresoMonto.wEnteros;		
    //Se actualiza la informacion del plu actual
    //Se limpia el buffer de montos
    func_plu_ini(FALSE);
    func_clean();
    tControl.cOp = OP_PLU;

    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//	

		if(tPluActual.iIndex<=MAX_DEP)cResp = func_lista_departamento(tPluActual.iIndex);
   
    //**************SE VERIFICAN LOS DATOS*************//

    if(cResp==FALSE)
    {
        func_mensaje_error_ingreso(FALSE);
        return;
    }

    //**************SE ARMA LA ESTRUCTURA DEL COMANDO*************//
    cResp=func_comando_plu();

    if ((cResp == TRUE) && (tControl.cEst < EST_FACTURA))
    {
        tControl.cEst=EST_FACTURA;
    }
    tControl.cOp=OP_NO;
}

/**
Funcion que registra un plu de la tabla a partir del numero ingresado
@fila fila de a tecla de plu directo
@columna columna de la tecla del plu directo
@funcion funcion primaria o secundaria
@note  Nota importante.
*/
void func_plu_consulta(void)
{
    uint8_t cResp=FALSE, cAux[17]={NULL};
    tPLU_INFO tPluAux;

    //Se valida si en la operacion actual se puede procesar la tecla numerica
    if(tControl.cOp==OP_MENU)
    {
        return;
    }

    //Se procesa la informacion del plu
    func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
    tPluActual.iIndex = tIngresoMonto.wEnteros;
    tControl.cOp = OP_PLU;

    //*********SE LEE DE LA MEMORIA DE TRABAJO*********//

    if(tPluActual.iIndex<=MAX_PLU)
    {
        cResp = mem_read_write(MEM_PLU,tPluAux.cValor,tPluActual.iIndex,MODO_LECTURA);
    }

    //**************SE VERIFICAN LOS DATOS*************//

    if(cResp==FALSE)
    {			
        mensaje_display(MENSAJE_ERROR_INGRESO,DISPLAY_OPERADOR);
        func_plu_ini(TRUE);
        func_mensaje_error_ingreso(FALSE);
    }
    else
    {
        tSys_flag.bTerceralinea=0;
        display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);   
        
        sprintf((char*)cAux, "%lld,%02lld",tPluAux.wPrecio/100LL, tPluAux.wPrecio%100LL);
        //Se imprime la descripcion
        Display_rotateWrite((char *)tPluAux.cDescripcion, LINEA_SUPERIOR);
        //Se imprime el precio
        Display_rotateWrite((char *)cAux, LINEA_INFERIOR);
        Display_rotateSet();
 
        //Se limpia el buffer de montos
        tSys_flag.bErase = 0;
        func_plu_ini(TRUE);
        tControl.cOp=OP_INFO;
    }

    func_clean();
}

//}
//===================================Pago total, parcial==========================================
/**
Funcion de envio de un pago total con un medio especifico
@note  Nota importante.
*/
void func_pago_total(uint8_t cMedio)
{
     
    //Se permite en estado de transaccion
    if(tControl.cEst<EST_FACTURA || tControl.cEst>EST_NOTA_CREDITO)
    {
        if (func_verifica_operacion_fiscal() == TRUE)
        {
            mensaje_display(MENSAJE_ABIERTO, DISPLAY_AMBOS);
            OSTimeDly(1000);
            display_message_ini();
        }
        return;
    }

    //No debe existir una operacion en curso
    if((tControl.cOp!=OP_NO)&&(tControl.cOp!=OP_MODO))return ;

    cBufGen[0]='1';

    if(cMedio>99) return;
    sprintf((char*)cBufGen+1,"%02d",cMedio);

			switch(tConfig.cFlag.bMensajeOk)
			{
				case 0:
					if(cmd_send((uint8_t*)cBufGen, 3, 1, 1, SIN_MENSAJE)==TRUE)
					{
						display_message_ini();
						func_salir(TRUE);
						tSys_flag.bTerceralinea=1;
					}
					break;
				case 1 :
					if(cmd_send((uint8_t*)cBufGen, 3, 1, 1, MENSAJE_CMD_OK)==TRUE)
					{
						display_message_ini();
						func_salir(TRUE);
						tSys_flag.bTerceralinea=1;
					}
					break;
				}
}


/**
Función de envio del comando de pago parcial
@note  Nota importante.
*/
void  func_pago_parcial(void)
{
    uint8_t cPos=0;

    //Se envia el comando de pago
    cBufGen[cPos++]='2';
    //Medio de pago (2)
    if(tPluActual.cModo>99)return ;
    sprintf((char*)cBufGen+cPos,"%02d",tPluActual.cModo);
    cPos+=2;
    //Monto del pago (10)
    sprintf((char*)cBufGen+cPos,"%012lld",tPluActual.wMonto);
	
		switch(tConfig.cFlag.bMensajeOk){
				case 0:
					if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE)==TRUE)
					{
						display_message_ini();
						tControl.cEst=EST_ESPERA;
						tControl.cOp=OP_NO;
						tSys_flag.bMenuAtivo=0;
						tSys_flag.bTerceralinea=1;

						func_pago_directo0(); 
						func_salir(FALSE);
					}
					break;
				case 1 :
					if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, MENSAJE_CMD_OK)==TRUE)
					{
						display_message_ini();
						tControl.cEst=EST_ESPERA;
						tControl.cOp=OP_NO;
						tSys_flag.bMenuAtivo=0;
						tSys_flag.bTerceralinea=1;

						func_pago_directo0(); 
						func_salir(FALSE);
					}
					break;
			}

}

//================================Modo de pago=============================================
/**
Función encargada de recibir el modo de pago, en las notas de credito, debito, retiros, fondo y pagos parciales
@note  Nota importante.
*/
void plu_func_modo(void)
{
    //Se procesa la informacion del monto
    func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
    tPluActual.cModo = tIngresoMonto.wEnteros;

    //Se determina si es necesario pasar a operacion monto
    if((tControl.cEst!=EST_FONDO_RETIRO)&&(tSys_flag.bTotalParcial==0))
    {
        func_pago_total(tPluActual.cModo);
    }
    else
    {
        //Se inicializa la estructura de montos
        func_plu_ini(FALSE);
        func_clean();
        //Se muestra la etiqueta en la pantalla
        mensaje_display(MENSAJE_MONTO,DISPLAY_AMBOS);
        tDisplay.cPosCaracter=7;

        //Se actualiza el tipo de operacion en curso a ingreso de modo
        tControl.cOp=OP_MONTO;
//				func_salir(FALSE);
    }
}

#define TASA_MASK (int64_t)(1 << 15)

