/* Standard Includes */
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* Application Includes */

#include "config.h"
#include "system.h"
#include "includes.h"
#include "Keyscan.h"
#include "keyboard.h"
#include "display.h"
#include "func_plu.h"
#include "func.h"
#include "mem.h"
#include "cmd.h"

#define PRINT      67

typedef enum {
    EXENTO=0,
    BI_G=1,
    BI_R=2,
    BI_A=3,	
		Bs=4,
		T_PLU=5,
		I_PLU=6,
		T_CANT=7,
		T_COD=8
} Posicion;

typedef enum {
  	T_DEP=5,
		I_DEP=6,
		L_DEP=7
} Posicion2;

const char Datos_Report[][18]={
"(E)","(G)","(R)","(A)","Bs","¡DATOS PLU","PLU","EXISTENCIA","CODIGO"
};

const char Datos_Report2[][22]={
"(E)","(G)","(R)","(A)","Bs","¡DATOS DEPARTAMENTO","DEPARTAMENTO","LIMITE DEPARTAMENTO"};

/* Public Functions ----------------------------------------------------------*/
void Reporte_PLU(void)
{	
		uint8_t cResp = TRUE;
		uint8_t cResp2;
		tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
		tDEP_INFO *pDepAux = (tDEP_INFO *)cBufAux2;
		uint8_t cAux[128] = { NULL };
		uint8_t pos=0;
		
									
	 /* Solo se permite se permite en este estado  */
    if (tControl.cEst != EST_IMP_REPORT_PLU) {
        return;
    }

		switch (tControl.cOp) {
			
      case OP_INICIO:
			
			iContador=iStart;
						
			 /* Se limpia el buffer */
			memset(cBufGen, 0, sizeof(cBufGen));
			/* Se escribe Comando Para Documento No Fiscales. */
			cBufGen[0] = '8';
			cBufGen[1] = '0';
			/* Se Abre el Documento y se Envio en Membrete del Reporte */
			sprintf((char *)(cBufGen + 2), "%s", Datos_Report[T_PLU]);			
			/* Se envia el comando */
			cResp = cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); // Fiscat

			/* Si se envio correctamente se cambia de Estado a Procesando */
			if(cResp==TRUE)
			{
				/* Se cambia de Estado*/	
				tControl.cOp=OP_PROCENSANDO;
				/* Se Inicializa el contador*/		
				cIntentos=0;
			}
			
			/* Sino fue exitoso el envio se cuenta hasta tres intentos para enviar el Inicio  */
			else
			{
				cIntentos++;
				/*Si se supero los intentos se sale de la funcion   */
				if(cIntentos==3)
				{
					cIntentos=0;	
					//Se cambia de estado Para Salir
					tControl.cEst=EST_ESPERA;
					func_salir(TRUE);
								
				}
			}			
      break;
			
			case OP_PROCENSANDO:

				/* Se lee la memoria de trabajo */
				cResp = MEM_leerPLU(pPluAux->cValor, iContador);

				cResp &= MEM_leerDepartamento(pDepAux->cValor, pPluAux->cDepAsociado);
				/* Se Inicia la respuesta de la impresora en TRUE */
				cResp2 = TRUE;
			
				if(cResp==TRUE){
					//==============Se Imprime Indice del PLU y Descripcion del Departamento===================
					/* Se limpia el buffer */
					memset(cBufGen, 0x20, PRINT);
					/* Se escribe Comando Para Documento No Fiscales. */
					cBufGen[0] = '8';
					cBufGen[1] = '0';
					pos=sprintf((char *)(cBufGen + 2), " %s%04d", Datos_Report[I_PLU],iContador);
					cBufGen[pos+2]=' ';	
					strcpy((char *)tPluActual.cDescripcion, (char *)pDepAux->cDescripcion);
					pos=strlen((char *)tPluActual.cDescripcion);
						//Si la Longitud de la Descripcion es mayor de 20, Solo se Reservan 20 espacios para mostrar 
						//sino la reserva la longitud original 
					if(pos>=21)pos=20;
					snprintf((char *)(cBufGen + (PRINT-pos)), 20,"%s", (char *)tPluActual.cDescripcion);	
					/* Se envia el comando */

					cResp2 = cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); // Fiscat

					tControl.cOp=OP_PROCENSANDO;

						//==============Se Imprime Descripcion del PLU , Precio y Tasa===================
					/* Se limpia el buffer */
					memset(cBufGen, 0x20, PRINT);
					/* Se escribe Comando Para Documento No Fiscales. */
					cBufGen[0] = '8';
					cBufGen[1] = '0';
					strcpy((char *)tPluActual.cDescripcion, (char *)pPluAux->cDescripcion);
					tPluActual.cDescripcion[sizeof(tPluActual.cDescripcion)-1] = 0;
					pos=snprintf((char *)(cBufGen + 2), 22," %s", (char *)tPluActual.cDescripcion);
					if(pos>=0x50)
						cBufGen[23]=' ';
					else
					cBufGen[pos+2]=' ';	
					tPluActual.cTasa = pDepAux->cTasa;
						sprintf((char *)cAux, "%s %llu,%02llu %s",Datos_Report[Bs] ,pPluAux->wPrecio / 100llu, pPluAux->wPrecio % 100llu,Datos_Report[tPluActual.cTasa]);
					sprintf((char *)(cBufGen + (PRINT-(strlen((char *)cAux)))), "%s", cAux);	
					/* Se envia el comando */
					cResp2 &= cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE);
					tControl.cOp=OP_PROCENSANDO;
					
					//==============Se Existencia del PLU===================
					/* Se limpia el buffer */
								memset(cBufGen, 0x20, PRINT);
								/* Se escribe Comando Para Documento No Fiscales. */
								cBufGen[0] = '8';
								cBufGen[1] = '0';
								pos=sprintf((char *)(cBufGen + 2), " %s", Datos_Report[T_CANT]);
								cBufGen[pos+2]=' ';	
					
								int32_t lStockA;
								MEM_leerInventario(&lStockA, iContador);
					
								if(lStockA <0)
								{
									lStockA = lStockA *(-1);
									sprintf((char *)cAux, "%-llu,%03llu",lStockA / 1000llu, lStockA % 1000llu);
								}
								else
								{
									sprintf((char *)cAux, "%llu,%03llu",lStockA / 1000llu, lStockA % 1000llu);
								}
								sprintf((char *)(cBufGen + (PRINT-(strlen((char *)cAux)))), "%s", cAux);	
								
//								/* Se envia el comando */
//								cResp2 &= func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
								cResp2 &= cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 3, SIN_MENSAJE);
					
					//==============Se Imprime EL Codigo de Barras del PLU===================
					/* Se limpia el buffer */
					
					memset(cBufGen, 0x20, PRINT);
					/* Se escribe Comando Para Documento No Fiscales. */
					cBufGen[0] = '8';
					cBufGen[1] = '0';
					pos=sprintf((char *)(cBufGen + 2), " %s", Datos_Report[T_COD]);
					cBufGen[pos+2]=' ';	
					strcpy((char *)tPluActual.cCodigo, (char *)pPluAux->cCodigo);
					sprintf((char *)(cBufGen + (PRINT-(strlen((char *)tPluActual.cCodigo)))), "%s", (char *)tPluActual.cCodigo);	
					/* Se envia el comando */
//								cResp2 &= func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
					cResp2 &= cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE);
					tControl.cOp=OP_PROCENSANDO;

					}
					/* Si los envios de los Comandos Fueron TRUE se aumentar el contador para seguir*/
					if(cResp2==TRUE){

						/* Se aumenta el contador del PLU*/
						iContador++;

						/* Se Inicializa el contador*/
						cIntentos=0;	
					}
//					else{iContador++;}
					/* Sino se cuenta hasta tres intentos para enviar el PLU  */
					else{
						cIntentos++;
						/*Si se supero los intentos se avanza al siguiente PLU y se Inicializa el contador  */
						if(cIntentos==3)
						{
							/* Se aumenta el contador del PLU*/
							iContador++;
							cIntentos=0;	
						}
					}

					/* Si llego al ultimo PLU+1 Pasa a Finalizado*/
					if(iContador>iEnd)
					{
						tControl.cOp=OP_FINALIZADO;
					}
					break;
       
				case OP_FINALIZADO:
						/* Se limpia el buffer */
						memset(cBufGen, 0, sizeof(cBufGen));
						/* Se escribe Comando Para Cerrar Documento No Fiscales. */
						cBufGen[0] = '8';
						cBufGen[1] = '1';
												
						/* Se envia el comando */
							cResp = cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE);
				
							/* Si se envio correctamente se sale de Funcion del Reporte */
								if(cResp==TRUE){
								//Se cambia de estado Para Salir
								tControl.cEst=EST_ESPERA;	
								/* Se Sale de la funcion*/
								func_salir(TRUE);
								}
								/* Sino fue exitoso el envio se cuenta hasta tres intentos para enviar el Inicio  */
								else{
								cIntentos++;
										/*Si se supero los intentos se sale de la funcion   */
											if(cIntentos==3)
											{
												//Se cambia de estado Para Salir
												tControl.cEst=EST_ESPERA;	
												/* Se Sale de la funcion*/
												func_salir(TRUE);
											}
								}

            break;
        default:
        break;
    }

}

void Reporte_DEP(void)
{
	
		uint8_t cResp = TRUE;
		uint8_t cResp2;
		tDEP_INFO *pDepAux = (tDEP_INFO *)cBufAux2;
		uint8_t cAux[128] = { NULL };
		uint8_t pos=0;
								
	 /* Solo se permite se permite en este estado  */
    if (tControl.cEst != EST_IMP_REPORT_DEP) {
        return;
    }
		
		switch (tControl.cOp) {
        case OP_INICIO:
					iContador=iStart;
						
					 /* Se limpia el buffer */
					memset(cBufGen, 0, sizeof(cBufGen));
					/* Se escribe Comando Para Documento No Fiscales. */
					cBufGen[0] = '8';
					cBufGen[1] = '0';
					/* Se Abre el Documento y se Envio en Membrete del Reporte */
					sprintf((char *)(cBufGen + 2), "%s", Datos_Report2[T_DEP]);
					
//						/* Se envia el comando */
//						cResp = func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
					cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); 
				
					/* Si se envio correctamente se cambia de Estado a Procesando */
							if(cResp==TRUE){
							/* Se cambia de Estado*/	
							tControl.cOp=OP_PROCENSANDO;
							/* Se Inicializa el contador*/		
							cIntentos=0;
							}
							/* Sino fue exitoso el envio se cuenta hasta tres intentos para enviar el Inicio  */
							else{
							cIntentos++;
									/*Si se supero los intentos se sale de la funcion   */
										if(cIntentos==3)
										{
											cIntentos=0;	
											//Se cambia de estado Para Salir
											tControl.cEst=EST_ESPERA;
											func_salir(TRUE);
										
										}
							}
					
					break;
			case OP_PROCENSANDO:
						
		
						 /* Se lee la memoria de trabajo */
						cResp = MEM_leerDepartamento(pDepAux->cValor, iContador);
			
						 /* Se Inicia la respuesta de la impresora en TRUE */
						 cResp2 = TRUE;
								
							
							if(cResp==TRUE){
								
							
							//==============Se Imprime Indice del Departamento===================
							/* Se limpia el buffer */
							memset(cBufGen, 0x20, PRINT);
							/* Se escribe Comando Para Documento No Fiscales. */
							cBufGen[0] = '8';
							cBufGen[1] = '0';
							pos=sprintf((char *)(cBufGen + 2), " %s", Datos_Report2[I_DEP]);
							cBufGen[pos+2]=' ';	
							snprintf((char *)(cBufGen + (PRINT-2)), 20,"%02d", iContador);	
							/* Se envia el comando */
//								cResp2 = func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
							cResp2 = cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); 
							tControl.cOp=OP_PROCENSANDO;
								//==============Se Imprime Descripcion del DEP, Precio y Tasa===================
							/* Se limpia el buffer */
							memset(cBufGen, 0x20, PRINT);
							/* Se escribe Comando Para Documento No Fiscales. */
							cBufGen[0] = '8';
							cBufGen[1] = '0';
							strcpy((char *)tPluActual.cDescripcion, (char *)pDepAux->cDescripcion);
							tPluActual.cDescripcion[sizeof(tPluActual.cDescripcion)-1] = 0;
							pos=snprintf((char *)(cBufGen + 2), 22," %s", (char *)tPluActual.cDescripcion);
							if(pos>=0x50)
								cBufGen[23]=' ';
							else
							cBufGen[pos+2]=' ';	
							sprintf((char *)cAux, "%s %llu,%02llu %s",Datos_Report[Bs] ,pDepAux->wPrecioDepartamento / 100llu, pDepAux->wPrecioDepartamento % 100llu,Datos_Report[pDepAux->cTasa]);
							sprintf((char *)(cBufGen + (PRINT-(strlen((char *)cAux)))), "%s", cAux);	
							/* Se envia el comando */
//								cResp2 &= func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
							cResp2 &= cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); 
							tControl.cOp=OP_PROCENSANDO;
							//==============Precio Limite del Departamento===================
							/* Se limpia el buffer */
							memset(cBufGen, 0x20, PRINT);
							/* Se escribe Comando Para Documento No Fiscales. */
							cBufGen[0] = '8';
							cBufGen[1] = '0';
							pos=sprintf((char *)(cBufGen + 2), " %s", Datos_Report2[L_DEP]);
							cBufGen[pos+2]=' ';	
							sprintf((char *)cAux, "%s %llu,%02llu",Datos_Report[Bs] ,pDepAux->wPrecioLimite / 100llu, pDepAux->wPrecioLimite % 100llu);
							sprintf((char *)(cBufGen + (PRINT-(strlen((char *)cAux)))), "%s", cAux);	
							/* Se envia el comando */
//								cResp2 &= func_send_cmd((uint8_t *)cBufGen, strlen((char *)cBufGen));
							cResp2 &= cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); 
							tControl.cOp=OP_PROCENSANDO;														
							}
							
							/* Si los envios de los Comandos Fueron TRUE se aumentar el contador para seguir*/
							if(cResp2==TRUE){
							/* Se aumenta el contador del PLU*/
							iContador++;
							/* Se Inicializa el contador*/
							cIntentos=0;	
							}
							/* Sino se cuenta hasta tres intentos para enviar el PLU  */
							else{
							cIntentos++;
									/*Si se supero los intentos se avanza al siguiente PLU y se Inicializa el contador  */
										if(cIntentos==3)
										{
											/* Se aumenta el contador del PLU*/
											iContador++;
							
											cIntentos=0;
										
										}
							}
							
							/* Si llego al ultimo PLU+1 Pasa a Finalizado*/
							if(iContador>100)
								tControl.cOp=OP_FINALIZADO;
															
			
					break;
		 
			case OP_FINALIZADO:
											
					/* Se limpia el buffer */
					memset(cBufGen, 0, sizeof(cBufGen));
					/* Se escribe Comando Para Cerrar Documento No Fiscales. */
					cBufGen[0] = '8';
					cBufGen[1] = '1';
											
					/* Se envia el comando */
					cResp = cmd_send((uint8_t*)cBufGen,strlen((char *)cBufGen), 1, 1, SIN_MENSAJE); 
						/* Si se envio correctamente se sale de Funcion del Reporte */
							if(cResp==TRUE){
							//Se cambia de estado Para Salir
							tControl.cEst=EST_ESPERA;	
							/* Se Sale de la funcion*/
							func_salir(TRUE);
							}
							/* Sino fue exitoso el envio se cuenta hasta tres intentos para enviar el Inicio  */
							else{
							cIntentos++;
									/*Si se supero los intentos se sale de la funcion   */
										if(cIntentos==3)
										{
																			
											//Se cambia de estado Para Salir
											tControl.cEst=EST_ESPERA;	
											/* Se Sale de la funcion*/
											func_salir(TRUE);
										
										}
							}
					break;
			default:
					break;
	}

}

