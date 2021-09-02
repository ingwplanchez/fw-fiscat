#include "func.h"
#include "menu.h"
#include "display.h"
#include "cmd.h"
#include "func_plu.h"
#include "uart.h"
#include "mem.h"
#include "F25L016A.h"
#include <stdio.h>
#include <ctype.h>

//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//
#define AnchoDisplay ((uint8_t) 20)
const char Datos_Subttl[][7]={"SUBTTL","Bs"};

uint8_t cAux3[41], cTotalEnteros[30],cTotalDecimales[30];


//**********************DECLARACION DE VARIABLES LOCALES************************//



//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//


//******************************************************************************//
//                                                  DEFINICION DE FUNCIONES                             //
//******************************************************************************//
uint64_t func_ingreso_obtenerNumero(void);
//*****************************FUNCIONES DE VENTAS******************************//

//================================Nota de débito y crédito========================================
/**
Funciones para evento en la tecla nota de debito/credito
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_nota_deb_cred(uint8_t cOp)
{
		
    //No Se permite esta opcion en medio de un ingreso
    if((tControl.cOp==OP_MENU)||((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU)))return;

    //No se permite estando en otra transaccion
    if(tControl.cEst!=EST_ESPERA)return;

    Display_rotateUnSet();

    //Se inicia el proceso de registro de datos
    func_personalizar_factura();

    //Se cambia el estado
    tControl.cEst=cOp;
}

//=========================Anular PLU, Anular Factura, Corregir PLU, Personalizar factura================================
/**
Función de anulación del último PLU de la factura o nota de crédito/débito
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_anular_plu(void)
{
	
		tSys_flag.bShiftActivo=0;
    //Se permite en estado de transaccion
    if(tControl.cEst < EST_FACTURA) return;

    //Se valida la operacion en curso
    if(tControl.cOp>OP_PLU) return;

    //Se activa el indicador de anulacion
    tSys_flag.bAnulacion = TRUE;
	
	//////////////////////////////////
	
		Display_rotateUnSet();
		tControl.cOp=OP_CLAVE_S; 
		tSys_flag.bTecAlfabetico=FALSE;
		tSys_flag.bIngresoDoble=FALSE;
		func_clean();
		func_print_etiqueta(MENSAJE_CLAVE, 0, 6, LINEA_SUPERIOR, DISPLAY_OPERADOR); 
	
	//////////////////////////////////
}

/**
Anulación de factura
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_cmd_simple_fac(uint8_t cCmd)
{
			
		// Se ejecuta solo una vez al entrar y activar las banderas de anulacion
		if(tAns.bAnularFactura == 0 && tAns.bAnularPlu == 0 ) 
		{
			//Se permite en estado de transaccion
			if((tControl.cEst<EST_FACTURA || tControl.cEst>EST_NOTA_CREDITO ) && tControl.cEst!=EST_CALCULADORA )return;

			//No debe existir una operacion en curso
			if(tControl.cOp!=OP_NO && tControl.cEst!=EST_CALCULADORA)return;
			
			// cCmd = '7' Anula Factura ; cCmd = 'k' Anula PLU ; cCmd = '3' Muestra Sub total en la impresora
			if(cCmd == '7')
			{
				tAns.bAnularFactura = 1;
			}
			else if(cCmd == 'k'){
				
				tAns.bAnularPlu = 1;
			}
			tAns.bAnulacionActiva = 1;
		}
		
		if(tAns.bDone == 1 || cCmd == '3'){
				wAns = 0;
				//Se envia el comando de correccion
				cBufGen[0]=cCmd;
				switch(tConfig.cFlag.bMensajeOk)
				{
					case 0:
						if(cmd_send(cBufGen, 1, 1, 1, SIN_MENSAJE)==TRUE)
						{
							if(cCmd == '7')
							{
									tControl.cEst = EST_ESPERA;
									func_salir(FALSE);
									tControl.cOp=OP_NO;
							}
							
							else if(cCmd == 'k'){func_salir(FALSE);}
							
							else if(cCmd == '3')
							{
								if(func_verifica_operacion_fiscal() != FALSE)
								{
									Display_rotateUnSet();
									display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
									memset(cBufGen, 0x00, sizeof(cBufGen));
									memset(cBufAux, ' ', MAX_DISPLAY);
									sprintf((char *)(cBufGen), "%s %llu,%02llu",Datos_Subttl[0], wAns / 100llu, wAns % 100llu);
									sprintf((char *)(cBufAux)+(MAX_DISPLAY-strlen((char *) cBufGen)), "%s", cBufGen);
									display_print(cBufAux,LINEA_SUPERIOR,FONDO_NEGRO,DISPLAY_AMBOS);
								}
							}
						}
						break;
					
					case 1 :
						if(cmd_send(cBufGen, 1, 1, 1, MENSAJE_CMD_OK)==TRUE)
						{
							if(cCmd == '7')
							{
									tControl.cEst = EST_ESPERA;
									func_salir(FALSE);
									tControl.cOp=OP_NO;
							}
							else if(cCmd == 'k'){func_salir(FALSE);}
							
							else if(cCmd == '3')
							{
								if(func_verifica_operacion_fiscal() != FALSE)
								{
									Display_rotateUnSet();
									display_erase_line(AMBAS_LINEAS,DISPLAY_AMBOS);
									memset(cBufGen, 0x00, MAX_DISPLAY);
									memset(cBufAux, ' ', MAX_DISPLAY);
									sprintf((char *)(cBufGen), "%s %s %llu,%02llu",Datos_Subttl[0],Datos_Subttl[1], wAns / 100llu, wAns % 100llu);
									sprintf((char *)(cBufAux)+(MAX_DISPLAY-strlen((char *) cBufGen)), "%s", cBufGen);
									display_print(cBufAux,LINEA_SUPERIOR,FONDO_NEGRO,DISPLAY_AMBOS);
								}
								else 	display_message_ini();
							}
						}
						break;
				}
		}

}

/**
Personalización de factura
*/
void func_personalizar_factura(void)
{		

    //No Se permite esta opcion en medio de un ingreso
    if((tControl.cOp==OP_MENU)||((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU)))return;

    //Solo se permite en espera
    if(tControl.cEst!=EST_ESPERA)return;

    Display_rotateUnSet();

    //Se coloca el descriptor de ingreso
    func_print_etiqueta(MENSAJE_ID, 0, 0, LINEA_SUPERIOR, DISPLAY_AMBOS);
    func_plu_ini(TRUE);
    //Se cambia el estado
    tControl.cOp=OP_INGRESO_ID;
    //Se activa el teclado alfabetico
    tSys_flag.bTecAlfabetico=TRUE;
    tSys_flag.bIngresoDoble=FALSE;
}

/**
Personalización de factura
*/
void func_cliente(void)
{
    uint8_t cNuevoEstado = 0, cNuevoMensaje = 0;
	
    if (strlen((char *)tPluActual.cTexto) == 0)
    {
        return;
    }

    memset(cBufGen,0,sizeof(cBufGen));
    cBufGen[0]='i';

    switch(tControl.cOp)
    {
        case OP_INGRESO_ID:
					
					if (strlen((char *)tPluActual.cTexto) <= 0 || strlen((char *)tPluActual.cTexto) >10)
					{
						mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
      			OSTimeDly(1500);
						tControl.cOp = OP_INGRESO_ID;
						func_personalizar_factura();
						return;
					}
					else{
							cNuevoEstado = OP_INGRESO_NOMBRE;
							cNuevoMensaje = MENSAJE_NOMBRE;
							strcat((char *)cBufGen, "R*");
					}
            break;
        case OP_INGRESO_NOMBRE:
            cNuevoEstado = OP_INGRESO_DIRECCION;
            cNuevoMensaje = MENSAJE_DIRECCION;
            strcat((char *)cBufGen, "S*");
            break;
        case OP_INGRESO_DIRECCION:
            if((tControl.cEst == EST_NOTA_DEBITO) || (tControl.cEst == EST_NOTA_CREDITO))
            {
                cNuevoEstado=OP_INGRESO_NUMERO_FO;
                cNuevoMensaje=MENSAJE_FACTURA;
            }
            else
            {
                cNuevoEstado=OP_NO;
                cNuevoMensaje=MENSAJE_MARCA;
            }
            strcat((char *)cBufGen, "00Dirección: ");
            break;
        case OP_INGRESO_NUMERO_FO:
            cNuevoEstado=OP_INGRESO_FECHA_FO;
            cNuevoMensaje=MENSAJE_FECHA_FAC;
            strcat((char *)cBufGen, "F*");
            break;
        case OP_INGRESO_FECHA_FO:
            cNuevoEstado=OP_INGRESO_CONTROL_FO;
            cNuevoMensaje=MENSAJE_CONTROL_FAC;
            strcat((char *)cBufGen, "D*");
            break;
        case OP_INGRESO_CONTROL_FO:
            cNuevoEstado=OP_NO;
            cNuevoMensaje=MENSAJE_MARCA;
            strcat((char *)cBufGen, "I*");
            break;
        default:
            return;
    }

    strcat((char *)cBufGen, (char *)tPluActual.cTexto);
    if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE) == TRUE)
    {
        //Se activa el teclado alfabetico
        tSys_flag.bTecAlfabetico=TRUE;
        tSys_flag.bIngresoDoble=FALSE;
        func_plu_ini(TRUE);
        //Se coloca el descriptor de ingreso
        if(cNuevoMensaje==MENSAJE_MARCA)
        {
            func_clean();
            func_plu_ini(TRUE);
            tSys_flag.bTerceralinea=1;
            tSys_flag.bTecAlfabetico=FALSE;
            tSys_flag.bIngresoDoble=FALSE;
            tSys_flag.bErase = TRUE;
            tControl.cOp = OP_NO;
            display_message_ini();
        }
        else
        {
            //Se coloca el descriptor de ingreso
            func_print_etiqueta(cNuevoMensaje, 0, 0, LINEA_SUPERIOR, DISPLAY_AMBOS);
            tSys_flag.bTecAlfabetico=TRUE;  //IMPRESION
            tSys_flag.bIngresoDoble=FALSE;
        }
        tControl.cOp=cNuevoEstado;
    }
    else
    {
        func_salir(FALSE);
        tControl.cEst=EST_ESPERA;
    }				
}

/**
Función de evento (función primaria) en la tecla X
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_tecla_x(void)
{
		char *pbuffer;
		uint8_t cCantidadEntero;
		uint8_t cCantidadDecimal;
	
		//MODCALC
		if (tControl.cEst==EST_CALCULADORA)
		{		
			tCal2.Op = OP_MULTIPLICACION; 
			func_op_calc(tCal2.Op);
			func_print_etiqueta(MENSAJE_MULTIPLICACION, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
		}
		
    //ya se encuentra en este estado retorna
    if(tControl.cOp==OP_CANTIDAD)return;
		
    //Solo se permite estando en operacion de ingreso
    if(tControl.cOp!=OP_INGRESO)return;

        tSys_flag.bDecimal=0;
        tControl.cOp=OP_CANTIDAD;
		
        //Se procesa la informacion del plu
				func_plu_procesa_datos();
					
        //Se actualiza la informacion del plu actual
				cCantidadEntero = strlen((char*)tIngresoMonto.sEnteros);
				cCantidadDecimal = strlen((char*)tIngresoMonto.sDecimales);
		
        if(((tPluActual.wCantidad==0)&&(cCantidadEntero)<=5)||(tIngresoMonto.wEnteros !=0 && tIngresoMonto.wDecimal!=0))
        {
          tPluActual.wCantidad = tIngresoMonto.wEnteros*1000LL;
          tPluActual.wCantidad += tIngresoMonto.wDecimal;
					
					if (strlen((char *)tIngresoMonto.sEnteros) == 4) {}
						
					sprintf(pbuffer,"CANT:%llu,%02llu",tIngresoMonto.wEnteros*1llu, tIngresoMonto.wDecimal*1llu);
					display_print((uint8_t*) pbuffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
					
          func_plu_ini(FALSE);
        }
        else
        {
					func_mensaje_error_ingreso(FALSE);
        }
        func_clean();
}
/**
Función de evento (función primaria) en la tecla X
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_price(void)
{
		char *pbuffer;
    //Se ya se encuentra en este estado retorna
    if(tControl.cOp==OP_PRECIO)return;

    //Solo se permite estando en operacion de ingreso o en cantidad
    if(tControl.cOp!=OP_INGRESO)return;

    /* Si el precio es fijo, no se permite */
    if (tConfig.cFlag.bPrecioFijo == TRUE)
    {
        mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_AMBOS);
        OSTimeDly(1000);
        func_salir(FALSE);
        return;
    }

        tControl.cOp=OP_PRECIO;
        func_plu_procesa_datos();
        tSys_flag.bShiftActivo=0;
        //Se actualiza la informacion del plu actual
        if(tPluActual.wPrecio==0)
        {
						if(tSys_flag.bDecimal == 0)
						{
							tPluActual.wPrecio = tIngresoMonto.wEnteros/100llu;
							tPluActual.wPrecio += tIngresoMonto.wEnteros%100llu;
							
							memset(cAux3,0,sizeof(cAux3));
							memcpy(cAux3, tIngresoMonto.sEnteros, strlen((char*)tIngresoMonto.sEnteros));
							
							memset(pbuffer,0,sizeof(strlen((char*)tIngresoMonto.sEnteros)+3));
							sprintf(pbuffer,"PREC:%0llu,%01.2llu", atol((char*)cAux3)/100llu, atol((char*)cAux3)%100llu);
							display_print((uint8_t*) pbuffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
						
						else{
							tSys_flag.bDecimal=0;
							tPluActual.wPrecio = tIngresoMonto.wEnteros*100LL;
							tPluActual.wPrecio += tIngresoMonto.wDecimal;
					
							sprintf(pbuffer,"PREC:%llu,%02llu",tIngresoMonto.wEnteros*1llu, tIngresoMonto.wDecimal*1llu);
							display_print((uint8_t*) pbuffer, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
						
            func_plu_ini(FALSE);
        }
        else
        {
            func_mensaje_error_ingreso(FALSE);
        }

        func_clean();
}

//*****************************FUNCIONES DE PAGOS******************************//

/**
Evento en la tecla total/efectivo
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_pago_menu(void)
{
		if(tAns.bBorrarPlu  == 1 && tControl.cOp==OP_INGRESO){ //&& tControl.cOp==OP_INGRESO
			tControl.cOp = OP_NUMERO_PLU_ELIMINAR;
			func_plu_procesa_datos();
			tPluActual.iIndex = tIngresoMonto.wEnteros;

			func_elimina_plu();
			func_plu_ini(FALSE);
			return;
		}
		
///////////////////////////////////////////////////////////////////////////////////////
		
			else if(tAns.bBorrarACC == 1 && tControl.cOp==OP_INGRESO){ 
			tControl.cOp = OP_NUMERO_ACC_ELIMINAR;
			func_plu_procesa_datos();
			tPluActual.iIndex = --tIngresoMonto.wEnteros;
			func_elimina_acc();
			func_plu_ini(FALSE);
			return;
		}
		
///////////////////////////////////////////////////////////////////////////////////////		
		
		
    //No Se permite esta opcion en medio de un ingreso
    if(((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU)))return;

    if(tControl.cOp!=OP_NO || tControl.cEst==EST_FONDO_RETIRO)
    {
        func_ejecutar();
    }
    else
    {
        tSys_flag.bMenuOtrosActivo=1;
        menu_mostrar(tPagos, 1,LINEA_SUPERIOR);
    }
}

/**
Pago directo medio de pago 0
*/
void func_pago_directo0(void)
{
	
		if(tControl.cOp == OP_RANGO_NUMERO && (tControl.cEst == EST_REPORTES_PLU || tControl.cEst == EST_REPORTES_DEP))
		{
			func_plu_procesa_datos();
			iEnd = tIngresoMonto.wEnteros;
			
	    display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);	
			Display_rotateUnSet();
			
			if (iStart == 0 || iEnd == 0 || iStart > iEnd || iEnd>=3001) {
				display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);	
				func_mensaje_error_ingreso(FALSE);
				
				return;
      }
			
			else{
				if(tControl.cEst == EST_REPORTES_PLU)
				{			
					func_print_etiqueta(MENSAJE_IMP_REPORT_PLU, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
					tControl.cEst = EST_IMP_REPORT_PLU;
				}
				else if(tControl.cEst == EST_REPORTES_DEP)
				{				
					func_print_etiqueta(MENSAJE_IMP_REPORT_DEP, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);					
					tControl.cEst = EST_IMP_REPORT_DEP;
				}
				
				tControl.cOp  = OP_INICIO;
				
			}
		}
	
		if(tControl.cEst==EST_CALCULADORA && tFlag.bCalc==1 && tFlag.bCalc2==0 )
		{
			func_calculadora();
			tFlag.bCalc=0;
			return;
		}

		
		//MODCALC
			
		if((tControl.cEst==EST_CALCULADORA)&&(tCal2.Op>=OP_SUMA &&  tCal2.Op<=OP_DIVISION)){
			func_op_calculadora();
			tFlag.bCalc=1;
			return;
		}

    //No Se permite esta opcion en medio de un ingreso
    if(((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU)))return;
			
    //En operacion menu
    if(tControl.cOp!=OP_NO || tControl.cEst==EST_FONDO_RETIRO)
    {		
        func_ejecutar();
    }
		
    //Funcion por defecto
    else
    {
        func_pago_total(1);
    }
}

void func_pago_total_menu(void)
{
    uint8_t cMedio=1;
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
Mensajes de monto y modo, posicion en el display del cursor
@cTipo Pago total 0, y Parcial 1
*/
void func_pago_lista(uint8_t cTipo)
{
    //Se inicializa la estructura de montos
    func_plu_ini(TRUE);
    func_clean();
    //Se muestra la etiqueta en la pantalla
    mensaje_display(MENSAJE_TIPO,DISPLAY_AMBOS);
    tDisplay.cPosCaracter=6;
    //Se actualiza el tipo de operacion en curso a ingreso de modo
    tControl.cOp=OP_MODO;
    if(cTipo==MENU_PAGO_TOTAL)tSys_flag.bTotalParcial=0;    //Indicador de pago total
    else tSys_flag.bTotalParcial=1; //Indicador de pago parcial
		//func_salir(FALSE);
}

//*****************************FUNCIONES DE MENU******************************//

/**
Evento en la tecla C
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_ejecutar(void)
{
	
	//No Se permite esta opcion en medio de un ingreso
    if((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU))return;
	
    /* Si hay se esta en modo alfabetico y hay una tecla procesada */
    if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cChHex!=0)
    {
			tPluActual.cTexto[tPluActual.cContadorCh]=tPluActual.cChHex;
			tPluActual.cContadorCh++;
			tPluActual.cChHex=0;
			tPluActual.cTeclaBase=0;
			tDisplay.cPosCaracter++;
    }

    switch(tControl.cOp)
    {
		case OP_NO:
            func_fin_fondo_retiro();
        break;
        //En Operacion de Menu
        case OP_MENU:
            menu_ejecutar_opcion();
        break;
			 case OP_MENU2:
            menu_ejecutar_opcion2();
						tControl.cEst = EST_FACTURA;
			 break;
        //En Operacion de Menu
        case OP_DESC_POR:
            func_plu_descuento(0,0);
        break;
        case OP_REC_POR:
            func_plu_descuento(1,0);
        break;
        case OP_DESC_MONTO:
            func_plu_descuento(0,1);
        break;
        case OP_REC_MONTO:
            func_plu_descuento(1,1);
        break;
        case OP_MODO:
            plu_func_modo();
        break;
        case OP_MONTO:
            func_monto();
        break;
        case OP_RANGO_FECHA:
        case OP_RANGO_NUMERO:
            if(tControl.cEst==EST_REPORTES_MF)func_menu_historicos();
						else if(tControl.cEst== EST_REPORTES_PLU){Reporte_PLU();}
            else func_menu_reimpresiones();
        break;

				case OP_RANGO_PLU_NUMERO:
					func_eliminar_plu();
					break;
				case OP_RANGO_ACC_NUMERO:
					func_eliminar_acc();
					break;
        case OP_INGRESO_ID:
        case OP_INGRESO_NOMBRE:
        case OP_INGRESO_DIRECCION:
        case OP_INGRESO_NUMERO_FO:
        case OP_INGRESO_FECHA_FO:
        case OP_INGRESO_CONTROL_FO:
            func_cliente();
            break;
        case OP_PROG_FECHA:
            func_prog_fecha(0);
        break;
				case OP_PROG_EXI:
					func_procesar_stock();
					break;
        case OP_PROG_HORA:
            func_prog_fecha(1);
        break;
        case OP_PROG_FLAGS:
            func_prog_flag();
        break;
        case OP_PROG_MEDIOS:
            func_prog_medio();
        break;
        case OP_PROG_TECLA:
					  func_prog_acceso_directo();
            break;
        case OP_PROG_TECLA_TIPO:
            func_prog_acceso_directo();
						func_salir(FALSE);//func_salir(FALSE);func_salir(FALSE);
            break;
        case OP_PROG_PLU:
				    func_prog_plu();
            break;
        case OP_PROG_DEP:
            func_prog_dep();
            break;
        case OP_PROG_PRECIO:
				case OP_PROG_PLU_EXI:
            if(tControl.cEst==EST_PROGRAMACION_PLU)func_prog_plu();
            else func_prog_dep();
            break;
        case OP_PROG_PLU_TASAS:
            if(tControl.cEst==EST_PROGRAMACION_PLU)func_prog_plu();
            else func_prog_dep();
            break;
        case OP_CLAVE_X:
        case OP_CLAVE_Z_EJ:
        case OP_CLAVE_S:
						// Verifica si se va a realizar una anulacion o habilitacion de menu
						if(tSys_flag.bAnulacion || tAns.bAnularFactura || tAns.bAnularPlu)
						{
							func_verifica_anulacion();
						}
						else{
							func_habilitar_menu();
						}
            break;
        case OP_PROG_CAJERO:
        case OP_PROG_CAJERO_NOMBRE:
            func_prog_cajero();
            break;
        case OP_CAJERO:
            func_activar_cajero();
            break;
        case OP_PROG_CLAVE:
            func_prog_clave();
        break;
        case OP_PROG_CONFIG:
            func_prog_config();
            break;
        case OP_PROG_MENSAJE:
            func_prog_mensaje();
            break;
        default:
            break;
    }
    return;
}

/**
Menu
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_menu(void)
{
    uint8_t i;
	
    //No se permite esta opcion en medio de un ingreso
    if((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU))
		{
			return;
		}

    /* No se permite en medio de una venta */
    if ((tControl.cEst >= EST_FACTURA) && (tControl.cEst <= EST_NOTA_CREDITO))
    {
        return;
    }

		if( (tControl.cOp >= OP_PROG_FECHA) && (tControl.cOp <=OP_MENU))
		{
			return;
		}
		
    //Colocar chequeo de estatus de la impresora para ver si esta libre
    for(i=0;i<2;i++)
    {
        //Se cambia al modo de operacion a OP_MENU
        tControl.cOp=OP_MENU;

        //Se abre el menu del operador
        menu_principal();
    }
}

/**
Funcion de la tecla arriba
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_teclas_direc_up(void)
{
    //Funcionamiento solo con el menu activo
		
		if(tControl.cOp!=OP_MENU && tControl.cOp!=OP_MENU2)
    {
			if(tSys_flag.bTecAlfabetico==0)
			{
        if(func_plu_borrar(tDisplay.cPosCaracter)==TRUE)
        {
            tDisplay.cPosCaracter--;
        }
			}
		}
		
    if(tControl.cOp==OP_MENU)
    {
        LineaElemSeleccion=0;
        menu_cambio_seleccion(ElementoSeleccionActual-1);
    }
		
		/* Funcionamiento solo con el menu activo de PLUS */
    if (tControl.cOp == OP_MENU2) {
			
        LineaElemSeleccion = 0;
        menu_cambio_seleccion2(ElementoSeleccionActual - 1);
				
    }
		
    //Opciones en modo de ingreso doble
    else if(tControl.cOp==OP_RANGO_FECHA)
    {
//        func_rango_fecha();
    }
    else if(tControl.cOp==OP_RANGO_NUMERO)
    {
//        func_rango_numero();
    }
    else if(tControl.cOp==OP_PROG_FLAGS)
    {
//        func_programacion(PROGRAMACION_FLAGS);
    }
    else if(tControl.cOp==OP_PROG_MEDIOS)
    {
//        func_programacion(PROGRAMACION_MEDIOS);
    }

    /* Si  se esta en modo alfabetico */
    if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cContadorCh > 0)
    {
        display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
        tPluActual.cTexto[tPluActual.cContadorCh]=0;
        tPluActual.cContadorCh--;
        tPluActual.cChHex=0;
        tPluActual.cTeclaBase=0;
        tDisplay.cPosCaracter--;
			
    }
    else if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cContadorCh == 0)
    {
        display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
        tPluActual.cTexto[tPluActual.cContadorCh]=0;
        tPluActual.cChHex=0;
        tPluActual.cTeclaBase=0;
    }			
}

/**
Descripcion de la Funcion
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_teclas_direc_down(void)
{
	uint8_t cDesde;
	uint8_t cHasta;
	
	if(tControl.cOp == OP_RANGO_NUMERO && (tControl.cEst == EST_REPORTES_PLU || tControl.cEst == EST_REPORTES_DEP))
	{
		func_plu_procesa_datos();
		iStart = tIngresoMonto.wEnteros;
		
	}
	
    //Funcionamiento con el menu activo
    if(tControl.cOp==OP_MENU)
    {
        LineaElemSeleccion=1;
        menu_cambio_seleccion(ElementoSeleccionActual+1);
    }
		
		/* Funcionamiento solo con el menu activo de PLUS */
    if (tControl.cOp == OP_MENU2) {
        LineaElemSeleccion = 1;
				
        menu_cambio_seleccion2(ElementoSeleccionActual + 1);
   }
		
    //Funciones de ingreso doble, se pasa a la segunda linea de ingreso
    else if((tSys_flag.bTecAlfabetico==FALSE)&&(tSys_flag.bIngresoDoble==TRUE))
    {
        //Se procesa la informacion del monto ingresado en el primer nivel
        func_plu_procesa_datos();
        //Debe existir al menos un valor ingresado
        if(tIngresoMonto.cContadorEnteros==0) return;
        //El segundo nivel es alfabetico
        if((tControl.cOp==OP_PROG_MEDIOS)||(tControl.cOp==OP_PROG_PLU)
            ||(tControl.cOp==OP_PROG_DEP || (tControl.cOp==OP_PROG_PLU_EXI)))
        {
            //Se actualiza la informacion del plu actual
            tPluActual.wIndex = tIngresoMonto.wEnteros;
						if(tControl.cOp==OP_PROG_PLU_EXI)
            {
                tPluActual.wInicio = tIngresoMonto.wEnteros*1000LL;
                tPluActual.wInicio += tIngresoMonto.wDecimal;
            }
            func_clean();
            func_plu_ini(FALSE);
            if (tControl.cOp==OP_PROG_MEDIOS)tDisplay.cPosCaracter=22;
						else if(tControl.cOp==OP_PROG_PLU_EXI)tDisplay.cPosCaracter=20;
						else tDisplay.cPosCaracter=20;
            tSys_flag.bTecAlfabetico=TRUE;
        }
		
        //El segundo nivel es numerico
        //Si el ingreso es de fecha deben ingresarse todos los digitos
        else if((tControl.cOp!=OP_RANGO_FECHA) || ((tControl.cOp==OP_RANGO_FECHA)
            && (tIngresoMonto.cContadorEnteros==tIngresoMonto.cMaxEnteros)))
        {
            //Se actualiza la informacion del plu actual
            tPluActual.wInicio = tIngresoMonto.wEnteros;
            //Dependiendo del estado el monto tiene parte decimal
            if(tControl.cOp==OP_PROG_PRECIO)
            {
                tPluActual.wInicio = tIngresoMonto.wEnteros*100LL;
                tPluActual.wInicio += tIngresoMonto.wDecimal;
            }
						
            func_clean();
            func_plu_ini(FALSE);
            if(tControl.cOp==OP_PROG_FLAGS)tDisplay.cPosCaracter=22;
            else if(tControl.cOp==OP_PROG_PRECIO)tDisplay.cPosCaracter=21;
						else if(tControl.cOp==OP_PROG_EXI)tDisplay.cPosCaracter=22;
            else tDisplay.cPosCaracter=22;
        }
    }
    //Ingresar letra en factura personalizada
    else if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cContadorCh<=25)
    {
        //Si hay una letra procesada
        if(tPluActual.cChHex!=0)
        {
            tPluActual.cTexto[tPluActual.cContadorCh]=tPluActual.cChHex;
            tPluActual.cContadorCh++;
            tPluActual.cChHex=0;
            tPluActual.cTeclaBase=0;
            tDisplay.cPosCaracter++;
            display_print_un_caracter('_', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);				
        }
    }

    if (((tControl.cOp >= OP_RANGO_NUMERO) && (tControl.cOp <= OP_PROG_CLAVE)) || tControl.cOp==OP_PROG_PLU_EXI || tControl.cOp==OP_PROG_EXI)
    {
        display_print_un_caracter('_', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
    }
		
}

//*****************************FUNCIONES AUXIIARES******************************//

//==========================Limpieza de variables===============================//
void func_clean(void)
{
    tDisplay.cLinea=0;
    tDisplay.cPosCaracter=0;
    tSys_flag.bTecAlfabetico=0;
}

void func_void(void)
{
    //Funcion vacia
}

void func_monto(void)
{
    //Se procesa la informacion del monto
    func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
    tPluActual.wMonto = tIngresoMonto.wEnteros*100LL;
    tPluActual.wMonto += tIngresoMonto.wDecimal;

    //Si se encuentra en un fondo retiro
    if(tControl.cEst==EST_FONDO_RETIRO)
    {
        if(tSys_flag.bFondoRetiro == 0) /* Envio de fondo */
        {
            func_fondo_retiro(0);
        }
        else /* Envio de Retiro */
        {
            func_fondo_retiro(1);
        }
    }
    //Si se encuentra en un pago
    else
    {	
        func_pago_parcial();
    }
}

/**
Función encargada de armar el comando de retiro/fondo
@note  Nota importante.
*/
void func_fondo_retiro(uint8_t cTipo)
{
    uint8_t cPos = 0;

    //Se envia el comando de fondo o retiro
    if (cTipo == 0)
    {
        sprintf((char *)cBufGen, "%s", "91");
    }
    else
    {
        sprintf((char *)cBufGen, "%s", "90");
    }
    cPos = strlen((char *)cBufGen);
    //Medio de pago (2)
    if(tPluActual.cModo>99)return ;
    sprintf((char*)cBufGen + cPos, "%02d", tPluActual.cModo);
    cPos += 2;
    //Monto del pago (10 + 2)
    sprintf((char*)cBufGen + cPos, "%012lld", tPluActual.wMonto);
		
		switch(tConfig.cFlag.bMensajeOk)
		{
		case 0:
			if (cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE) == TRUE)
			{
					func_fin_fondo_retiro();
			}
			break;
		case 1 :
			if (cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, MENSAJE_CMD_OK) == TRUE)
			{
					func_fin_fondo_retiro();
			}
			break;
		}

 }
//*****************************FUNCIONES TECLADO******************************//

//=================================Gaveta, Tecla X, SHIFT, Borrar, Hora y Fecha, Salir de la opercaión, Modo Entrenamiento============================================

/**
Funcion para la apertura de gaveta
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_apertura_gaveta(void)
{
    //Se permite en estado de transaccion
    if(tControl.cEst!=EST_ESPERA)return;

    //No debe existir una operacion en curso
    if(tControl.cOp!=OP_NO)return;

    /* Se comando no venta */
    cBufGen[0]= '0';
		
		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				cmd_send(cBufGen, 1, 1, 1, SIN_MENSAJE);
				break;
			case 1 :
				cmd_send(cBufGen, 1, 1, 1, MENSAJE_CMD_OK);
				break;
		}

    func_salir(TRUE);
}


/**
Cambia el valor de la bandera de estado de la tecla SHIFT
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_tecla_shift(void)
	
{	    
    if (tControl.cOp == OP_NO) // || (tControl.cEst == EST_ESPERA))
    {
				if(tControl.cEst==EST_CALCULADORA){}
				else{	
					display_ini();
					display_message_ini();
				}
    }
		
    if(tSys_flag.bShiftActivo==0)
    {
        tSys_flag.bShiftActivo=1;
    }
    else
    {
        tSys_flag.bShiftActivo=0;
    }

}

/**
Funciones del evento de la tecla borrar
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_tecla_borrar(void)
{
//  En modo menu no se procesa
//	
		if(tControl.cOp == OP_MENU){return;}
		
    if(tSys_flag.bTecAlfabetico==0)
    {
        if(func_plu_borrar(tDisplay.cPosCaracter)==TRUE)
        {
            tDisplay.cPosCaracter--;
        }
    }
		
		if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cContadorCh > 0)
    {
        display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
        tPluActual.cTexto[tPluActual.cContadorCh]=0;
        tPluActual.cContadorCh--;
        tPluActual.cChHex=0;
        tPluActual.cTeclaBase=0;
        tDisplay.cPosCaracter--;
    }
		
    else if(tSys_flag.bTecAlfabetico==TRUE && tPluActual.cContadorCh == 0)
    {
        display_print_un_caracter(' ', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
        tPluActual.cTexto[tPluActual.cContadorCh]=0;
        tPluActual.cChHex=0;
        tPluActual.cTeclaBase=0;
    }
			
}

/**
Función de evento (función secundaria) en la tecla X, hora y fecha
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_hora_fecha(void)
{
		//MODCALC
			if(tControl.cOp == OP_MENU)
    {
        return;
    }
		
		if (tControl.cEst==EST_CALCULADORA)
		{
			
			tCal2.Op = OP_DIVISION; 
			func_op_calc(tCal2.Op);
			func_print_etiqueta(MENSAJE_DIVISION, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
		}
		
		else{ 
			//No Se permite esta opcion en medio de un ingreso
			if((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU))return;
			
					cBufGen[0]='a';
					cmd_send(cBufGen, 1, 1,0,SIN_MENSAJE);
		}
}

/**
Función de evento de la tecla salir
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_salir(uint8_t bEstado)
{
	  if(tControl.cEst==EST_CALCULADORA)
    {
			tControl.cEst=EST_ESPERA;
		}
		
    if ((tControl.cOp >= OP_INGRESO_ID) && (tControl.cOp <= OP_INGRESO_CONTROL_FO))
    {

			for(uint8_t z = 0;z<=1;z++){
				//Se limpia estructura de control de display
				func_clean();
				//Se limpia estructura de control de ingreso y plu actual
				func_plu_ini(TRUE);
				//Se limpian banderas del sistema
				memset((char*)&tSys_flag,0,sizeof(tSys_flag));

				//Se inicializa el display
				display_ini();
				display_message_ini();
				//Se actualiza la variable de control
				tControl.cOp = OP_NO;

				tSys_flag.bTerceralinea=1;
				tControl.cEst=EST_NO_OP;
				tControl.cOp = OP_NO;
				
				if((bEstado==TRUE)||(tControl.cEst>=EST_REPORTES_MF))
				{
					tControl.cEst = EST_ESPERA;
				}
			}
				return;	
    }

//////////////////////////////////////////////////////////////////////////////////////////////

		if(cMenuAct != NO_SUBMENU)
		{
			if(tControl.cOp >=OP_CLAVE_X && tControl.cOp <= OP_CLAVE_S)
			{
				menu_principal();
				return;
			}
			menu_principal(); // [MarcaX]
		}
		else
		{
			//Se limpia estructura de control de display
			func_clean();
			//Se limpia estructura de control de ingreso y plu actual
			func_plu_ini(TRUE);
			//Se limpian banderas del sistema
			memset((char*)&tSys_flag,0,sizeof(tSys_flag));
			
			//Se inicializa el display
			display_ini();
			display_message_ini();
			//Se actualiza la variable de control
			tControl.cOp = OP_NO;

			tSys_flag.bTerceralinea=1;

			if((bEstado==TRUE)||(tControl.cEst>=EST_REPORTES_MF))
			{
					tControl.cEst = EST_ESPERA;
			}

			tAns.bAnularFactura = 0;
			tAns.bAnularPlu = 0;
			tAns.bAnulacionActiva = 0;	
			tAns.bDone = 0;
		
	}

}

//====================Teclado Numérico, Teclado Alfabético=========================================================
/**
Función que se encarga de los eventos en el teclado numérico
@numero caracter correspondiente a la tecla presionada
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_evento_teclado_numerico(uint8_t numero, uint16_t bdoblecero)
{
    uint8_t cMaxDig = 0;
		char *pbufferEntero;
		char *pbufferDecimal;
//		uint8_t cAux4[16];
	
    size_t i;
		
		if(tControl.cOp== OP_CLAVE_S && tControl.cEst== EST_FACTURA){tSys_flag.bTerceralinea = 0;}
		else{tSys_flag.bTerceralinea = 1;}
		
    Display_rotateUnSet();
		
    //Se valida si en la operacion actual se puede procesar la tecla numerica
	
    if((tControl.cOp == OP_INFO) || (tControl.cOp == OP_MENU) || (tSys_flag.bDeccomp == TRUE))
    {
        return;
    }
	
		if(numero == '0'){
			tAns.bzerokey = 1; //wplanchez
		}
    //Si la posicion es cero es el primer digito, se borra la linea completa
    if(tDisplay.cPosCaracter == 0)
    {
        display_erase_line(LINEA_SUPERIOR, DISPLAY_OPERADOR);
    }

    //Se verifican los limites de digitos permitidos
    func_plu_limite();

    //Se valida la aceptacion del separador decimal
    if((tIngresoMonto.cMaxDecimales == 0) && (numero == ','))
    {
        return;
    }
		
		if(tControl.cEst == EST_FACTURA){
			display_erase_line(LINEA_INFERIOR,DISPLAY_OPERADOR);
		}
    //Se define el maximo permitido
    if(tSys_flag.bDecimal == 0)
    {
        cMaxDig = tIngresoMonto.cMaxEnteros;
    }
    else
    {
        cMaxDig = tIngresoMonto.cMaxDecimales;
    }

    //Se actualiza el contador de digitos
    if(tSys_flag.bDecimal == 0)
    {
				//Ingreso de Enteros
        if(numero != ',')
        {
						/* Si esta dentro del limite se agrega el numero */
            if (tIngresoMonto.cContadorEnteros == cMaxDig)
            {
                return;
            }
						//INGRESO
            tIngresoMonto.sEnteros[tIngresoMonto.cContadorEnteros] = numero;
            tIngresoMonto.cContadorEnteros++;
						
						///////////////////////////////////////////////////////////////////////////////////////////

						memset(cAux3,0,sizeof(cAux3));
						memcpy(cAux3, tIngresoMonto.sEnteros, strlen((char*)tIngresoMonto.sEnteros));
						memset(pbufferEntero,0,sizeof(strlen((char*)tIngresoMonto.sEnteros)));
						sprintf(pbufferEntero,"%0llu,%01.2llu", atol((char*)cAux3)/100llu, atol((char*)cAux3)%100llu);
						
						if(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)
						{
							display_print((uint8_t*)pbufferEntero, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
						
						////////////////////////////////////////////////////////////////////////////////////////////

            if(tSys_flag.bCalc==1 || tSys_flag.bAns==1)
            {
                tCal2.sEnteros[tIngresoMonto.cContadorEnteros-1]=numero;
                tCal2.cContadorEnteros++;
            }

            if((bdoblecero==1)&&(tIngresoMonto.cContadorEnteros<cMaxDig))
            {
							
                tIngresoMonto.sEnteros[tIngresoMonto.cContadorEnteros]=numero;
                tIngresoMonto.cContadorEnteros++;
							
								memset(cAux3,0,sizeof(cAux3));
								memcpy(cAux3, tIngresoMonto.sEnteros, strlen((char*)tIngresoMonto.sEnteros));
								memset(pbufferEntero,0,sizeof(strlen((char*)tIngresoMonto.sEnteros)));
								sprintf(pbufferEntero,"%0llu,%01.2llu", atol((char*)cAux3)/100llu, atol((char*)cAux3)%100llu);
						
								if(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)
								{
									display_print((uint8_t*)pbufferEntero, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
								}

							
                if(tSys_flag.bCalc==1 || tSys_flag.bAns==1)
                {
                    tCal2.sEnteros[tIngresoMonto.cContadorEnteros]=numero;
                    tCal2.cContadorEnteros++;

                }
            }
            else bdoblecero=0;
        }
        else
        {
            tSys_flag.bDecimal=1;
            tSys_flag.bDeccomp=0;
					
						// Si la cantidad de enteros es cero muestra 0,XX . los digitos se van desplazando a la Izquierda
						if ((tIngresoMonto.cContadorEnteros == 0)&&(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)){
							sprintf(pbufferDecimal, "0,00%s", tIngresoMonto.sDecimales);
							display_print((uint8_t*)pbufferDecimal, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
						//Si no es cero X.00
						else if ((tIngresoMonto.cContadorEnteros != 0)&&(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)){
							sprintf(pbufferDecimal, "%s,00", tIngresoMonto.sEnteros);
							display_print((uint8_t*)pbufferDecimal, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
        }
    }
		
    else
    {
						// Ingreso de Decimales
            if(numero!=',')
            {
                if (tIngresoMonto.cContadorDecimal == cMaxDig)
                {
                    return;
                }
								
                tIngresoMonto.sDecimales[tIngresoMonto.cContadorDecimal]=numero;
                tIngresoMonto.cContadorDecimal++;
								memset(pbufferDecimal,0,sizeof(strlen((char*)tIngresoMonto.sDecimales)));
								
								// Si la cantidad de enteros es Cero y hay Decimales Muestra 0.D
								
								if ((tIngresoMonto.cContadorEnteros == 0)&&(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)){
									sprintf(pbufferDecimal, "0,%s", tIngresoMonto.sDecimales);
									display_print((uint8_t*)pbufferDecimal, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);

								}
								// Si no Muestra E.D
								else if((tIngresoMonto.cContadorEnteros != 0)&&(tControl.cOp <= OP_INGRESO && tControl.cEst != EST_CALCULADORA)) {
									sprintf(pbufferDecimal, "%s,%s", tIngresoMonto.sEnteros,tIngresoMonto.sDecimales);
									display_print((uint8_t*)pbufferDecimal, LINEA_SUPERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
								}
								
                if(tControl.cOp!=OP_NO && tControl.cEst==EST_CALCULADORA)
                {
                    tCal2.sEnteros[tIngresoMonto.cContadorEnteros]=numero;
                    tCal2.cContadorDecimal++;
                }

                if((bdoblecero==1)&&(tIngresoMonto.cContadorDecimal<cMaxDig))
                {
                    tIngresoMonto.sDecimales[tIngresoMonto.cContadorDecimal]=numero;
                    tIngresoMonto.cContadorDecimal++;

                    if(tControl.cOp!=OP_NO && tControl.cEst==EST_CALCULADORA)
                    {
                        tCal2.sEnteros[tIngresoMonto.cContadorEnteros]=numero;
                        tCal2.cContadorDecimal++;
                    }
                }
                else bdoblecero=0;
            }
            else
            {
                return;
            }
    }

    //Se muestra el caracter en el display
    for(i=0;i<=bdoblecero;i++)
    {
			if(tSys_flag.bTecNumerico == TRUE)
			{				
				if((tControl.cOp != OP_NO && tControl.cOp != OP_INGRESO) || (tControl.cEst == EST_CALCULADORA))
					{
						display_print_un_caracter(numero,tDisplay.cPosCaracter,LINEA_SUPERIOR,DISPLAY_AMBOS);
					}			
			}
			else
			{
				if((tControl.cOp != OP_NO && tControl.cOp != OP_INGRESO) || (tControl.cEst == EST_CALCULADORA))
				{
					display_print_un_caracter(numero,tDisplay.cPosCaracter,LINEA_SUPERIOR,DISPLAY_OPERADOR);
				}
			}
      tDisplay.cPosCaracter++;
    }

    //Se actualiza la operacion actual, entra a operacion de ingreso de datos
		if(tControl.cOp==OP_PROG_PLU_EXI || tControl.cOp==OP_PROG_EXI)
			return;
		else  if((tControl.cOp < OP_MODO) || (tControl.cOp > OP_PROG_MENSAJE))
    {
        tControl.cOp = OP_INGRESO;
    }
}

/**
Función que se encarga de los eventos en el teclado numérico
@hexadecimal tecla presionada en hexadecimal
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_hexadecimal_tecla(uint8_t tecla)
{
		
    //Se determina el caracter rotado
    //En la primera posicion estan los numeros
    tPluActual.cChHex=tPluActual.cTeclaBase;
		
    //Si es la primera vez que se presiona retorna
    if(tPluActual.cPosRot==0){
			return;
		}
    //Si corresponde a una tecla de numero y letra simple
    if((tecla>=0x32)&&(tecla<=0x39))
    {
        //Se valida la letra ñ
        if((tecla=='6')&&(tPluActual.cPosRot==4))
        {
            tPluActual.cChHex=0xD1;
        }
        else
        {
            tPluActual.cChHex+=0x0F;
            tPluActual.cChHex+=(tPluActual.cTeclaBase - 0x32)*2;
            if(tecla!=0x38&&tecla!=0x39)tPluActual.cChHex+=(tPluActual.cPosRot-1);
            else tPluActual.cChHex+=tPluActual.cPosRot;
        }
        //Se coloca el minuscula
        if(tSys_flag.bShiftActivo==1)
        {
            if((tecla=='6')&&(tPluActual.cPosRot==4))tPluActual.cChHex=0xF1;
            else tPluActual.cChHex+=0x20;
        }
    }
    else if(tecla == '1')
    {		
				// espacio ? ( ) * @ [ ] ~
				uint8_t cCharsKey_1[]= {0x20,0x3F,0x28,0x29,0x2A,0x40,0x5B,0x5D,0x7E};
				tPluActual.cChHex  = cCharsKey_1[tPluActual.cPosRot-1];
    }
    else if(tecla == '0')
    {
				// $ % # " ! ' .
				uint8_t cCharsKey_0[]= {0x24,0x25,0x23,0x22,0x21,0x27,0x2E};
				tPluActual.cChHex  = cCharsKey_0[tPluActual.cPosRot-1];
				//tAns.bzerokey = 1;
    }
    else if(tecla == ',')
    {	
				// , < > | + - / _ :
				uint8_t cCharsKey_Comma[]= {0x3C,0x3E,0x7C,0x2B,0x2D,0x2F,0x5F,0x3A};
				tPluActual.cChHex  = cCharsKey_Comma[tPluActual.cPosRot-1];
    }
}


/**
Función que se encarga de los eventos en el teclado numérico
@hexadecimal tecla presionada en hexadecimal
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_teclado_alfabetico(uint8_t tecla)
{
    uint8_t cMaxRotacion[]={7,9,3,3,3,3,4,4,3,4,1,8};

    Display_rotateUnSet();
		
    if(tPluActual.cContadorCh>=24)return;
		
		if(tControl.cOp == OP_INGRESO_ID && tPluActual.cContadorCh>8)
		{

			return;
		}
		
    //Si es el primer ingreso se inicializa la estructura
    if(tPluActual.cTeclaBase!=tecla)
    {
				func_teclas_direc_down();
			
        tPluActual.cTeclaBase=tecla;
        tPluActual.cPosRot=0;
			
        //Se setea el maximo a rotar por tecla
        if(tecla>=0x30 && tecla<=0x39)
        {
            tPluActual.cPosRotMax=cMaxRotacion[tecla-0x30];
        }
        else if(tecla==' ')
        {
            tPluActual.cPosRotMax=cMaxRotacion[10];
        }
        else if(tecla==',')
        {
            tPluActual.cPosRotMax=cMaxRotacion[11];
        }
    }

    //Se determina el caracter rotado
    func_hexadecimal_tecla(tecla);

		if(tControl.cOp==OP_PROG_CAJERO_NOMBRE)
		{
			display_print_un_caracter(tPluActual.cChHex,tDisplay.cPosCaracter,LINEA_SUPERIOR,DISPLAY_OPERADOR);
		}
	
		else{	
			display_print_un_caracter(tPluActual.cChHex,tDisplay.cPosCaracter,LINEA_SUPERIOR,DISPLAY_OPERADOR);
			
		}
		
    //Se incrementa la posicion del caracter
    tPluActual.cPosRot++;
    //Se verifica si ya se roto hasta el primer caracter de nuevo
    if(tPluActual.cPosRot>tPluActual.cPosRotMax)
    {
        tPluActual.cPosRot=0;
    }
}





//==================================Descuento y recargo=======================================================
/**
Funcion para evento en tecla de recargo/descuento por porcentaje
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Funcion llamada desde el evento de la tecla
*/
void func_descuento_menu(uint8_t cTipo)
{		
		// MODCALC
		if (tControl.cEst==EST_CALCULADORA && cTipo == 0)
		{
			
			tCal2.Op = OP_SUMA; 
			func_op_calc(tCal2.Op);
			func_print_etiqueta(MENSAJE_SUMA, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
		}
		
		else if (tControl.cEst==EST_CALCULADORA && cTipo == 1)
		{
			tCal2.Op = OP_RESTA; 
			func_op_calc(tCal2.Op);
			func_print_etiqueta(MENSAJE_RESTA, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
		}
		
    //Es necesario estar en una transaccion
    if((tControl.cEst<EST_FACTURA)||(tControl.cEst>EST_NOTA_CREDITO))return;

    //No Se permite esta opcion en medio de un ingreso
    if((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU))return;

    tSys_flag.bMenuOtrosActivo=1;       //Indica un menu activo
    tSys_flag.bPorMonto=cTipo;          //Indica operacion por MONTO
    menu_mostrar(tDescuentos, 1,LINEA_SUPERIOR);
		tSys_flag.bTecNumerico = TRUE;

}


//}
/**
Funcion para la selección descuento
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Funcion llamada desde el menu
*/
void func_descuento(void)
{
    //Se inicializa la estructura de montos
    func_plu_ini(TRUE);
    func_clean();
    //Se muestra la etiqueta en la pantalla
    mensaje_display(MENSAJE_DESC,DISPLAY_AMBOS);
    tDisplay.cPosCaracter=5;
    //Se actualiza el tipo de operacion en curso descuento por porcentaje o monto
    if(tSys_flag.bPorMonto==0)tControl.cOp=OP_DESC_POR;
    else tControl.cOp=OP_DESC_MONTO;

}

/**
Funcion para la selección recargo
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Funcion llamada desde el menu
*/
void func_recargo(void)
{
    //Se inicializa la estructura de montis
    func_plu_ini(TRUE);
    func_clean();
    //Se muestra la etiqueta en la pantalla
    mensaje_display(MENSAJE_REC,DISPLAY_AMBOS);
    tDisplay.cPosCaracter=4;
    //Se actualiza el tipo de operacion en curso
    //Se actualiza el tipo de operacion en curso descuento por porcentaje o monto
    if(tSys_flag.bPorMonto==0)tControl.cOp=OP_REC_POR;
    else tControl.cOp=OP_REC_MONTO;
}

//================================Fondo y Retiro=============================================

/**
Funcion de retiro de caja
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_fondo_retiro_menu(uint8_t cTipo)
{
    /* Solo se permite en espera */
    if(tControl.cEst != EST_ESPERA) return;

    /* No Se permite esta opcion en medio de un ingreso */
    if(tControl.cOp > OP_NO) return;

    /* Se inicializa la estructura de montos */
    func_plu_ini(TRUE);
    func_clean();
    /* Se muestra la etiqueta en la pantalla */
    mensaje_display(MENSAJE_TIPO, DISPLAY_OPERADOR);
    tDisplay.cPosCaracter = 5;
    /* Se actualiza el tipo de operacion en curso a ingreso de mod y el estado a fondo/retiro */
    tControl.cEst = EST_FONDO_RETIRO;
    tControl.cOp = OP_MODO;
    tSys_flag.bFondoRetiro = cTipo;         //Indicador de fondo
}

/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_fin_fondo_retiro(void)
{
    if(tControl.cEst!=EST_FONDO_RETIRO)return;

    //Se envia el comando de subtotal
    cBufGen[0] = 't';
    cBufGen[1] = 0;
		
		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				if(cmd_send(cBufGen, 1, 1, 1, SIN_MENSAJE) == TRUE)
				{
					tControl.cEst = EST_ESPERA;
					tSys_flag.bFondoRetiro = 0;
					func_salir(TRUE);
				}
				break;
			case 1 :
				if(cmd_send(cBufGen, 1, 1, 1, MENSAJE_CMD_OK) == TRUE)
				{
					tControl.cEst = EST_ESPERA;
					tSys_flag.bFondoRetiro = 0;
					func_salir(TRUE);
				}
				break;
			}

}

/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_cajero_menu(void)
{
		
		/* No se permite en medio de una venta */

		if(tSys_flag.bTecAlfabetico!=0)
    {
			return;
		}
		
			if ((tControl.cEst >= EST_FACTURA) && (tControl.cEst <= EST_NOTA_CREDITO))
			{		
					return;
			}

			func_plu_procesa_datos();
			
			if(tIngresoMonto.wEnteros > 4)
			{
				mensaje_display(MENSAJE_ERROR_INGRESO,DISPLAY_OPERADOR);
        func_plu_ini(TRUE);
        func_mensaje_error_ingreso(FALSE);
				return;
			}

			tPluActual.wIndex = tIngresoMonto.wEnteros;
		
			//Si esta saliendo o activando
			if(tPluActual.wIndex == 0 && tAns.bzerokey == 1 )
			{
					cBufGen[0]='6';
					cBufGen[1]=0;
					
					tAns.bzerokey = 0;
					
			switch(tConfig.cFlag.bMensajeOk)
			{
				case 0:
					cmd_send(cBufGen, strlen((char*)cBufGen), 1, 3, SIN_MENSAJE);
					break;
				case 1 :
					cmd_send(cBufGen, strlen((char*)cBufGen), 1, 3, MENSAJE_CMD_OK);
					break;
			}

					mensaje_display(MENSAJE_CAJERO, DISPLAY_OPERADOR);
					OSTimeDly(1500);
				
					func_plu_ini(TRUE);
					func_salir(FALSE);
			}
			
			else
			{		
					if(tPluActual.wIndex > 0 && tPluActual.wIndex < 5)
					{
						func_print_etiqueta(MENSAJE_CLAVE, 0, 6, LINEA_SUPERIOR, DISPLAY_OPERADOR);
						tAns.bzerokey = 0;
						tSys_flag.bTecAlfabetico=FALSE;
						tSys_flag.bIngresoDoble=FALSE;
						tControl.cOp=OP_CAJERO;
					}

			}
}

/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_activar_cajero(void)
{
    if (tControl.cEst != EST_ESPERA)
    {
        return;
    }

    if (strlen((char *)tIngresoMonto.sEnteros) == 4) {
        func_plu_procesa_datos();
			
			if(tIngresoMonto.wEnteros != tConfig.iClaveCajero[tPluActual.wIndex-1])
			{ 
				func_mensaje_error_ingreso(FALSE);//wplanchez
				return;
			}	
        tPluActual.wIndex--;
        cBufGen[0] = '5';
        sprintf((char*)cBufGen + 1, "%05d", tConfig.iClaveCajero[tPluActual.wIndex]);
        cmd_send(cBufGen, strlen((char*)cBufGen), 1, 3, DISPLAY_OPERADOR);
        tControl.cOp=OP_NO;
    }
}


/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_prog_cajero(void)
{
		func_plu_procesa_datos();

		if ((tPluActual.wInicio > 0) && (tPluActual.wInicio < 5))
		{
			if(tControl.cOp==OP_PROG_CAJERO)
			{
					tPluActual.wFin=tIngresoMonto.wEnteros;
					func_print_etiqueta(MENSAJE_NOMBRE, 0, 7, LINEA_SUPERIOR, DISPLAY_OPERADOR);//wplanchez
					tSys_flag.bTecAlfabetico=TRUE;
					tSys_flag.bIngresoDoble=FALSE;
					tControl.cOp=OP_PROG_CAJERO_NOMBRE;
			}
			else if(tControl.cOp==OP_PROG_CAJERO_NOMBRE)
			{
					sprintf((char*)cBufGen,"PC%02lld%05lld%s", tPluActual.wInicio, tPluActual.wFin, tPluActual.cTexto);
					func_config_leer();
					tConfig.iClaveCajero[tPluActual.wInicio - 1] = tPluActual.wFin;
					if(mem_read_write(MEM_CONFIG, (uint8_t *)&tConfig , 0, MODO_ESCRITURA) == TRUE)
					{
							cmd_send(cBufGen, strlen((char*)cBufGen), 1, 3, DISPLAY_OPERADOR);
						
							//Incidencia 530
							mensaje_display(MENSAJE_PROG_EXITO, DISPLAY_OPERADOR);
							OSTimeDly(1000);
						
							func_salir(TRUE);
						
					}
			 }
		}
		else
		{
			func_plu_ini(TRUE);
			func_mensaje_error_ingreso(FALSE);
		}
}

/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_prog_clave(void)
{
    if (strlen((char *)tIngresoMonto.sEnteros) == 4) {
        func_plu_procesa_datos();

        if(tControl.cOp!=OP_PROG_CLAVE) return;

        func_config_leer();
        if(tPluActual.wInicio==0)tConfig.iClaveX=tIngresoMonto.wEnteros;
        else if(tPluActual.wInicio==1)tConfig.iClaveZ=tIngresoMonto.wEnteros;
        else if(tPluActual.wInicio==2)tConfig.iClaveS=tIngresoMonto.wEnteros;

        if(mem_read_write(MEM_CONFIG,(uint8_t *)&tConfig,0,MODO_ESCRITURA)==TRUE)
        {		
						// Incedencia 530
						mensaje_display(MENSAJE_PROG_EXITO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
            func_salir(TRUE);
            tSys_flag.bTerceralinea=1;
        }
    }
    else {
        return;
    }
}


/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_prog_config(void)
{
    if(tControl.cOp!=OP_PROG_CONFIG) return;

    if(tIngresoMonto.cContadorEnteros!=5)return;

    func_config_leer();

    if(tIngresoMonto.sEnteros[0]=='1')
    {
        tConfig.cFlag.bPrecioFijo=1;
    }
    else
    {
        tConfig.cFlag.bPrecioFijo=0;
    }
    if(tIngresoMonto.sEnteros[1]=='1')
    {
        tConfig.cFlag.bCodigoPLU=1;
    }
    else
    {
        tConfig.cFlag.bCodigoPLU=0;
    }
    if(tIngresoMonto.sEnteros[2]=='1')
    {
        tConfig.cFlag.bMensajeComercial=1;
    }
    else
    {
        tConfig.cFlag.bMensajeComercial=0;
    }
		
		if(tIngresoMonto.sEnteros[3]=='1')
    {
        tConfig.cFlag.bMensajeOk =1;
				
    }
    else
    {
        tConfig.cFlag.bMensajeOk =0;

    }
		
		if(tIngresoMonto.sEnteros[4]=='1'){
			tConfig.cFlag.bConfigVenta=0;
		}else if(tIngresoMonto.sEnteros[4]=='2'){
			tConfig.cFlag.bConfigVenta=1;
		}else if(tIngresoMonto.sEnteros[4]=='3'){
		tConfig.cFlag.bConfigVenta=2;
		}
		
    if(mem_read_write(MEM_CONFIG,(uint8_t *)&tConfig,0,MODO_ESCRITURA)==TRUE)
    {
        func_salir(TRUE);
    }
}

//================================Funciones de Borrado de PLUs========================================

/**
func_eliminar_plu Permite Seleccionar el Numero de PLU a borrar
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_eliminar_plu(void)
{
	if(tAns.bBorrarPlu  == 0){
		tControl.cOp=OP_RANGO_PLU_NUMERO ;
		tAns.bBorrarPlu  = 1;
	}
	else{
		tControl.cOp=OP_NUMERO_PLU_ELIMINAR;
		tAns.bBorrarPlu  = 0;
	}
	
	switch (tControl.cOp) {
		case OP_RANGO_PLU_NUMERO:
			
			func_plu_ini(TRUE);
			tControl.cOp=OP_NUMERO_PLU_ELIMINAR;
			func_print_etiqueta(MENSAJE_NUMERO, 0, 4, LINEA_SUPERIOR, DISPLAY_OPERADOR);
			break;
		
		default:
			break;
    }
}

void func_eliminar_acc(void)
{
	if(tAns.bBorrarACC == 0){
		tControl.cOp=OP_RANGO_ACC_NUMERO ;
		tAns.bBorrarACC  = 1;
	}
	else{
		tControl.cOp=OP_NUMERO_ACC_ELIMINAR;
		tAns.bBorrarACC  = 0;
	}
	
	switch (tControl.cOp) {
		case OP_RANGO_ACC_NUMERO:
			
			func_plu_ini(TRUE);
			tControl.cOp=OP_NUMERO_ACC_ELIMINAR;
			func_print_etiqueta(MENSAJE_NUMERO, 0, 4, LINEA_SUPERIOR, DISPLAY_OPERADOR);
			break;
		
		default:
			break;
    }
}

/**
func_elimina_plu Elimina el PLU seleccionado
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_elimina_plu(void)
{
		uint8_t cResp = TRUE;
		tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
	
		if(tPluActual.iIndex>0 && tPluActual.iIndex<=3000)
		{				
			
			/* Se lee la memoria de trabajo */
			cResp = MEM_leerPLU(pPluAux->cValor, tPluActual.iIndex );	
			if(cResp==TRUE){
				/* Se gabraba Nuevamente el PLU con la Marca Cambiada a no Programada*/
				
				cResp = MEM_escribirPLU(pPluAux->cValor, tPluActual.iIndex ,PLU_NOPROG);
				if(cResp==TRUE){
					mensaje_display(MENSAJE_BORRADO_EXITO, DISPLAY_OPERADOR);
					OSTimeDly(1000);
					func_salir(TRUE);
					}
					else{
					mensaje_display(MENSAJE_BORRADO_ERROR, DISPLAY_OPERADOR);
					OSTimeDly(1000);
					func_salir(TRUE);
				}
		}
		else{
			mensaje_display(MENSAJE_BORRADO_ERROR, DISPLAY_OPERADOR);
			OSTimeDly(1000);
			func_salir(TRUE);
		}					
	}
	else{
		func_eliminar_plu();	
		func_salir(TRUE);
	}
}

/**
func_elimina_acc Elimina el acceso directo seleccionado
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_elimina_acc(void)
{
		uint8_t cResp = TRUE;
		tDIRECTO_INFO *pDirectoInfo = (tDIRECTO_INFO *)cBufAux;
	
		if(tPluActual.iIndex<=16)
		{				
			
			/* Se lee la memoria de trabajo */
			cResp = MEM_leerAccesoDirecto(pDirectoInfo->cValor, tPluActual.iIndex );
	
			if(cResp==TRUE){
				/* Se gabraba Nuevamente el PLU con la Marca Cambiada a no Programada*/
				
				cResp = MEM_escribirAccesoDirecto(pDirectoInfo->cValor, tPluActual.iIndex ,PLU_NOPROG);
				if(cResp==TRUE){
					mensaje_display(MENSAJE_BORRADO_EXITO, DISPLAY_OPERADOR);
					OSTimeDly(1000);
					func_salir(TRUE);
					}
					else{
					mensaje_display(MENSAJE_BORRADO_ERROR, DISPLAY_OPERADOR);
					OSTimeDly(1000);
					func_salir(TRUE);
				}
		}
		else{
			mensaje_display(MENSAJE_BORRADO_ERROR, DISPLAY_OPERADOR);
			OSTimeDly(1000);
			func_salir(TRUE);
		}					
	}
	else{

		func_eliminar_acc();	
		func_salir(TRUE);
	}
}
/**
func_borrado_total_plu Elimina todos los PLU existentes
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_borrado_total_plu(void){
	
	uint8_t cResp = TRUE;
	tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;

	 /* Solo se permite se permite en este estado */
	if (tControl.cEst != EST_BORRAR_TODO_PLU) 
	{
		return;
  }
	/* Se lee la memoria de trabajo */
		
	cResp = MEM_leerPLU(pPluAux->cValor, tPluActual.wInicio);		
	if(cResp==TRUE)
	{
		/* Se gabraba Nuevamente el PLU con la Marca Cambiada a no Programada*/
		cResp=MEM_escribirPLU(pPluAux->cValor, tPluActual.wInicio,PLU_NOPROG);
				
		if(cResp==TRUE)
		{
			/* Si se escribo correctamente avanzamos al siguiente */
			tPluActual.wInicio++;
			cIntentos=0;
		}
		else
		{
			/* Si no intentamos tres veces, si se supera se avanza al siguiente */
			cIntentos++;
			if(cIntentos==3)
			{
				cIntentos=0;
				tPluActual.wInicio++;
			}
		}
	}
	/* Si la respuesta es False porque el PLU No esta programado y se avanza al siguiente */
	else{
		tPluActual.wInicio++;
	}
							
	//Si termino de borrar todos los PLU 				
	if(tPluActual.wInicio==3001){
		tControl.cEst = EST_ESPERA;
		mensaje_display(MENSAJE_BORRADO_EXITO, DISPLAY_OPERADOR);
		OSTimeDly(1000);
		func_salir(TRUE);
	}
}
/*
func_borrado_total_acc Elimina todos los Accesos Directos existentes
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/

void func_borrar_accesos(void){
	
	uint8_t cResp = TRUE;
  tDIRECTO_INFO *pDirectoInfo = (tDIRECTO_INFO *)cBufAux;

	/* Solo se permite se permite en este estado */
	if (tControl.cEst != EST_BORRAR_ACCESOS)
	{
		return;
  }
	
	/* Se lee la memoria de trabajo */
	cResp = MEM_leerAccesoDirecto(pDirectoInfo->cValor,tPluActual.wInicio);
		
	if(cResp==TRUE)
	{

	/* Se gabraba Nuevamente el PLU con la Marca Cambiada a no Programada*/
	cResp = MEM_escribirAccesoDirecto(pDirectoInfo->cValor,tPluActual.wInicio,PLU_NOPROG);
				
		if(cResp==TRUE)
		{
			/* Si se escribo correctamente avanzamos al siguiente */
			tPluActual.wInicio++;
			cIntentos=0;
		}
		else
		{
		/* Si no intentamos tres veces, si se supera se avanza al siguiente */
			cIntentos++;
			if(cIntentos==3)
			{
				cIntentos=0;
				tPluActual.wInicio++;
			}
		}
	}
			/* Si la respuesta es False porque el PLU No esta programado y se avanza al siguiente */
			else
			{
				tPluActual.wInicio++;
			}
							
			//Si termino de borrar todos los PLU 				
		if(tPluActual.wInicio==17){
			tControl.cEst = EST_ESPERA;
			func_salir(TRUE);
		}
		//else tControl.cEst = EST_BORRAR_TODOS_PLU;
}


/**
Comando de fin de fondo/retiro
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_prog_mensaje(void)
{
    tMSG_COM msgcom;

    func_config_leer();

    if (strlen((char *)tPluActual.cTexto) != 0)
    {
        strncpy((char *)msgcom.cMensaje, (char *)tPluActual.cTexto, 127);
        msgcom.cMarca = MARCA;
        mem_read_write(MEM_MSG_COMERCIAL, msgcom.cValor, 0, MODO_ESCRITURA);
    }

    display_message_ini();
}

/**
Comando de fin de fondo/retiro
*/
void func_reimpresion_ultimo(void)
{
    //Se permite en estado de transaccion
    if(tControl.cEst!=EST_ESPERA)return;

    //No debe existir una operacion en curso
    if(tControl.cOp!=OP_NO)return;

    //Se envia el comando de subtotal
    sprintf((char*)cBufGen, "RU00000000000000");
		
		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				cmd_send(cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE);
				break;
			case 1 :
				cmd_send(cBufGen, strlen((char*)cBufGen), 1, 1, MENSAJE_CMD_OK);
				break;
		}
}


//===============================Mensajes de modo y monto (fondo,retiro,pago parcial, nota de debito y credito==============================================


/**
Imprime una etiqueta en el display para un ingreso de datos
@retval Resultado de la operacion
*/
void func_print_etiqueta(uint8_t cMensaje1, uint8_t cMensaje2,uint8_t cPos,uint8_t cLinea,  uint8_t cDisplay)
{
    uint8_t cEtiqueta[20]={NULL};

    func_clean();
    func_plu_ini(FALSE);
    tSys_flag.bTerceralinea=0;
    tSys_flag.bErase=1;
    display_erase_line(AMBAS_LINEAS,cDisplay);

    //Se coloca el descriptor de ingreso
    memset(cEtiqueta,0,sizeof(cEtiqueta));
    mensaje_leer(cEtiqueta,cMensaje1);

    if(cLinea==AMBAS_LINEAS)
    {
        tSys_flag.bTerceralinea=0;
        tSys_flag.bErase=1;
        display_print(cEtiqueta,LINEA_SUPERIOR,FONDO_NEGRO,cDisplay);
        //Se coloca el descriptor de ingreso
        memset(cEtiqueta,0,sizeof(cEtiqueta));
        mensaje_leer(cEtiqueta,cMensaje2);
        display_print(cEtiqueta,LINEA_INFERIOR,FONDO_NEGRO,cDisplay);
    }
    else
    {
        tSys_flag.bTerceralinea=0;
        tSys_flag.bErase=1;
        display_print(cEtiqueta,cLinea,FONDO_NEGRO,cDisplay);
        if(cLinea==LINEA_INFERIOR)tDisplay.cPosCaracter+=16;
    }

    //Posicion de ingreso
    if(cPos==0)tDisplay.cPosCaracter=strlen((char*)cEtiqueta);
    else tDisplay.cPosCaracter=cPos;

    if (((cMensaje1 >= MENSAJE_RANGO_NUM1) && (cMensaje1 <= MENSAJE_RANGO_NUM2)) ||
        ((cMensaje1 >= MENSAJE_MONTO) && (cMensaje1 <= MENSAJE_REC)) ||
        ((cMensaje1 >= MENSAJE_TASAS) && (cMensaje1 <= MENSAJE_CONTROL_FAC)) ||
        (((cMensaje1 >= MENSAJE_NUMERO) && (cMensaje1 <= MENSAJE_TECLA)) && (cMensaje1 != MENSAJE_CODIGO)&&(cMensaje1 != MENSAJE_CLAVE)))
    {
        display_print_un_caracter('_', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
    }
		if(cMensaje1==MENSAJE_EXISTENCIA)
			{
        display_print_un_caracter('_', tDisplay.cPosCaracter, LINEA_SUPERIOR, DISPLAY_OPERADOR);
    }
}

/**
Ingreso del intervalo inicial de fecha/numero de reporte histórico
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_historico(uint8_t cTipo)
{
    uint8_t cPos = 0;
    uint8_t aux[2];

    /* Se arma el comando */
    cBufGen[cPos++] = 'I';
    /* Tipo de rango de reporte */
    if(tSys_flag.bFechaNumero == 0)
    {
        cBufGen[cPos++] = '2';
    }
    else
    {
        cBufGen[cPos++] = '3';
    }
    /* Reporte resumen/completo/mensual */
    cBufGen[cPos++] = cTipo;
    /* Rango Inicio (6) */
    sprintf((char*)cBufGen + cPos, "%06lld", tPluActual.wInicio);
    if (tSys_flag.bFechaNumero == 0)
    {
        aux[0] = cBufGen[cPos + 4];
        aux[1] = cBufGen[cPos + 5];
        cBufGen[cPos + 4] = cBufGen[cPos + 0];
        cBufGen[cPos + 5] = cBufGen[cPos + 1];
        cBufGen[cPos + 0] = aux[0];
        cBufGen[cPos + 1] = aux[1];
    }
    cPos += 6;
    /* Rango Fin (6) */
    sprintf((char*)cBufGen + cPos, "%06lld", tPluActual.wFin);
    if (tSys_flag.bFechaNumero == 0)
    {
        aux[0] = cBufGen[cPos + 4];
        aux[1] = cBufGen[cPos + 5];
        cBufGen[cPos + 4] = cBufGen[cPos + 0];
        cBufGen[cPos + 5] = cBufGen[cPos + 1];
        cBufGen[cPos + 0] = aux[0];
        cBufGen[cPos + 1] = aux[1];
    }
		
		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE)==TRUE)
				{
					display_message_ini();
					tControl.cEst=EST_ESPERA;
					tControl.cOp=OP_NO;
					tSys_flag.bTerceralinea=1;
					return ;
				}
				else
				{
					return ;
				}
				//break;
				case 1 :
					if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, MENSAJE_CMD_OK)==TRUE)
					{
						display_message_ini();
						tControl.cEst=EST_ESPERA;
						tControl.cOp=OP_NO;
						tSys_flag.bTerceralinea=1;
						return ;
					}
					else
					{
						return ;
					}
					//break;
		}

}



/**
Ingreso del intervalo final de fecha/numero de reporte histórico
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_rango_fecha(void)
{
    tSys_flag.bShiftActivo=0;
    tSys_flag.bTerceralinea=0;
    tSys_flag.bErase=1;
    //Se coloca el descriptor de ingreso
    func_print_etiqueta(MENSAJE_RANGO_FECHA1,MENSAJE_RANGO_FECHA2, 6, AMBAS_LINEAS, DISPLAY_OPERADOR); //wplanchez
	
    //Se actualiza el estado
    func_plu_ini(TRUE);
    tSys_flag.bIngresoDoble=TRUE;
    tSys_flag.bTecAlfabetico=FALSE;
    tSys_flag.bMenuOtrosActivo=TRUE;
    tControl.cOp=OP_RANGO_FECHA;
	
		if(cMenuAct == SUBMENU_EJ1){cMenuAct = SUBMENU_EJ2;}
}

/**
Ingreso del intervalo final de fecha/numero de reporte histórico
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_rango_numero(void)
{
    tSys_flag.bShiftActivo=0;
    tSys_flag.bTerceralinea=0;
    tSys_flag.bErase=1;
    //Se coloca el descriptor de ingreso
    func_print_etiqueta(MENSAJE_RANGO_NUM1, MENSAJE_RANGO_NUM2, 6, AMBAS_LINEAS, DISPLAY_OPERADOR);//wplanchez

    //Se actualiza el estado
    func_plu_ini(TRUE);
    tSys_flag.bIngresoDoble=TRUE;
    tSys_flag.bTecAlfabetico=FALSE;
    tSys_flag.bMenuOtrosActivo=TRUE;
    tControl.cOp=OP_RANGO_NUMERO;
		if(cMenuAct == SUBMENU_EJ1 ){cMenuAct = SUBMENU_EJ2;}
}

/**
Validacion de datos de un campo de fecha
@wFecha Valor de la fecha a Validar
@bOrden Formato de la fecha AAMMD(0)-DDMMAA(1)
@retval Retorna Valido o Invalido
*/
uint8_t func_validar_fecha(uint64_t wFecha, uint8_t bOrden)
{
    uint8_t cResp=TRUE,iAux=0;
    uint8_t cAux[10],cAux2[3];

    if(wFecha==0)cResp=FALSE;

    //Validacion de inicio
    sprintf((char*)cAux,"%06lld",wFecha);

    //Se valida el mes
    memcpy(cAux2,cAux+2,2);
    cAux2[2]=0;
    iAux=atoi((char*)cAux2);
    if(iAux<1 || iAux>12)cResp=FALSE;

    //Se valida el dia
    if(bOrden==0)memcpy(cAux2,cAux+4,2);
    else memcpy(cAux2,cAux,2);
    cAux2[2]=0;
    iAux=atoi((char*)cAux2);
    if(iAux<1 || iAux>31)cResp=FALSE;

    return cResp;
}



/**
Ingreso del intervalo final de fecha/numero de reporte histórico
*/
void func_menu_historicos(void)
{
    uint8_t cResp=TRUE;

    //No Se permite esta opcion en medio de un ingreso
    if(tControl.cEst!=EST_REPORTES_MF)return;

    if(tControl.cOp==OP_RANGO_FECHA || tControl.cOp==OP_RANGO_NUMERO)
    {
        //Se procesa la informacion del monto
        func_plu_procesa_datos();
        //Se actualiza la informacion del plu actual
        tPluActual.wFin = tIngresoMonto.wEnteros;

        if(tControl.cOp==OP_RANGO_FECHA)
        {
            cResp&=func_validar_fecha(tPluActual.wInicio,0);
            cResp&=func_validar_fecha(tPluActual.wFin,0);
            if(tIngresoMonto.cContadorEnteros!=tIngresoMonto.cMaxEnteros)cResp=FALSE;
        }
        else
        {
            if(tPluActual.wInicio==0 || tPluActual.wFin==0)cResp=FALSE;
        }

        if(tPluActual.wInicio>tPluActual.wFin)cResp=FALSE;

        if(cResp==TRUE)
        {
                func_clean();
                func_plu_ini(FALSE);
                tSys_flag.bMenuOtrosActivo=1;
                tSys_flag.bIngresoDoble=FALSE;
                tSys_flag.bTecAlfabetico=FALSE;
                if(tControl.cOp==OP_RANGO_FECHA)
                {
                    tSys_flag.bFechaNumero=0;
                }
                else
                {
                    tSys_flag.bFechaNumero=1;
                }
                if (tSys_flag.bFechaNumero == 0)
                {
                    menu_mostrar(tHistoricos, 1, LINEA_SUPERIOR);
                }
                else
                {
                    menu_mostrar(tHistoricos2, 1, LINEA_SUPERIOR);
                }

        }
        else
        {
            func_mensaje_error_ingreso(TRUE);
        }
    }
}

/**
Estatus EJ
*/
void func_estatus_EJ(void)
{
    //Se arma el comando
    cBufGen[0]='E';
		switch(tConfig.cFlag.bMensajeOk)
		{
		case 0:
			cmd_send(cBufGen, 1, 1,1,SIN_MENSAJE);
			break;
		case 1 :
			cmd_send(cBufGen, 1, 1,1,MENSAJE_CMD_OK);
			break;
		}
}
/**
Ingreso del intervalo final de fecha/numero de reporte histórico
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_menu_reimpresiones(void)
{
    uint8_t cResp=TRUE;
    //No Se permite esta opcion en medio de un ingreso
    if(tControl.cEst!=EST_REPORTES_EJ)return;

    if(tControl.cOp==OP_RANGO_FECHA || tControl.cOp==OP_RANGO_NUMERO)
    {
        //Se procesa la informacion del monto
        func_plu_procesa_datos();
        //Se actualiza la informacion del plu actual
        tPluActual.wFin = tIngresoMonto.wEnteros;

        if(tControl.cOp==OP_RANGO_FECHA)
        {
            cResp&=func_validar_fecha(tPluActual.wInicio,0);
            cResp&=func_validar_fecha(tPluActual.wFin,0);
            if(tIngresoMonto.cContadorEnteros!=tIngresoMonto.cMaxEnteros)cResp=FALSE;
        }
        else
        {
            if(tPluActual.wInicio==0 || tPluActual.wFin==0)cResp=FALSE;
        }

        if(tPluActual.wInicio>tPluActual.wFin)cResp=FALSE;

        if(cResp==TRUE)
        {
            func_clean();
            func_plu_ini(FALSE);
            tSys_flag.bMenuOtrosActivo=1;
            if(tControl.cOp==OP_RANGO_FECHA)tSys_flag.bFechaNumero=0;
            else tSys_flag.bFechaNumero=1;
            menu_mostrar(tTipoDoc, 2,LINEA_SUPERIOR);
        }
        else
        {
            func_mensaje_error_ingreso(TRUE);
        }
    }
}
/**
Menu de reimpresion de documentos
*/
void func_reimpresion(uint8_t cTipo)
{
    uint8_t cPos=0;

    //Se arma el comando
    cBufGen[cPos++]='R';
    //Tipo de documento a reimprimir
    if(tSys_flag.bFechaNumero==0)
    {
        cBufGen[cPos++]=cTipo;
    }
    else
    {
        cTipo=toupper(cTipo);
        cBufGen[cPos++]=cTipo;
    }
    /* Rango Inicio (7) */
    sprintf((char*)cBufGen+cPos,"%07lld",tPluActual.wInicio);
    cPos+=7;
    /* Rango Fin (7) */
    sprintf((char*)cBufGen+cPos,"%07lld",tPluActual.wFin);
		
		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, SIN_MENSAJE)==TRUE)
				{
					tControl.cEst=EST_ESPERA;
					tControl.cOp=OP_NO;
					tSys_flag.bMenuAtivo=0;
					tSys_flag.bTerceralinea=1;
					display_message_ini();
					return ;
				}
				else
				{
					return ;
				}
//				break;
			case 1 :
				if(cmd_send((uint8_t*)cBufGen, strlen((char*)cBufGen), 1, 1, MENSAJE_CMD_OK)==TRUE)
				{
					tControl.cEst=EST_ESPERA;
					tControl.cOp=OP_NO;
					tSys_flag.bMenuAtivo=0;
					tSys_flag.bTerceralinea=1;
					display_message_ini();
					return ;
				}
				else
				{
					return ;
				}
//				break;
		}
		

}

/**
Calculadora
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_calculadora(void)
{
    //No Se permite esta opcion en medio de un ingreso
    if((tControl.cOp==OP_MENU)||((tControl.cOp>=OP_INGRESO)&&(tControl.cOp<=OP_PLU)))
		{
			  tSys_flag.bCalc=0;
				tControl.cEst=EST_ESPERA;
        tControl.cOp=OP_NO;
			
				wCal1= 0; 	//Valor Antes del signo de operación en modo calculador
				wCal2=0;		//Valor despues del signo de operación en modo calculadora
				wAns=0;
					
			  func_calc_clean();
        func_clean();
			
			  display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);
        display_message_ini();
			
			return; /* TODO */
		}
		

    if(tControl.cEst!=EST_CALCULADORA)
    {
        tControl.cEst=EST_CALCULADORA;
				tControl.cOp = OP_CALC;

        display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);
			
				wCal1= 0; 	//Valor Antes del signo de operación en modo calculador
				wCal2=0;		//Valor despues del signo de operación en modo calculadora
				wAns=0;	
				
				tSys_flag.bCalc=1;
			
				tFlag.bCalc=0;
				//Se inicializa la bandera de operacion en curso
				tFlag.bCalc2=0;
				//Reset A la bandera para poder realizar operaciones con el resultado
				tFlag.bCalc3 = 0;
						
			  Display_rotateUnSet();
				func_print_etiqueta(MENSAJE_CALCULADORA, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
    }
		
    else if(tControl.cEst==EST_CALCULADORA)
    {
				tControl.cEst=EST_ESPERA;
        tControl.cOp=OP_NO;
        display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);
        func_calc_clean();
        func_clean();
        tSys_flag.bCalc=0;

				memset(tCal1.sEnteros,'0',sizeof(tCal1.sEnteros)); 	
        memset(tCal2.sEnteros,'0',sizeof(tCal1.sEnteros)); 	

				Display_rotateSet();
        display_message_ini();
    }
}

uint64_t func_ingreso_obtenerNumero(void){
		
			strlen((char*)tIngresoMonto.sEnteros);
			
			memset(cAux3,0,sizeof(cAux3));
			memcpy(cAux3, tIngresoMonto.sEnteros, strlen((char*)tIngresoMonto.sEnteros));
			tCal2.wEnteros=atol((char*)cAux3)*1000llu;

			memset(cAux3,0,sizeof(cAux3));
			memcpy(cAux3, tIngresoMonto.sDecimales, strlen((char*)tIngresoMonto.sDecimales));
			tCal2.wDecimal=atol((char*)cAux3)*100llu;;
			
			return tCal2.wEnteros + tCal2.wDecimal;

}

void func_op_calc(uint8_t operacion)
{
		tSys_flag.bShiftActivo = 0;
	
		if(tCal1.Op==0xFF)
    {
        tControl.cOp=OP_INGRESO;
        display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);//INGRESO

    }		

    //Solo se permite en modo calculadora
    if (tControl.cEst!=EST_CALCULADORA)return;
		
//		Permite Realizar Operaciones Con el resultado Anterior

		if( tFlag.bCalc3==1){
			
			if(func_ingreso_obtenerNumero()==0)
				tFlag.bCalc2 = 1;
				tFlag.bCalc3=0;
		}
		//Se pregunta si ya existe una operacion en curso, si asi no captura el valor
		
		if(tFlag.bCalc2 == 0 ){
			
			wCal1 = func_ingreso_obtenerNumero();
		}

		//Se Setea la bandera para indicar un operacion en curso
		tFlag.bCalc2 = 1;
		
		////////////////////////////////////////////////////////////////////////////////
		
    switch(operacion)
    {
        case OP_SUMA:
            tControl.cOp=OP_SUMA; 
            tCal2.Op=OP_SUMA;	
						func_plu_ini(FALSE);
            break;
        case OP_RESTA:
						tControl.cOp=OP_RESTA; 
            tCal2.Op=OP_RESTA;
						func_plu_ini(FALSE);
            break;
        case OP_MULTIPLICACION:
						tControl.cOp=OP_MULTIPLICACION; 
            tCal2.Op=OP_MULTIPLICACION;
						func_plu_ini(FALSE);
            break;
        case OP_DIVISION:
						tControl.cOp=OP_DIVISION; 			
            tCal2.Op=OP_DIVISION;
						func_plu_ini(FALSE);
            break;
    }

    tSys_flag.bDecimal=0;

    //Se procesa la informacion
    func_plu_procesa_datos();

    //Se actualiza la informacion
    if(tPluActual.wCantidad==0)
    {
        tPluActual.wCantidad = tIngresoMonto.wEnteros*1000LL;
        tPluActual.wCantidad += tIngresoMonto.wDecimal;
        func_plu_ini(FALSE);
    }
		
    else
    {
        mensaje_display(MENSAJE_ERROR_INGRESO,DISPLAY_OPERADOR);
        func_plu_ini(TRUE);
        func_mensaje_error_ingreso(FALSE);
    }

        tSys_flag.bDeccomp=0;
        tSys_flag.bDecimal=0;
        memset((char*)tIngresoMonto.cValor,0,sizeof(tIngresoMonto));
        tControl.cEst=EST_CALCULADORA;

        switch(operacion)
    {
        case OP_SUMA:
            tControl.cOp=OP_SUMA;
            tCal1.Op=OP_SUMA;
            tCal2.Op=OP_SUMA;
            break;
        case OP_RESTA:
            tControl.cOp=OP_RESTA;
            tCal1.Op=OP_RESTA;
            tCal2.Op=OP_RESTA;
            break;
        case OP_MULTIPLICACION:
            tControl.cOp=OP_MULTIPLICACION;
            tCal1.Op=OP_MULTIPLICACION;
            tCal2.Op=OP_MULTIPLICACION;
            break;
        case OP_DIVISION:
            tControl.cOp=OP_DIVISION;
            tCal1.Op=OP_DIVISION;
            tCal2.Op=OP_DIVISION;
            break;
    }
}

/**
Totalizar operación calculadora
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_op_calculadora(void)
{		
		//Obtener Numeros ingresados

		wCal2 = func_ingreso_obtenerNumero();
		
		//MODCALC
		func_print_etiqueta(MENSAJE_RESULTADO, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
		func_plu_ini(FALSE);

    if(tCal2.Op==OP_MULTIPLICACION)
    {		
			func_mult();
    }
    else if(tCal2.Op==OP_DIVISION)
    {
      func_div();
    }
    else if(tCal2.Op==OP_RESTA)
    {
				if(wCal2<=wCal1){
					func_resta(wCal1,wCal2);
				}
				else{
					func_resta(wCal2,wCal1);
				}
    }
    else if(tCal2.Op==OP_SUMA)
    {
			func_suma();
    }
		
		//Permite Realizar otra operacion despues de obtener el resultado de la operacion anterior. 
	  tControl.cEst=EST_CALCULADORA;
    tControl.cOp=OP_NO; 
		func_plu_ini(FALSE);		
		//Bandera que permite tomar el nuevo numero de la operacion
		tFlag.bCalc2 = 0;
		//Bandera Realizar Operaciones Con el resultado Anterior
		tFlag.bCalc3 = 1;		
}

void func_calc_clean(void)
{
    tCal1.Op=NULL;
    tCal2.Op=NULL;
}

/**
Convierte de long int a string
@str[] Se guarda la conversión
@num Long int a convertir
@retval Conversión
@note  Nota importante.
*/
void tostring(char str[], long long int num) //tratar long int a string
{ long long  int i, rem, len = 0, n;

   n = num;
   while (n != 0)
    {
      len++;
      n /= 10;
     }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

/**
Suma Calculadora
@param Descripcion de Parametro 1
@param Descripcion de Parametro 2
@retval Resultado de la operacion
@note  Nota importante.
*/
void func_suma(void)
{	
		char *bufferP;
		memset(bufferP,0,sizeof((uint8_t*)bufferP));
		wAns = wCal1+wCal2;
	
		sprintf(bufferP,"%llu,%03llu",wAns / 1000llu, wAns% 1000llu);
		display_print((uint8_t*) bufferP, LINEA_INFERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
	
    tControl.cEst=EST_CALCULADORA;
    tControl.cOp=OP_NO;

    tSys_flag.bCalc=0;
    tCal1.Op=0xFF;
	
		wCal1 = wAns;
//		wAns = 0;
		wCal2 = 0;
}

void func_resta(uint64_t wNum1,uint64_t wNum2){
	
		char *bufferP;
		memset(bufferP,0,sizeof((uint8_t*)bufferP));
		wAns = wNum1-wNum2;
		sprintf(bufferP,"%llu,%03llu",wAns / 1000llu, wAns % 1000llu);
		display_print((uint8_t*) bufferP, LINEA_INFERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
	
    tControl.cEst=EST_CALCULADORA;
    tControl.cOp=OP_NO;

    tSys_flag.bCalc=0;
    tCal1.Op=0xFF;
		wCal1 = wAns;
		wCal2 = 0;
}

void func_mult(void){
	
		char *bufferP;
		memset(bufferP,0,sizeof((uint8_t*)bufferP));
	
		tControl.cEst=EST_CALCULADORA;
    tControl.cOp=OP_NO;

		wAns = wCal1*wCal2;
		
		sprintf(bufferP,"%llu,%03llu",wAns / 1000000llu, wAns % 1000000llu);
	
		if(wCal1 <= 99999999 && wCal2 <= 99999999){

			display_print((uint8_t*) bufferP, LINEA_INFERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
		}
		
		else{
				mensaje_display(MENSAJE_ERROR_DIV, DISPLAY_OPERADOR);
				OSTimeDly(1500);
				func_salir(FALSE);
		}
		
    tSys_flag.bCalc=0;
    tCal1.Op=0xFF;
		
		wCal1 = wAns/1000;
		wCal2 = 0;
}

void func_div(void){
	
		char *bufferP;
		memset(bufferP,0,sizeof((uint8_t*)bufferP));
		
		tControl.cEst=EST_CALCULADORA;
    tControl.cOp=OP_NO;
	   if(wCal2>0)
		{
			
			uint64_t wrEntero=0;
			uint64_t wDecimal=0;
 
			wrEntero = Divisiones(&wCal1, &wCal2);
	
			for(uint8_t i = 0; i<3; i++)
						{
								if(wCal1 != 0)
										{
											wCal1 = wCal1*10;
												if(i==0)
													wDecimal=Divisiones(&wCal1, &wCal2)*100+wDecimal;
												if(i==1)
													wDecimal=Divisiones(&wCal1, &wCal2)*10+wDecimal;
												if(i==2)
													wDecimal=Divisiones(&wCal1, &wCal2)*1+wDecimal;
										}
								if(wCal1==0)
									break;
						}
			
   		sprintf(bufferP,"%llu,%03llu",wrEntero,wDecimal);
			display_print((uint8_t*) bufferP, LINEA_INFERIOR, FONDO_NEGRO, DISPLAY_OPERADOR);
			wAns = wrEntero + wDecimal;
		}
	else
		{
				mensaje_display(MENSAJE_ERROR_DIV, DISPLAY_OPERADOR);
				OSTimeDly(1500);
				func_salir(FALSE);
		}
		tSys_flag.bCalc=0;
    tCal1.Op=0xFF;
		
		wCal1 = wAns*1000;
		wCal2 = 0;

}

uint64_t Divisiones (uint64_t *wDividendo, uint64_t *wDivisor)
{
	uint64_t cContador=0;
	while(*wDividendo >= *wDivisor)
		{		
			*wDividendo = *(wDividendo)-*(wDivisor);
			cContador=cContador+1;
		}
	return cContador;
}

//===========================Programación Fecha, Hora y Flags ==================================================

/**
Ingreso del intervalo final de fecha/numero de reporte histórico
@wHora Valor de la hora
*/
uint8_t func_validar_hora(uint64_t wHora)
{
    uint8_t cResp=TRUE,iAux=0;
    uint8_t cAux[10],cAux2[3];

    //Validacion de hora
    sprintf((char*)cAux,"%06lld",wHora);
    memcpy(cAux2,cAux,2);
    cAux2[2]=0;
    iAux=atoi((char*)cAux2);
    if(iAux>23)cResp=FALSE;

    //Se valida el minuto
    memcpy(cAux2,cAux+2,2);
    cAux2[2]=0;
    iAux=atoi((char*)cAux2);
    if(iAux>59)cResp=FALSE;

    //Se validan segundos
    memcpy(cAux2,cAux+4,2);
    cAux2[2]=0;
    iAux=atoi((char*)cAux2);
    if(iAux>59)cResp=FALSE;

    return cResp;
}

/**
Inicia el proceso de programacion
@wHora Valor de la hora
*/
void func_programacion(uint8_t cTipo)
{
    uint8_t cMensaje1, cMensaje2, cPos,cOp;
//		cMenuAct = 3;
    //Inicializacion de plu completa (OP_NO)
    func_plu_ini(TRUE);

		if(cTipo==MENU_AUM_STOCK || cTipo==MENU_DIS_STOCK)
		{
				cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_VALOR; cPos=4;
        cOp=OP_PROG_EXI;
				cStockM=cTipo;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
		}
    //Programacion de fecha
    else if(cTipo==PROGRAMACION_FECHA)
    {
        cMensaje1=MENSAJE_FECHA; cMensaje2=0; cPos=7;
        cOp=OP_PROG_FECHA;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=FALSE;
    }
    //Programacion de hora
    else if(cTipo==PROGRAMACION_HORA)
    {
        cMensaje1=MENSAJE_HORA; cMensaje2=0; cPos=6;
        cOp=OP_PROG_HORA;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=FALSE;
    }
    //Programacion de medios
    else if(cTipo==PROGRAMACION_MEDIOS)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_VALOR; cPos=4;
        cOp=OP_PROG_MEDIOS;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    //Programacion de flags
    else if(cTipo==PROGRAMACION_FLAGS)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_VALOR; cPos=4;
        cOp=OP_PROG_FLAGS;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    //Programacion de PLU
    else if(cTipo==PROGRAMACION_PLU)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_PLU; cPos=4;
        memset(cBufGen,0,sizeof(cBufGen));
        cOp=OP_PROG_PLU;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    //Programacion de Departamento
    else if(cTipo==PROGRAMACION_DEP)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_DEP; cPos=4;
        memset(cBufGen,0,sizeof(cBufGen));
        cOp=OP_PROG_DEP;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    //Programacion de tecla de acceso directo
    else if(cTipo==PROGRAMACION_DIRECTO)
    {
        cMensaje1=MENSAJE_TECLA; cMensaje2=0; cPos=6;
        memset(cBufGen,0,sizeof(cBufGen));
        cOp=OP_PROG_TECLA;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=FALSE;
    }
    //Programacion de cajero
    else if(cTipo==PROGRAMACION_CAJERO)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_CLAVE; cPos=6;
        cOp=OP_PROG_CAJERO;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    //Programacion de clave
    else if(cTipo==PROGRAMACION_CLAVE)
    {
        cMensaje1=MENSAJE_NUMERO; cMensaje2=MENSAJE_CLAVE; cPos=6;
        cOp=OP_PROG_CLAVE;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
    }
    /* Programación Configuración */
    else if(cTipo==PROGRAMACION_CONFIGURACION)
    {
        cMensaje1=MENSAJE_VALOR; cMensaje2=0; cPos=6;
        cOp=OP_PROG_CONFIG;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=FALSE;
    }

    //Actualizacion de estado
    if(cMensaje2==0)func_print_etiqueta(cMensaje1,cMensaje2, cPos, LINEA_SUPERIOR, DISPLAY_OPERADOR);
    else func_print_etiqueta(cMensaje1,cMensaje2, cPos, AMBAS_LINEAS, DISPLAY_OPERADOR);
    tControl.cEst=EST_PROGRAMACION;
    tControl.cOp=cOp;
}



/**
Procesa el estado final del proceso de programacion
@cResp Si el proceso fue exitoso o no
*/
void func_procesar_prog(uint8_t cResp)
{
//    func_salir(TRUE);
	
    if(cResp==TRUE)
    {
        tControl.cEst=EST_ESPERA;
        tControl.cOp=OP_NO;
        //Se regresa al menu de programacion
        menu_mostrar(tSubMenu3,1,LINEA_SUPERIOR);
    }
    else
    {
        func_mensaje_error_ingreso(FALSE);
    }
}

void func_procesar_stock()
{
	
	 uint8_t cResp = TRUE;
	
	 func_plu_procesa_datos();
	
		if(tControl.cOp==OP_PROG_EXI)
    {
			
      int32_t lStockA;
				union{
					int32_t lStockA;
					uint8_t	cStockA[4];
				 } tStock;
							 
			tPluActual.wFin = tIngresoMonto.wEnteros*1000LL;
      tPluActual.wFin += tIngresoMonto.wDecimal;
        
		if (tPluActual.wInicio > 0 && tPluActual.wInicio <= 3000)// 
				{
					cResp&=valida_plu(tPluActual.wInicio);
					if(cStockM==MENU_AUM_STOCK){
					cResp&=MEM_leerInventario(&lStockA, tPluActual.wInicio);						
						
					lStockA=lStockA+tPluActual.wFin;
					cResp&=MEM_escribirInventario(lStockA, tPluActual.wInicio);
					}
					else if(cStockM==MENU_DIS_STOCK){
						cResp&=MEM_leerInventario(&lStockA, tPluActual.wInicio);
							
					
					lStockA=lStockA-tPluActual.wFin;
					cResp&=MEM_escribirInventario(lStockA, tPluActual.wInicio);
					}
			
			    if (cResp != FALSE) 
					{

						mensaje_display(MENSAJE_PROG_EXITO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
          }
					else
					{
						mensaje_display(MENSAJE_PROG_ERROR, DISPLAY_OPERADOR);
						OSTimeDly(1000);
					}
				}
				else{
					func_mensaje_error_ingreso(FALSE);
				}
        //Se muestra el resultado
        func_procesar_prog(cResp);
       
    }
}

void func_prog_fecha(uint8_t cTipo)
{
    uint8_t cPos=0, cResp=TRUE;
    uint64_t wAux;

    if(tIngresoMonto.cContadorEnteros!=tIngresoMonto.cMaxEnteros)cResp=FALSE;

    //Se procesa la informacion del monto
    func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
    wAux = tIngresoMonto.wEnteros;

    if(cTipo==0)cResp&=func_validar_fecha(wAux,1);
    else cResp&=func_validar_hora(wAux);

    if(cResp==TRUE)
    {
        cBufGen[cPos++]='P';
        if(cTipo==0)cBufGen[cPos++]='G';
        else cBufGen[cPos++]='F';
        sprintf((char*)&cBufGen+cPos,"%06lld",wAux);
				
				switch(tConfig.cFlag.bMensajeOk)
				{
					case 0:
						cResp=cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3,SIN_MENSAJE);
						break;
					case 1 :
						cResp=cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3, MENSAJE_CMD_OK);
						break;
				}
    }

    func_procesar_prog(cResp);
}
void func_prog_flag(void)
{
    uint8_t cResp=FALSE;

    //Se procesa la informacion del monto
    func_plu_procesa_datos();
    //Se actualiza la informacion del plu actual
    tPluActual.wFin = tIngresoMonto.wEnteros;

    sprintf((char*)&cBufGen,"PJ%02lld%02lld",tPluActual.wInicio, tPluActual.wFin);

    if(tPluActual.wInicio<63 && tPluActual.wFin<99 && tIngresoMonto.cContadorEnteros!=0)
    {

			switch(tConfig.cFlag.bMensajeOk)
			{
				case 0:
					cResp = cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3, SIN_MENSAJE);
					break;
				case 1 :
					cResp = cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3, MENSAJE_CMD_OK);
					break;
			}
			
    }

    func_procesar_prog(cResp);
}

void func_prog_medio(void)
{
    uint8_t cResp=FALSE,cPos=0;

    if(tPluActual.wIndex>0 && tPluActual.wIndex<=24)
    {
        cBufGen[cPos++]='P';
        cBufGen[cPos++]='E';
        sprintf((char*)&cBufGen+cPos,"%02lld",tPluActual.wIndex);
        cPos+=2;
        strcat((char*)cBufGen+cPos,(char*)tPluActual.cTexto);

				switch(tConfig.cFlag.bMensajeOk)
				{
					case 0:
						cResp = cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3, SIN_MENSAJE);
						break;
					case 1 :
						cResp = cmd_send((uint8_t*)cBufGen,strlen((char*)&cBufGen), 1, 3, MENSAJE_CMD_OK);
						break;
				}
				
    }

    func_procesar_prog(cResp);
}

void func_rep_programacion(void)
{
    uint8_t cResp=FALSE;

    cBufGen[0]='D';

		switch(tConfig.cFlag.bMensajeOk)
		{
			case 0:
				cResp = cmd_send((uint8_t*)cBufGen,1, 1, 3, SIN_MENSAJE);
				break;
			case 1 :
				cResp = cmd_send((uint8_t*)cBufGen,1, 1, 3, MENSAJE_CMD_OK);
				break;
		}

    func_procesar_prog(cResp);
}

void func_prog_plu(void)
{
    static size_t index;
    uint8_t cResp=TRUE;
    tPLU_INFO* pPLU = (tPLU_INFO*)&cBufGen;

    if(tControl.cOp==OP_PROG_PLU)
    {
        index = tPluActual.wIndex;
    }
		
    func_plu_procesa_datos();

    //Se ingreso numero de plu y descripcion
		
    if(tControl.cOp==OP_PROG_PLU)
    {
			if((index > 0 && index < 3000)&& (strlen((char*)tPluActual.cTexto)>0))
			{
        tPluActual.wIndex = tIngresoMonto.wEnteros;
        strcat((char*)pPLU->cDescripcion,(char*)tPluActual.cTexto);

				func_plu_ini(TRUE);
			
				func_print_etiqueta(MENSAJE_PRECIO,MENSAJE_DEP, 5, AMBAS_LINEAS, DISPLAY_OPERADOR);
				tControl.cOp=OP_PROG_PRECIO;
				tSys_flag.bTecAlfabetico=FALSE;
				tSys_flag.bIngresoDoble=TRUE;
				tControl.cEst=EST_PROGRAMACION_PLU;
			
			}
			
			else
			{
				mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
				OSTimeDly(1000);
				tControl.cEst=EST_ESPERA;
				tControl.cOp=OP_NO;
				//Se regresa al menu de programacion
				menu_mostrar(tSubMenu3,1,LINEA_SUPERIOR);
			}
    }
    //Se ingreso precio de plu y codigo
    else if(tControl.cOp==OP_PROG_PRECIO)
    {
				if(tPluActual.wInicio>0 && tIngresoMonto.wEnteros > 0){
					pPLU->wPrecio=tPluActual.wInicio;
					pPLU->cDepAsociado=(uint8_t)tIngresoMonto.wEnteros;
					func_print_etiqueta(MENSAJE_EXISTENCIA,MENSAJE_CODIGO, 6, AMBAS_LINEAS, DISPLAY_OPERADOR);
					tControl.cOp=OP_PROG_PLU_EXI;
					tSys_flag.bTecAlfabetico=FALSE;
					tSys_flag.bIngresoDoble=TRUE;
					tControl.cEst=EST_PROGRAMACION_PLU;
				}
    }
    //Se ingreso tasa y departamento asociado
    else if(tControl.cOp==OP_PROG_PLU_EXI)
    {			
				sprintf((char*)pPLU->cCodigo,"%s",tPluActual.cTexto);

				if ((index > 0 && index < 3000)&&(strlen((char*)pPLU->cCodigo)>0) && (pPLU->cDepAsociado > 0 && pPLU->cDepAsociado <= 99)) 
				{
					cResp = MEM_escribirPLU(pPLU->cValor, index,PLU_PROG);
					if (cResp != FALSE) 
					{	
							MEM_escribirInventario((uint32_t) tPluActual.wInicio,(uint16_t) index);
							mensaje_display(MENSAJE_PROG_EXITO, DISPLAY_OPERADOR);
							OSTimeDly(1000);
					}
					
					else
					{
						mensaje_display(MENSAJE_PROG_ERROR, DISPLAY_OPERADOR);
						OSTimeDly(1000);
					}
				}
					
				else
				{
//						func_mensaje_error_ingreso(FALSE);
//						cResp = FALSE;
					mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
					OSTimeDly(1000);
					tControl.cEst=EST_ESPERA;
					tControl.cOp=OP_NO;
					//Se regresa al menu de programacion
					menu_mostrar(tSubMenu3,1,LINEA_SUPERIOR);
				}
					
        //Se muestra el resultado
        func_procesar_prog(cResp);
    }
}

void func_prog_dep(void)
{
    static size_t index;
    uint8_t cResp = TRUE;
    tDEP_INFO *pDep= (tDEP_INFO *)&cBufGen;

    if(tControl.cOp==OP_PROG_DEP)
    {
        index = tPluActual.wIndex;
    }

    func_plu_procesa_datos();

    if(tControl.cOp==OP_PROG_DEP)
    {
			if((index > 0 && index < 100) && (strlen((char*)tPluActual.cTexto)>0))
			{
        strcat((char*)pDep->cDescripcion,(char*)tPluActual.cTexto);
        func_plu_ini(FALSE);
        func_print_etiqueta(MENSAJE_PRECIO,MENSAJE_PRECIO_MAX, 5, AMBAS_LINEAS, DISPLAY_OPERADOR);
        tControl.cOp=OP_PROG_PRECIO;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
        tControl.cEst=EST_PROGRAMACION_DEP;
			}
			else
			{
				mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
				OSTimeDly(1000);
				tControl.cEst=EST_ESPERA;
				tControl.cOp=OP_NO;
				//Se regresa al menu de programacion
				menu_mostrar(tSubMenu3,1,LINEA_SUPERIOR);
			}
    }
    else if(tControl.cOp==OP_PROG_PRECIO)
    {
        pDep->wPrecioDepartamento=tPluActual.wInicio;
        pDep->wPrecioLimite=tIngresoMonto.wEnteros*100LL + tIngresoMonto.wDecimal;
			
				if(tPluActual.wInicio >0 && pDep->wPrecioLimite > tPluActual.wInicio)
				{
					func_print_etiqueta(MENSAJE_TASA, 0,6, LINEA_SUPERIOR, DISPLAY_OPERADOR);
					tControl.cOp=OP_PROG_PLU_TASAS;
					tSys_flag.bTecAlfabetico=FALSE;
					tSys_flag.bIngresoDoble=TRUE;
					tControl.cEst=EST_PROGRAMACION_DEP;
				}
    }
    else if(tControl.cOp==OP_PROG_PLU_TASAS)
    {
        pDep->cTasa=tIngresoMonto.wEnteros;
				cResp = MEM_escribirDepartamento(pDep->cValor,index);
        func_procesar_prog(cResp);
    }
}

void func_prog_acceso_directo(void)
{
    uint8_t cResp=TRUE;
    tDIRECTO_INFO* pTecla= (tDIRECTO_INFO*)&cBufGen;

    func_plu_procesa_datos();

    if(tControl.cOp==OP_PROG_TECLA)
    {
			if(tIngresoMonto.wEnteros>0 && tIngresoMonto.wEnteros<=16)
			{
        tPluActual.wIndex=(uint8_t)tIngresoMonto.wEnteros;
        func_print_etiqueta(MENSAJE_TIPO, MENSAJE_NUMERO, 6, AMBAS_LINEAS, DISPLAY_OPERADOR);
        tControl.cOp=OP_PROG_TECLA_TIPO;
        tSys_flag.bTecAlfabetico=FALSE;
        tSys_flag.bIngresoDoble=TRUE;
        tControl.cEst=EST_PROGRAMACION_DIR;
			}
    }
    else if(tControl.cEst==EST_PROGRAMACION_DIR)
    {
        pTecla->cTipoTecla=(uint8_t)tPluActual.wInicio;
        pTecla->wNumero_Asociado=(uint8_t)tIngresoMonto.wEnteros;

        if(tPluActual.wIndex<1||tPluActual.wIndex>16)cResp=FALSE;
        if(pTecla->cTipoTecla==0 && (pTecla->wNumero_Asociado>MAX_PLU))cResp=FALSE;
        if(pTecla->cTipoTecla==1 && (pTecla->wNumero_Asociado>MAX_DEP))cResp=FALSE;
					
        tPluActual.wIndex--; //Posicion relativa en el arreglo
	
				cResp&= MEM_escribirAccesoDirecto(pTecla->cValor,tPluActual.wIndex,PLU_PROG);
					
//				if (cResp != FALSE) 
//				{
//					mensaje_display(MENSAJE_PROG_EXITO, DISPLAY_OPERADOR);
//					OSTimeDly(1000);
//				}
//				else
//				{
//					mensaje_display(MENSAJE_PROG_ERROR, DISPLAY_OPERADOR);
//					OSTimeDly(1000);
//				}
//				else{
//					func_mensaje_error_ingreso(FALSE);
//				}
				
        func_procesar_prog(cResp);
    }
}

uint8_t func_actualizar_tasas(void)
{
    char aux[16];

    uint8_t resp;
    if (cmd_send_recv((uint8_t *)"S3", 2, TRUE))
    {
        /* Se Limpia el buffer */
        memset(aux, 0, sizeof(aux));
        /* Se extrae la tasa  1 */
        memcpy(aux, uart.rx_dat + 3, 4);
        /* Se convierte la tasa a entero */
        g_tasas.tasa1 = atoi(aux);
        /* Si la tasa esta incluida se setea el primer bit */
        if (uart.rx_dat[2] == '2')
        {
            g_tasas.tasa1 |= 0x8000U;
        }
        /* Se Limpia el buffer */
        memset(aux, 0, sizeof(aux));
        /* Se extrae la tasa  2 */
        memcpy(aux, uart.rx_dat + 9, 4);
        g_tasas.tasa2 = atoi(aux);
        /* Si la tasa esta incluida se setea el primer bit */
        if (uart.rx_dat[8] == '2')
        {
            g_tasas.tasa2 |= 0x8000U;
        }
        /* Se Limpia el buffer */
        memset(aux, 0, sizeof(aux));
        /* Se extrae la tasa  3 */
        memcpy(aux, uart.rx_dat + 15, 4);
        g_tasas.tasa3 = atoi(aux);
        /* Si la tasa esta incluida se setea el primer bit */
        if (uart.rx_dat[14] == '2')
        {
            g_tasas.tasa3 |= 0x8000U;
        }

        resp = TRUE;
    }
    else
    {
        resp = FALSE;
    }

    UART_BufferRxClear();
    
    return resp;
}

uint8_t func_enq(uint16_t *flags)
{
    if (cmd_send_recv((uint8_t *)"\x05", 1, FALSE) != FALSE)
    {        
        *flags = (uart.rx_dat[0] << 8) | uart.rx_dat[1];
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

uint8_t func_verifica_operacion_fiscal(void)
{
    uint8_t tipo;
    uint8_t resp;
    uint16_t flags;
		wAns =0;
	
    func_enq(&flags);

    if (flags & 0x0100U)    /* Hay una transaccion fiscal abierta? */
    {
        resp = TRUE;        /* Si hay una transaccion fiscal abierta */
        
        if (cmd_send_recv((uint8_t *)"S2", 2, TRUE) != FALSE)
        {

            tipo = uart.rx_dat[74];
					
            UART_BufferRxClear();
            
            switch(tipo)
            {
                case 0x31:
                    tControl.cEst = EST_FACTURA;
                    break;
                case 0x32:
                    tControl.cEst = EST_NOTA_CREDITO;
                    break;
                case 0x33:
                    tControl.cEst = EST_NOTA_DEBITO;
                    break;
                default:
										tControl.cEst = EST_ESPERA;
                    break;                
            }
						memset(cBufGen, 0x00, sizeof(cBufGen));
						memcpy((char *) cBufGen,uart.rx_dat+3,13); // //memcpy(aux, uart.rx_dat + 15, 4);
						wAns=atoll((char *) cBufGen);
         }            
    }
    else
    {

        resp = FALSE;   /* No hay una trasaccion fiscal anierta */
    }
    return resp;
}

//***************FUNCIONES DE REPORTES************************

void func_reporte(uint8_t cTipo)
{
    tArgReportes tCmd={NULL};
		cMenuAct = SUBMENU_X;
		uint8_t cResp = FALSE;
    //Se arma el comando
    memset((char*)&tCmd, 0, sizeof(tCmd));
		
		if(cTipo == MENU_REP_X || cTipo == MENU_REP_Z || cTipo==MENU_REP_X1 || cTipo==MENU_REP_Z1){tCmd.cCmd=CMD_REPORTE;}
		else {tCmd.cCmd=CMD_CLEAR_X2_Z2;} 
			
    if(cTipo==MENU_REP_X) 						//X  : I0X
    {
        tCmd.cSubCmd = '0';
        tCmd.cTipo = 'X';
    }
    else if(cTipo==MENU_REP_X1) 			// X1 . I1X
    {
        tCmd.cSubCmd = '1';
        tCmd.cTipo = 'X';
    }
		else if(cTipo == MENU_ELM_X1){ 		// Elimimar X1 : X1X
			  tCmd.cSubCmd = '1';
        tCmd.cTipo = 'X';
		}
		else if(cTipo==MENU_REP_Z) 				// Z : I0Z
    {
        tCmd.cSubCmd = '0';
        tCmd.cTipo = 'Z';
    }
    else if(cTipo==MENU_REP_Z1) 			//Z1 : I1Z
    {
        tCmd.cSubCmd = '1';
        tCmd.cTipo = 'Z';
    }
		else if(cTipo == MENU_ELM_Z1){ 		// Eliminar Z1 : X1Z
			  tCmd.cSubCmd = '1';
        tCmd.cTipo = 'Z';
		}
			
		switch(tConfig.cFlag.bMensajeOk)
		{
		case 0:
			cResp = cmd_send((uint8_t*)&tCmd,3, 1,1,SIN_MENSAJE);
			break;
		case 1 :
			cResp = cmd_send((uint8_t*)&tCmd,3, 1,1,MENSAJE_CMD_OK);
			break;
		}
		
		// Incidencia 541
		if((cResp == TRUE)&&(cTipo == MENU_ELM_X1|| cTipo== MENU_ELM_Z1))
		{
				mensaje_display(MENSAJE_BORRADO_EXITO, DISPLAY_OPERADOR);
				OSTimeDly(1000);
				func_salir(TRUE);
			
		}
		else if((cResp == FALSE)&&(cTipo == MENU_ELM_X1|| cTipo== MENU_ELM_Z1))
		{
							
				mensaje_display(MENSAJE_BORRADO_ERROR, DISPLAY_OPERADOR);
				OSTimeDly(1000);
				func_salir(TRUE);
			
		}
		
}

void func_habilitar_menu(void)
{
    uint8_t cResp=FALSE;
    
    if (strlen((char *)tIngresoMonto.sEnteros) == 4) {

        func_plu_procesa_datos();

        if(tControl.cOp==OP_CLAVE_S)
        {
            if((uint16_t)tIngresoMonto.wEnteros==tConfig.iClaveS)cResp=TRUE;
        }
        else if(tControl.cOp==OP_CLAVE_Z_EJ)
        {
            if((uint16_t)tIngresoMonto.wEnteros==tConfig.iClaveZ)cResp=TRUE;
        }
				
        else if(tControl.cOp==OP_CLAVE_X)
        {
            if((uint16_t)tIngresoMonto.wEnteros==tConfig.iClaveX)cResp=TRUE;
        }
		
        if(cResp==TRUE)menu_mostrar(MenuActual, 1,LINEA_SUPERIOR);
				else
				{
					func_mensaje_error_ingreso(FALSE);
				}
    }
}

void func_config_leer(void)
{
    mem_read_write(MEM_CONFIG,(uint8_t*)&tConfig.cValor, 0, MODO_LECTURA);
    if(tConfig.cMarca!=MARCA)
    {
        memset((char*)&tConfig,0,sizeof(tConfig.cValor));
        tConfig.cMarca=MARCA;
        mem_read_write(MEM_CONFIG,(uint8_t*)&tConfig.cValor, 0, MODO_ESCRITURA);
    }
}

void func_mensaje_error_ingreso(uint8_t bEstado)
{
	
	mensaje_display(MENSAJE_ERROR_INGRESO, DISPLAY_OPERADOR);
  OSTimeDly(1500);
	
/////////////////////////////////////////////////////////////
	
	if(cMenuAct != NO_SUBMENU)
	{
		//SUBMENU MODO Z
		if(cMenuAct == SUBMENU_Z1)
		{
			menu_mostrar(tSubMenu2, 1, LINEA_SUPERIOR);
		} 
		//SUBMENU MODO S
		else if(cMenuAct == SUBMENU_S1)
		{
			menu_mostrar(tSubMenu3, 1, LINEA_SUPERIOR);
		}
		//SUBMENU MODO EJ
		else if(cMenuAct == SUBMENU_EJ1 || cMenuAct == SUBMENU_EJ2 )
		{
			cMenuAct = SUBMENU_EJ1;
			menu_mostrar(tRangos, 1, LINEA_SUPERIOR);
		}
		
		// SUBMENU BORRAR PLU
		else if(cMenuAct == SUBMENU_S2  ){
			menu_mostrar(tMenuBorrarPLU, 1, LINEA_SUPERIOR);
		}
		// SUBMENU BORRAR ACCESOS	
		else if(cMenuAct == SUBMENU_S3 ){
			menu_mostrar(tMenuBorrarACC, 1, LINEA_SUPERIOR);
		}
		
		else
		{
			menu_principal();
		}
		
	}
///////////////////////////////////////////////////////////		
	
	else{
    func_plu_ini(TRUE);
    func_salir(bEstado);
	}
}

void func_status_impresor(void)
{

    if((tSys_flag.bTerceralinea==1) && (tSys_flag.bMenuAtivo == 0) && (tSys_flag.bMenuOtrosActivo == 0) && (tSys_flag.bErase == 0) && (tSys_flag.bModPC == 0))
    {
        if (tSys_flag.bShiftActivo==1)
        {
            CharToDot_3('A', 0, FONDO_BLANCO);
            CharToDot_3('L', 1, FONDO_BLANCO);
            CharToDot_3('T', 2, FONDO_BLANCO);
        }
        else
        {
            CharToDot_3('A', 0, FONDO_NEGRO);
            CharToDot_3('L', 1, FONDO_NEGRO);
            CharToDot_3('T', 2, FONDO_NEGRO);
        }

        if((tControl.cOp>=OP_INGRESO_ID) && (tControl.cOp<=OP_INGRESO_CONTROL_FO) && (tSys_flag.bMenuAtivo == 0))
        {
            CharToDot_3('A', 4, FONDO_BLANCO);
            CharToDot_3('L', 5, FONDO_BLANCO);
            CharToDot_3('F', 6, FONDO_BLANCO);
        }
        else
        {
            CharToDot_3('A', 4, FONDO_NEGRO);
            CharToDot_3('L', 5, FONDO_NEGRO);
            CharToDot_3('F', 6, FONDO_NEGRO);
        }

        if (tControl.cEst == EST_FACTURA)
        {
            CharToDot_3('V', 8, FONDO_BLANCO);
            CharToDot_3('T', 9, FONDO_BLANCO);
            CharToDot_3('A', 10, FONDO_BLANCO);
        }

        if (tControl.cEst==EST_NOTA_CREDITO)
        {
            CharToDot_3('C', 8, FONDO_BLANCO);
            CharToDot_3('R', 9, FONDO_BLANCO);
            CharToDot_3('E', 10, FONDO_BLANCO);
        }

        if (tControl.cEst==EST_NOTA_DEBITO)
        {
            CharToDot_3('D', 8, FONDO_BLANCO);
            CharToDot_3('E', 9, FONDO_BLANCO);
            CharToDot_3('B', 10, FONDO_BLANCO);
        }
    }
}

void func_verifica_anulacion(void)
{
	uint8_t cResp=FALSE;
	if (strlen((char *)tIngresoMonto.sEnteros) == 4) {
						
		func_plu_procesa_datos();

		if(tControl.cOp==OP_CLAVE_S)
		{
				if((uint16_t)tIngresoMonto.wEnteros==tConfig.iClaveS)cResp=TRUE;
		}
		if(cResp==TRUE)
		{
			// Anulacion Aprobada
			tAns.bDone = 1;
			
			if(tSys_flag.bAnulacion ==TRUE )
			{
					tControl.cOp = OP_NO;
					tControl.cOp = EST_FACTURA;
					tSys_flag.bTerceralinea = 1;
					tDisplay.cPosCaracter = 0;
					display_erase_line(LINEA_SUPERIOR, DISPLAY_OPERADOR);
					display_message_ini();
			}
			else if(tAns.bAnularFactura == 1)
			{
				display_erase_line(LINEA_SUPERIOR, DISPLAY_OPERADOR);
				func_cmd_simple_fac('7'); // Anula Factura
			}
			else if(tAns.bAnularPlu == 1)
			{
				display_erase_line(LINEA_SUPERIOR, DISPLAY_OPERADOR);
				func_cmd_simple_fac('k'); // Anula ultimo PLU
			}	
		}
		else
		{
			func_mensaje_error_ingreso(FALSE); // wplanchez
		}
	}
}
