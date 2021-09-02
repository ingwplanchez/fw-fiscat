#include "system.h"
#include "cmd.h"
#include "uart.h"
#include "display.h"
#include "func_plu.h"
#include "func.h"
#include "Timer.h"
#include "Includes.h"
#include "pc.h"
#include "F25L016A.h"
#include "mem.h"
#include <stdbool.h>

/**
 *
 */
 
 uint8_t pc_version(void)
 {
     char buffer[64];
     sprintf((char *)buffer, "SKY\n%s\n%02d\n%02d\n%02d", FIRMWARE_VERSION_COUNTRY, FIRMWARE_VERSION_MAYOR, FIRMWARE_VERSION_MINOR,HARDWARE_VERSION_ID);
     return cmd_send((uint8_t *)buffer, strlen(buffer), 1, 0, SIN_MENSAJE);
 }
 
uint8_t pc_cmd_guardar_registro(uint8_t cSubCmd, uint8_t *tramacmd, size_t length)
{
    size_t i;
    size_t j;
    size_t k;
    size_t indice;
    uint8_t cResp;
    uint8_t buffer[11];
    uint8_t dummy[128];
		int32_t iCantidad;
    tPLU_INFO *plu;
    tDEP_INFO *dep;
    tDIRECTO_INFO *acceso;
    tMSG_COM *msgcom;

    memset(dummy, 0, sizeof(dummy));

    /* Extrae el indice de plu o departamento o acceso directo (4 bytes) */
    memset(buffer, 0, sizeof(buffer));
    memcpy(buffer, tramacmd, 4);
    indice = atoi((char *)buffer);
    /* Procesa el sub comando */
    switch (cSubCmd)
    {
        case SUBCOMANDO_DEPARTAMENTO:
        {
            dep = (tDEP_INFO *)dummy;
            /* Extrae tasa asociada (2 bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 4, 2);
            dep->cTasa = atoi((char *)buffer);
            /* Extrae precio del departamento (10 bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 6, 10);
            dep->wPrecioDepartamento= atoll((char *)buffer);
            /* Extrae precio limite del departamento (10 Bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 16, 10);
            dep->wPrecioLimite = atoll((char *)buffer);
            /* Extre descripcion del departemento (maximo 40 bytes) */
            for (i = 26, j = 0; i < (length - 1); ++i, ++j) {
                dep->cDescripcion[j] = tramacmd[i];
            }
            if ((strlen((char *)dep->cDescripcion) != 0) && (indice  < 99) && (dep->wPrecioDepartamento < dep->wPrecioLimite))
            {
//                cResp = mem_read_write(MEM_DEPARTAMENTO, dep->cValor, indice, MODO_ESCRITURA);
									cResp = MEM_escribirDepartamento(dep->cValor, indice);
            }
            break;
        }
        case SUBCOMANDO_PRODUCTO:
        {
            plu = (tPLU_INFO *)dummy;
            /* Extrae departamento asociado (4 bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 4, 4);
            plu->cDepAsociado = atoi((char *)buffer);
            /* Extrae precio (10 bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 8, 10);
            plu->wPrecio = atoll((char *)buffer);
					
						// [EXISTENCIA]
						/* Extrae Existencia (9 bytes) */
						memset(buffer, 0, sizeof(buffer));
						memcpy(buffer, tramacmd + 18, 11);
						iCantidad = atoll((char *)buffer);
            /* Extrae codigo plu (maximo 32 bytes opcional)*/
//            i = 18;
						i  = 29;
            if (tramacmd[i] == 0x7C)
            {         
                memset(plu->cCodigo, 0, 31);
                ++i;
                for(j = 0, k = 31; i < length; ++i, ++j, --k)
                {
                    if(tramacmd[i] != 0x7C)
                    {
                        plu->cCodigo[j] = tramacmd[i];
                    }
                    else if (k == 0)
                    {
                        ++i;
                        break;
                    }
                    else
                    {
                        plu->cCodigo[j] = 0;
                        ++i;
                        break;
                    }
                }

            }
            /* Extrae descripcion (maximo 80 bytes */
            for(j = 0; i < (length - 1); ++i, ++j)
            {
                if (tramacmd[i] != 0)
                {
                    plu->cDescripcion[j] = tramacmd[i];
                }
                else
                {
                    break;
                }

            }
            plu->cDescripcion[j] = 0;
						
						// indice para borrar todos los plus
						if(indice == BORRA_LOTE_PLU){
							
							tControl.cEst = EST_BORRAR_TODO_PLU;
							func_print_etiqueta(MENSAJE_BORRADO_TODOS, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
							
							for(uint16_t i = 0; i<=3000; i++)
							{
									func_borrado_total_plu();
							}
							
							tSys_flag.bCmdRecive = TRUE;
							tControl.cEst = EST_NO_OP;
							
							tSys_flag.bModPC = TRUE;
              cResp = TRUE;
              tSys_flag.bTerceralinea = 0;
              tSys_flag.bErase=1;
              display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);
              tSys_flag.bTerceralinea = 0;
              tSys_flag.bErase=1;
              mensaje_display(MENSAJE_MODO_PC, DISPLAY_AMBOS);
							
						}						
						
            if ((strlen((char *)plu->cDescripcion) != 0) && (plu->cDepAsociado < MAX_DEP) && (indice < MAX_PLU))
            {
//          	cResp = mem_read_write(MEM_PLU, plu->cValor, indice, MODO_ESCRITURA);
							cResp = MEM_escribirPLU(plu->cValor, indice,PLU_PROG);
							MEM_escribirInventario(iCantidad,(uint16_t) indice);
            }
						
            break;
        }
        case SUBCOMANDO_TECLA:
        {
            acceso = (tDIRECTO_INFO *)dummy;

            /* Guarda accesso directo */
						
            acceso->wTeclaAsociada = --indice;//acceso->wTeclaAsociada = indice--;
					
            /* Extrae el numero de plu o departamento (4 bytes) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 4, 4);
            acceso->wNumero_Asociado = atol((char *)buffer);
            /* Extrae tipo de acceso directo (1 byte) */
            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, tramacmd + 8, 1);
            acceso->cTipoTecla = atoi((char *)buffer);
						
						// indice para borrar todos los plus
						// Se envia El indice 20: acceso->wTeclaAsociada = --indice y se pregunta por 19
						// Modo Lote: Borra los Accesos directos

						if(indice == BORRA_LOTE_ACC){
							
							tControl.cEst = EST_BORRAR_ACCESOS;
							func_print_etiqueta(MENSAJE_BORRADO_TODOS, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
							
							for(uint16_t i = 0; i<=17; i++)
							{
									func_borrar_accesos();
							}
							
							tSys_flag.bCmdRecive = TRUE;
							tControl.cEst = EST_NO_OP;
							
							tSys_flag.bModPC = TRUE;
              cResp = TRUE;
              tSys_flag.bTerceralinea = 0;
              tSys_flag.bErase=1;
              display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);
              tSys_flag.bTerceralinea = 0;
              tSys_flag.bErase=1;
              mensaje_display(MENSAJE_MODO_PC, DISPLAY_AMBOS);
							
						}
						
						// se envia El indice 18: acceso->wTeclaAsociada = --indice y se pregunta por 17 
						// Modo Lote: Guarda Los primeros Accessos Directos
						if(indice == GUARDA_LOTE_ACC)
						{
							for(uint8_t i = 1;i<=16;i++){

									acceso->wNumero_Asociado = i;
//									acceso->cTipoTecla =0;
									acceso->wTeclaAsociada = i-1;
									cResp&= MEM_escribirAccesoDirecto(acceso->cValor,acceso->wTeclaAsociada,PLU_PROG);
							}
						}
						
            if (indice <= 16) /* TODO faltan las comprobaciones necesarias para le numero de departamento o plu */
            {

							cResp&= MEM_escribirAccesoDirecto(acceso->cValor,acceso->wTeclaAsociada,PLU_PROG);

            }

            break;
        }
        case SUBCOMANDO_MENSAJE_COMERCIAL:
        {
            msgcom = (tMSG_COM *)dummy;
            mem_read_write(MEM_MSG_COMERCIAL, msgcom->cValor, 0, MODO_LECTURA);
            memset(msgcom->cMensaje, 0, 127);
            for (i = 0, j = 126 ; (i < (length - 1)) && (j != 0); ++i, --j)
            {
                msgcom->cMensaje[i] = tramacmd[i];
            }
            msgcom->cMarca = MARCA;
            cResp = mem_read_write(MEM_MSG_COMERCIAL, msgcom->cValor, 0, MODO_ESCRITURA);            
            break;
        }
        default:
        {
            cResp = FALSE;
            break;
        }
    }
    return cResp;
}

/**
 *
 */
uint8_t pc_cmd_leer_registro(uint8_t cSubCmd, uint8_t *tramacmd, size_t length)
{
    size_t i;
    size_t indiceInicial;
    size_t indiceFinal;
    uint8_t cResp;
    uint8_t buffer[128];
    uint8_t dummy[128];
    tPLU_INFO *plu;
    tDEP_INFO *dep;
    tDIRECTO_INFO *dir;
    tMSG_COM *msgcom;
    

    plu = (tPLU_INFO *)dummy;
    dep = (tDEP_INFO *)dummy;
    dir = (tDIRECTO_INFO *)dummy;

    if (cSubCmd != SUBCOMANDO_MENSAJE_COMERCIAL)
    {
        /* Extrae indice inicial de lectura */
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, tramacmd, 4);
        indiceInicial = atoi((char *)buffer);
        /* Extrae indice final de lectura */
        memset(buffer, 0, sizeof(buffer));
        memcpy(buffer, tramacmd + 4, 4);
        indiceFinal = atoi((void *)buffer);

        for (i = indiceInicial; i <= indiceFinal; ++i)
        {
            cResp = FALSE;
            switch (cSubCmd)
            {
                case SUBCOMANDO_DEPARTAMENTO: 
                    memset(dep->cValor, 0, sizeof(dep->cValor));
                    cResp = mem_read_write(MEM_DEPARTAMENTO, dummy, i, MODO_LECTURA);
                    /* Se formatea la respuesta */
                    sprintf((char*)buffer, "%02d|%02d|%010lld|%010lld|%s", i, dep->cTasa, dep->wPrecioDepartamento, dep->wPrecioLimite, dep->cDescripcion);
                    break; 
                case SUBCOMANDO_PRODUCTO: 
                    memset(plu->cValor, 0, sizeof(plu->cValor));
                    cResp = mem_read_write(MEM_PLU, dummy, i, MODO_LECTURA);
										int32_t lStockA;
										MEM_leerInventario(&lStockA,i);
                    /* Se formatea la respuesta */
                    sprintf((char*)buffer, "%04d|%04d|%010lld|%s|%s|%d", i, plu->cDepAsociado, plu->wPrecio, plu->cCodigo, plu->cDescripcion,lStockA);
                    break; 
                case SUBCOMANDO_TECLA: 
                    memset(dir->cValor, 0, sizeof(dir->cValor));
                    cResp = mem_read_write(MEM_ACCESO_DIR, dummy, i - 1, MODO_LECTURA);
                    /* Se formatea la respuesta */
                    sprintf((char*)buffer, "%04d|%04d|%01d", dir->wTeclaAsociada, dir->wNumero_Asociado, dir->cTipoTecla);
                    break;  
                default: 
                    cResp = FALSE;
                    break;
             }
            
            if (cResp == TRUE)
            {
                cmd_send(buffer, strlen((char*)buffer), 1, 0, SIN_MENSAJE);
            }
            else
            {    
                cmd_send((uint8_t *)"NF", 2, 1, 0, SIN_MENSAJE);
            }
         }
    }
    else
    {
        msgcom = (tMSG_COM *)dummy;
        cResp = mem_read_write(MEM_MSG_COMERCIAL, dummy, 0, MODO_LECTURA);
        strcpy((char *)buffer, (char *)msgcom->cMensaje);
        if (cResp == TRUE)
        {
            cmd_send(buffer, strlen((char*)buffer), 1, 0, SIN_MENSAJE);
        }
        else
        {    
            cmd_send((uint8_t *)"NF", 2, 1, 0, SIN_MENSAJE);
        }        
    }
    
    return cResp;
}

