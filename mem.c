#include "system.h"
#include "cmd.h"
#include "uart.h"
#include "display.h"
#include "func_plu.h"
#include "func.h"
#include "Timer.h"
#include "Includes.h"
#include "mem.h"
#include "F25L016A.h"

#define SECTOR_INVENTARIO 6
#define SECTOR_CONTADOR 5


#define SIZE_BLOCK     0x0100
#define SIZE_SECTOR 	 0x1000

//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//

/*------------------------------------------------------------------------------
 *                             MAPA MEMORIA FLASH
 *--------------+----------+---------+-----+----------+------------+------------
 * DESCRIPCION  | LONG REG | NUM REG | END |  TAMAÑO  |   DIR INI  | DIR FIN
 *--------------+----------+---------+-----+----------+------------+------------
 * PLU          |      128 |    3000 |  32 | 12288000 | 0x00000000 | 0x00BB7FFF
 * Departamento |       64 |      99 |  64 |   405504 | 0x00BB8000 | 0x00C1AFFF
 * Acc Directo  |       16 |      16 | 256 |    65536 | 0x00C1B000 | 0x00C2AFFF
 * Imagen       |      128 |      32 |  32 |   131072 | 0x00C2B000 | 0x00C4AFFF
 * Mens Comerc  |      128 |       1 |  32 |     4096 | 0x00C4B000 | 0x00C4BFFF
 * Conf Usuario |       64 |       1 |  64 |     4096 | 0x00C4C000 | 0x00C4CFFF
 *--------------+----------+---------+-----+----------+------------+------------
 * Total (Bytes)                                                       12898304
 * Total (Hex)                                                       0x00C4D000
 *----------------------------------------------------------------------------*/


/* Definiciones para el manejo de la memoria */
#define FLASH_MAX           0x00F423FF  // la direccion donde empieza el ultimo sector
#define FLASH_MAX_SECTOR    4096        // cantidad de sectores del chip

#define FLASH_PAGE_SIZE     0x000100    // 256 BYTES
#define FLASH_SEC_SIZE      0x000FFF    // 4kbytes

#define FLASH_SEC_START     0x001000    // cada 4k

//Bloque de PLU
#define PLU_LONG            128         // Longitud del bloque de respaldo de certificado
#define PLU_NUM             3000        // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define PLU_WM              0x00000000  // 128 * 3000 * 32 = 12288000
#define PLU_WM_MAX          0x00BB7FFF  // 0x00000000 - 0x00BB7FFF

//Bloque de DEPARTAMENTOS
#define DEPARTAMENTO_LONG   64          // Longitud del bloque de respaldo de certificado
#define DEPARTAMENTO_NUM    99          // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define DEPARTAMENTO_WM     0x00BB8000  // 64 * 99 * 64 = 405504
#define DEPARTAMENTO_WM_MAX 0x00C1AFFF  // 0x00BB8000 - 0x00C1AFFF

//Bloque de Accesos Directos
#define ACCESO_DIR_LONG     16          // Longitud del bloque de respaldo de certificado
#define ACCESO_DIR_NUM      16          // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define ACCESO_DIR_WM       0x00C1B000  // 16 * 16 * 256 = 65536
#define ACCESO_DIR_WM_MAX   0x00C2AFFF  // 0x00C1B000 - 0x00C2AFFF

//Bloque de Imagen
#define IMAGEN_LONG         128         // Longitud del bloque de respaldo de certificado
#define IMAGEN_NUM          32          // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define IMAGEN_WM           0x00C2B000  // 128 * 32 * 32 = 4096
#define IMAGEN_WM_MAX       0x00C4AFFF  // 0x00C2B000 - 0x00C2BFFF

//Bloque de Mensaje Comercial
#define MENSAJE_LONG        128         // Longitud del bloque de respaldo de certificado
#define MENSAJE_NUM         1           // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define MENSAJE_WM          0x00C4B000  // 128 * 32 * 1 = 128
#define MENSAJE_WM_MAX      0x00C4BFFF  // 0x00C2C000 - 0x00C2C07F

//Bloque de Configuraciones de Usuario
#define CONFIG_LONG         64          // Longitud del bloque de respaldo de certificado
#define CONFIG_NUM          1           // Cantidad de bloques disponibles
//Direccion de inicio y fin del bloque
#define CONFIG_WM           0x00C4C000  // 64 * 64 * 1 = 64
#define CONFIG_WM_MAX       0x00C4CFFF  // 0x00C2C080 - 0x00C2C0BF

//*****************************************************************************//

//Bloque Libre 1
#define FREE1_WM					  0x00C4D000  // 
#define FREE1_WM_MAX				0x00C56FFF	// 0x00C4D000 0x00C56FFF = 10 Sectores

//Bloque contador para lista de 128 PLU
#define CONT_LONG           1
#define CONT_WM							0x00C57000  //
#define CONT_WM_MAX         0x00C5BFFF  // 0x00C57000 - 0x00C5BFFF = 5 Sectores

//Bloque PLU en Curso
#define PLU_VENTA_LONG      8
#define PLU_VENTA_WM        0x00C5C000  // 
#define PLU_VENTA_WM_MAX    0x00CDBFFF  // 0x00C5C000 - 0x00CDBFFF = 128 Sectores

//Bloque Libre 2
#define FREE2_WM						0x00CDC000	//
#define FREE2_WM_MAX				0x00CE5FFF	// 0x00CDC000 - 0x00CE5FFF = 10 Sectores

//Bloque Inventario 
#define INVENTARIO_LONG     128         // Longitud del bloque de respaldo de certificado
#define INVENTARIO_WM				0x00CE6000	//
#define INVENTARIO_WM_MAX   0x00F3DFFF  // 0x00CE6000 - 0x00FA1FFF = 600 Sectores

//	Quedan 70 Sectores disponibles

//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//
uint8_t flash_read_cyclic(uint32_t address_sector, uint8_t *data, uint32_t reg_size);
uint8_t flash_write_cyclic(uint32_t address_sector, uint8_t *data, uint32_t reg_size);
uint8_t eep_handle_cycle(uint32_t address, uint8_t * pdata, uint32_t nbytes,uint16_t  nsector,uint8_t func);

//******************************************************************************//
//                                      DEFINICION DE FUNCIONES PUBLICAS                          //
//******************************************************************************//

uint8_t MEM_leerInventario(int32_t *lStock, size_t indice)
{
	
		uint16_t iIndex;
	
    tInventario pInventario = {NULL};

    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_PLU) {
        return FALSE;
    }
		
		iIndex = (indice) - ((indice/30)*30);
		iIndex--;
		
//		UART_sendf("iIndex READ: %lu\n",iIndex);
		
		mem_read_write(MEM_INVENTARIO,pInventario.cValor,(indice-1)/30, MODO_LECTURA);
		
		*lStock=pInventario.lExistencia[iIndex];
		
//		UART_sendf("STOCKL: %lu\n",pInventario.lExistencia[iIndex]);
		
		if(cal_bcc(pInventario.cValor, strlen((char *) pInventario.cValor))==pInventario.cCRC)
			return TRUE;
		else
			return FALSE;

}

uint8_t MEM_leerInventarioPrevio(uint8_t *buffer, size_t indice)
{
	

    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_PLU) {
        return FALSE;
    }
		mem_read_write(MEM_INVENTARIO,buffer,(indice-1)/30, MODO_LECTURA);
		return TRUE;


}

uint8_t MEM_escribirInventario(int32_t lStock, uint16_t indice)
{
//		UART_sendf("STOCKD: %lu\n",lStock);
//		UART_sendf("IndexD = %d", indice);
		uint16_t iIndex;
	
    tInventario pInventario = {NULL};
		MEM_leerInventarioPrevio(pInventario.cValor,indice);	
		
    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_PLU) {
        return FALSE;
    }
		
		iIndex = (indice) - ((indice/30)*30);
		iIndex--;
		
//				UART_sendf("iIndex Write: %lu\n",iIndex);
						
//				if(lStock<0)
//				UART_sendf("StockE: - %lu\n",(-1)*lStock);
//				else
//				UART_sendf("StockE: %lu\n",lStock);	
		
		pInventario.lExistencia[iIndex]=lStock;
		pInventario.cCRC=cal_bcc(pInventario.cValor, strlen((char *) pInventario.cValor));
		
		return mem_read_write(MEM_INVENTARIO,pInventario.cValor,(indice-1)/30, MODO_ESCRITURA);

}


uint8_t MEM_escribirVENTA(uint8_t *buffer, size_t indice, uint8_t MARK)
{
	uint8_t resp = FALSE;
	tPLU_VENTA *pPluVenta = (tPLU_VENTA *)buffer;
	
	pPluVenta->cMarca = MARK;
	
	resp = mem_read_write(MEM_PLU_VENTA,buffer,indice, MODO_ESCRITURA);
	
	return resp;
}

uint8_t MEM_leerVENTA(uint8_t *buffer, size_t indice)
{
	uint8_t resp = FALSE;
	tPLU_VENTA *pPluVenta = (tPLU_VENTA *)buffer;

	tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
	tDEP_INFO *pDepAux = (tDEP_INFO *)cBufAux2;
	
	resp = mem_read_write(MEM_PLU_VENTA,buffer,indice, MODO_LECTURA);
	
	if(resp == TRUE && pPluVenta->cMarca == PLU_PROG) 
	{
		tPluActual.iIndex = pPluVenta->iIndex;	
		tPluActual.cMarca = pPluVenta->cMarca;
		tPluActual.wCantidad = pPluVenta->wCantidad;
    tPluActual.wPrecio = pPluAux->wPrecio;
		
		if(pPluVenta->cTipo == 0)
		{
			MEM_leerPLU(pPluAux->cValor,tPluActual.iIndex);
			strcpy((char *)tPluActual.cDescripcion, (char *)pPluAux->cDescripcion);
			tPluActual.wPrecio = pPluAux->wPrecio;
		}
		else
		{
			MEM_leerDepartamento(pDepAux->cValor,tPluActual.iIndex);
			strcpy((char *)tPluActual.cDescripcion, (char *)pDepAux->cDescripcion);
	    tPluActual.wPrecio = pDepAux->wPrecioDepartamento;
		}
			
		return TRUE;
  }
	
  else
	{
    return FALSE;
  }
}

uint8_t MEM_escribirCONTADOR(uint8_t *buffer, size_t indice)
{
 	uint8_t resp = FALSE;
	resp = mem_read_write(MEM_PLU_CONT,buffer,indice, MODO_ESCRITURA);
	return resp;
}

uint8_t MEM_leerCONTADOR(uint8_t *buffer, size_t indice)
{
	uint8_t resp = FALSE;
	resp = mem_read_write(MEM_PLU_CONT,buffer,indice, MODO_LECTURA);
	
	cCONT = *buffer;
	return resp;
}

//==============================================================================

uint8_t MEM_escribirPLU(uint8_t *buffer, size_t indice, uint8_t MARK)
{
    tPLU_INFO *pPluInfo = (tPLU_INFO *)buffer;

    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_PLU) {
        return FALSE;
    }
    pPluInfo->cMarca = MARK;
		return mem_read_write(MEM_PLU,buffer,indice, MODO_ESCRITURA);
}

uint8_t MEM_leerPLU(uint8_t *buffer, size_t indice)
{
    uint8_t resp = FALSE;
    tPLU_INFO *pPluInfo = (tPLU_INFO *)buffer;
	
    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_PLU) {
        return FALSE;
    }
		
		resp = mem_read_write(MEM_PLU,buffer,indice,MODO_LECTURA);

    if (resp == TRUE && pPluInfo->cMarca == PLU_PROG) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

uint8_t MEM_escribirAccesoDirecto(uint8_t *buffer, size_t indice,uint8_t MARK)
{

    tDIRECTO_INFO *pDirectoInfo = (tDIRECTO_INFO *)buffer;

//    if (0 == indice || indice > MEM_MAX_ACC) {
//        return FALSE;
//    }
		
    pDirectoInfo->cMarca = MARK;
    return mem_read_write(MEM_ACCESO_DIR, buffer, indice, MODO_ESCRITURA);
}

uint8_t MEM_leerAccesoDirecto(uint8_t *buffer, size_t indice)
{
    uint8_t resp = FALSE;
    tDIRECTO_INFO *pDirectoInfo = (tDIRECTO_INFO *)buffer;	
	
//    if (0 == indice || indice > MEM_MAX_ACC) {
//        return FALSE;
//    }
	
    resp = mem_read_write(MEM_ACCESO_DIR, buffer,indice, MODO_LECTURA);
	
    if (resp == TRUE && pDirectoInfo->cMarca == PLU_PROG) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

uint8_t MEM_escribirDepartamento(uint8_t *buffer, size_t indice)
{
    tDEP_INFO *pDepInfo = (tDEP_INFO *)buffer;
    
    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_DEP) {
        return FALSE;
    }

    pDepInfo->cMarca = MARCA;
    return mem_read_write( MEM_DEPARTAMENTO,buffer, indice, MODO_ESCRITURA);	
}


uint8_t MEM_leerDepartamento(uint8_t *buffer, size_t indice)
{
    uint8_t resp = FALSE;
    tDEP_INFO *pDepInfo = (tDEP_INFO *)buffer;

    /* No se permiten indices fuera del intervalo */
    if (0 == indice || indice > MEM_MAX_DEP) {
        return FALSE;
    }

    resp = mem_read_write( MEM_DEPARTAMENTO,buffer,indice, MODO_LECTURA);
		
    if (resp == TRUE && pDepInfo->cMarca == MARCA) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

/**
Guarda y lee un plu en la tabla de plus de la memoria
@iNumero      Indice del plu
@cBuf           Buffer origen con la informacion a guardar
@bReadWrite Indicador de Lectura o Escritura
@retval         Resultado de la operacion (TRUE - FALSE)
*/
uint8_t mem_read_write(uint8_t cTipo, uint8_t *cBuf, uint16_t iIndex, uint8_t bReadWrite)
{
    uint8_t cResp=FALSE;
    uint8_t cAux[129]={NULL};
    uint32_t lDirmin, lDirmax, iReglen, iEstrucLen, lDir;

    if(cTipo==MEM_PLU)//PLU
    {
        lDirmin=PLU_WM;lDirmax=PLU_WM_MAX;iReglen=PLU_LONG;
        iEstrucLen = PLU_LONG;
    }
    else if(cTipo==MEM_DEPARTAMENTO)//departamento
    {
        lDirmin=DEPARTAMENTO_WM;lDirmax=DEPARTAMENTO_WM_MAX;iReglen=DEPARTAMENTO_LONG;
        iEstrucLen = DEPARTAMENTO_LONG;
    }
    else if(cTipo==MEM_ACCESO_DIR)//acceso directo
    {
        lDirmin=ACCESO_DIR_WM;lDirmax=ACCESO_DIR_WM_MAX;iReglen=ACCESO_DIR_LONG;
        iEstrucLen=ACCESO_DIR_LONG;
    }
    else if(cTipo==MEM_IMAGEN)//imagen
    {
        lDirmin=IMAGEN_WM;lDirmax=IMAGEN_WM_MAX;iReglen=IMAGEN_LONG;
        iEstrucLen=IMAGEN_LONG;
    }
    else if(cTipo==MEM_CONFIG)//Configuraciones
    {
        lDirmin=CONFIG_WM;lDirmax=CONFIG_WM_MAX;iReglen=CONFIG_LONG;
        iEstrucLen=CONFIG_LONG;
    }
    else if(cTipo==MEM_MSG_COMERCIAL)//Configuraciones
    {
        lDirmin=MENSAJE_WM;lDirmax=MENSAJE_WM_MAX;iReglen=MENSAJE_LONG;
        iEstrucLen=MENSAJE_LONG;
    }
		else if(cTipo==MEM_INVENTARIO) //Inventario
		{
				lDirmin=INVENTARIO_WM;lDirmax=INVENTARIO_WM_MAX;iReglen=INVENTARIO_LONG;
        iEstrucLen=INVENTARIO_LONG;
		}
		
		else if(cTipo == MEM_PLU_VENTA) // PLU Vendido
		{
			lDirmin=PLU_VENTA_WM;lDirmax=PLU_VENTA_WM_MAX;iReglen=PLU_VENTA_LONG;
      iEstrucLen=PLU_VENTA_LONG;
		}
			
//		else if(cTipo == MEM_PLU_CONT) // Contador para la lista de 128 PLUS Vendidos
//		{
//			lDirmin=CONT_WM;lDirmax=CONT_WM_MAX;iReglen=CONT_LONG;
//      iEstrucLen=CONT_LONG;
//		}

    //Se calcula la direccion del sector principal
		if(cTipo==MEM_INVENTARIO){
			lDir= lDirmin + ((uint32_t)(iIndex*SECTOR_INVENTARIO)*FLASH_SEC_START);
		}
//		else if(cTipo == MEM_PLU_CONT){
//			lDir= lDirmin + ((uint32_t)(iIndex*SECTOR_CONTADOR)*FLASH_SEC_START);
//		}
		else{
			lDir= lDirmin + ((uint32_t)iIndex*FLASH_SEC_START);
		}
    if(lDir>lDirmax)return FALSE;

    memset(cAux, 0, sizeof(cAux)); //memset(cAux, 0, 129);
		
    if(bReadWrite==0)
    {
        //Se lee el bloque
				if(cTipo==MEM_INVENTARIO){
					cResp=eep_handle_cycle(lDir, cAux, iReglen,SECTOR_INVENTARIO,'R');
        }
//				else if(cTipo == MEM_PLU_CONT){
//					cResp=eep_handle_cycle(lDir, cAux, iReglen,SECTOR_CONTADOR,'R');
//				}
				else{
					cResp=flash_read_cyclic(lDir, cAux, iReglen);
				}
        memcpy(cBuf, cAux, iEstrucLen);

    }
    else
    {
        memcpy(cAux, cBuf, iEstrucLen);
			
        //Se escribe el bloque
				if(cTipo==MEM_INVENTARIO){
					cResp=eep_handle_cycle(lDir, cAux, iReglen,SECTOR_INVENTARIO,'W');
        }
//				else if(cTipo == MEM_PLU_CONT){
//					cResp=eep_handle_cycle(lDir, cAux, iReglen,SECTOR_CONTADOR,'W');
//				}
				else{
					cResp=flash_write_cyclic(lDir, cAux, iReglen);
				}
    }

    return cResp;
}

//******************************************************************************//
//                                                  DEFINICION DE FUNCIONES  PRIVADAS                   //
//******************************************************************************//

//***************RUTINAS DE MANEJO DE BUFFER CICLICO************************//

/**
Funcion que maneja la lectura de un registro en un buffer ciclico de un sector
@address_sector Direccion del sector
@data Puntero al buffer donde se copiara la informacion leida
@reg_size  Tamaño del registro en el buffer ciclico
*/
uint8_t flash_read_cyclic(uint32_t address_sector, uint8_t *data, uint32_t reg_size)
{
    uint8_t resp;
    uint8_t aux_reg[128]; //El tamaño maximo del registro sera 128
    uint32_t add_max;
    uint32_t num_sector;
    uint32_t aux_address;

    //Se define y valida el sector a leer
    num_sector = (address_sector/FLASH_SEC_START);
    if(num_sector>FLASH_MAX_SECTOR)return FALSE;
    //Se define la direccion de inicio y fin del sector
    aux_address = address_sector;
    add_max=address_sector+FLASH_SEC_START;

    //Se busca el ultimo registro escrito
    resp=FALSE;
    while(aux_address<add_max)
    {
        memset(aux_reg,0xFF,sizeof(aux_reg));
        //verifico si este sector esta vacio
        resp = F25L_BuffCmpCanWrite((uint32_t)aux_address, (uint8_t *)aux_reg, (uint16_t)reg_size);
        if(resp == TRUE)
        {
            //Si el primer sector esta vacio es porque no hay data
            if(aux_address==address_sector)
            {
                memset((uint8_t*)data, 0, (uint16_t)reg_size);
                return FALSE;
            }
            else
            {
                //Si no es el primer sector la data esta en el anterior
                aux_address=aux_address-reg_size;
                break;
            }
        }
        else
        {
                aux_address=aux_address+reg_size;
        }

        //si es el ultimo registro del sector
        if(aux_address==add_max)
        {
            aux_address=aux_address-reg_size;
            break;
        }
    }

    F25L_Read(aux_address, data, (uint16_t)reg_size);

    return TRUE;
}

/**
Funcion que maneja la lectura de un registro en un buffer ciclico de un sector
@address_sector Direccion del sector
@data Puntero al buffer donde se copiara la informacion leida
@reg_size  Tamaño del registro en el buffer ciclico
*/
uint8_t flash_write_cyclic(uint32_t address_sector, uint8_t *data, uint32_t reg_size)
{
    uint8_t resp;
    uint8_t aux_reg[128];
    uint32_t add_max;
    uint32_t num_sector;
    uint32_t aux_address;

    //la direccion no es mayor que la maxima fisica del chip
    if(address_sector>FLASH_MAX)return FALSE;

    //defino el sector que quiero escribir
    num_sector = (address_sector/FLASH_SEC_START);
    #ifdef _DEBUGG_MEM
////    sprintf((char *)aux_reg,"sector %d \r\n", num_sector);
////    UART_send(aux_reg, strlen((char*)aux_reg));
    #endif

    //el sector calculado no es mayor que el maximo fisico del chip
  if(num_sector>FLASH_MAX_SECTOR)return FALSE;

    aux_address = address_sector;
    add_max=address_sector+FLASH_SEC_START;

    resp=FALSE;

    //Se busca el siguiente registro disponible en el sector
    while(aux_address<add_max)
    {
        memset(aux_reg,0xFF,sizeof(aux_reg));
        resp = F25L_BuffCmpCanWrite((uint32_t)aux_address, (uint8_t *)aux_reg, (uint32_t)reg_size);
        if(resp == TRUE)break;
        else aux_address=aux_address+reg_size;
    }

    //si la respuesta es falsa es que el sector entero esta lleno
    //debo borrar el sector y escribir en la primera posicion
    if(resp==FALSE)
    {
        aux_address = address_sector;
        //borro el sector completo, se borran 4kbytes
        F25L_SectorErase(num_sector);
    }

    //Se verifica de nuevo
    resp = F25L_BuffCmpCanWrite((uint32_t)aux_address, (uint8_t *)data, (uint32_t)reg_size);
    //si tengo un sector disponible
    if(resp==TRUE)
    {
        //Las direcciones deben coincidir para que siempre se este dentro de la pagina
        resp = F25L_BuffProgram(aux_address, data, (uint32_t)reg_size);
    }

    return resp;
}

uint8_t eep_handle_cycle(uint32_t address, uint8_t * pdata, uint32_t nbytes,uint16_t  nsector,uint8_t func)
{
    uint8_t resp = TRUE;
    uint8_t cont_reg;
    uint8_t block_ram[SIZE_BLOCK];// Tamaño del buffer de comparacion .    
#ifdef _DEBUGG_MEM
    uint8_t aux_reg[128];
#endif
	
	
    uint32_t l = 0,i = 0;
    uint8_t control_page = TRUE;
    uint8_t flag_erase_sector = FALSE;
    uint32_t cont_page = 0;
    uint32_t erase_sector = 0;
    uint32_t addressAux;
    //timer_ticks = 0;
    if((nbytes<8)||((nbytes%8)!=0)) return FALSE;

    while(control_page)
    {
        memset(block_ram,0xFF,SIZE_BLOCK);//fijo buffer auxiliar para comparacion.
    
        //Comparo la cabecera del sector de 4Kbyte.
        l = F25L_BuffCmpCanWrite(address+cont_page,(uint8_t *)block_ram,nbytes);

        //Si la página no esta vacia busco los registros en ese sector.
        if(l == 0)
        {
            cont_reg = SIZE_SECTOR/nbytes; //Calculo la cantidad de Registros que se puede escribir en el sector.

            memset(block_ram,0xFF,SIZE_BLOCK);//fijo buffer auxiliar para comparacion.

            for(i = 0; i < cont_reg;i++)
            {
                //Calculo la direccion a comparar, se compara con nBytes
                //Direccion = DIRECCION_PRINCIPAL + (INDICE_REGISTRO*CANTIDAD_DE_BYTES) + CONTADOR_DE_PAGINA
                addressAux = address+(i*nbytes)+cont_page;            
                //Comparo si esta limpio el sector.
                if(F25L_BuffCmpCanWrite(addressAux,(uint8_t *)block_ram,nbytes)==1) break;            
            }

            //** Si el indice del registro es mayor a la cantidad de registros posibles no hay espacio en la pagina
            if(i >= cont_reg)
            {
                if((func == 'W') || (func == 'w'))
                {        
                    flag_erase_sector = TRUE;//Flag que indica que debe hacerse un boorrado de pagina... 
										erase_sector = (address+cont_page)/FLASH_SEC_START;
//                    erase_sector = address+cont_page;//Respaldo la direccion de la pagina a borrar.
                    
                    cont_page+=SIZE_SECTOR; //Cambio la direccion a el inicio de la siguiente aumentado la direccion 4Kbyte.             

                    if((cont_page/SIZE_SECTOR)==nsector) cont_page = 0; // Si se sobre pasa al limite de páginas de ciclado        
                                                                           //comienzo desde la direccion inicial.
                    i = 0;//si resetea el indice.
                }
                else if(i>0) i--; //Si esta en modo lectura y llego al ultimo registro el indice se disminuye para que tome 
            }                      //el ultimo registro.
            else if((func == 'R') || (func == 'r'))
            {
                if(i>0) i--;//disminuyo el indice del registro.
            }

            addressAux = address+cont_page+(uint32_t)(nbytes*i);//Calculo la direccion
                
            if((func == 'W') || (func == 'w'))
            {                 
                if(F25L_BuffProgram(addressAux,(uint8_t *)pdata,nbytes)== FALSE)
                {    //Escribo la data.
                
                    #ifdef _DEBUGG_MEM                           
										sprintf((char *)aux_reg,"Error Write Serial Flash \r\n");
//										UART_send(aux_reg, strlen((char*)aux_reg));
                    #endif
								
                    
                     resp = FALSE; 
                }
                else
                {
                    //Se Borra el sector si se activa
                    if(flag_erase_sector == TRUE)
                    {
                        //Agregar despues de escribir.
                        F25L_SectorErase(erase_sector);// Limpio el sector activo.

                        #ifdef _DEBUGG_MEM  
                  sprintf((char *)aux_reg,"Erase Block \r\n");
//									UART_send(aux_reg, strlen((char*)aux_reg));
                        #endif
                    
                    }
										else
										{
										  sprintf((char *)aux_reg,"Not Erase Block \r\n");
//											UART_send(aux_reg, strlen((char*)aux_reg));
										}	
											
                    
                    resp = TRUE;
                }
                
                #ifdef _DEBUGG_MEM  
                    sprintf((char*) aux_reg,"SectorWrite =%04d, Reg =%04d,add =%08X",(uint32_t)(cont_page/SIZE_SECTOR),i,addressAux);
//										UART_send(aux_reg, strlen((char*)aux_reg));
                #endif
            }
            else if((func == 'R') || (func == 'r'))
            {
                F25L_Read(addressAux,(uint8_t *)pdata,nbytes);//Leo la data.
                resp = TRUE;

                #ifdef _DEBUGG_MEM 
                    sprintf((char*) aux_reg,"Sector Read = %04d, Reg = %04d, add = %08X",(uint32_t)(cont_page/SIZE_SECTOR),i,addressAux);
//                    UART_send(aux_reg, strlen((char*)aux_reg));
                #endif
            }

            control_page = FALSE;//Finalizo la escritura del ciclo.

        }
        else
        {
            //** Si la página esta vacia busco la siguiente pagina.            
            cont_page+=SIZE_SECTOR;
            // Si se recorre todas las paginas de ciclado y todas estan limpias es primera vez que se usan
            // entonces escribo en la primera direccion del primer sector asignado.
            if((cont_page/SIZE_SECTOR)==nsector)
            {
                
                if((func == 'W') || (func == 'w'))
                {
                    if(F25L_BuffProgram(address,(uint8_t *)pdata,nbytes)== FALSE)//Escribo en el primer valor del sector
                    {
                         resp = FALSE;
                    }
                    else resp = TRUE;
                    
                     #ifdef _DEBUGG_MEM    
               		sprintf((char *)aux_reg,"Write Firts Reg - Addr = %08X - %d\r\n", address, resp);
//									UART_send(aux_reg, strlen((char*)aux_reg));
                     #endif
                }
                else if((func == 'R') || (func == 'r'))
                {
                     F25L_Read(address,(uint8_t *)pdata,nbytes);//Leo el primer valor del sector
                     resp = TRUE;

                     #ifdef _DEBUGG_MEM                            
									sprintf((char *)aux_reg,"Read Firts Reg - Addr = %08X\r\n", address);
//									UART_send(aux_reg, strlen((char*)aux_reg));
                     #endif
                }
                
                #ifdef _DEBUGG_MEM 
                    sprintf((char*) aux_reg,"Page Address Check Blank = %08X",(uint32_t)cont_page);
//                    UART_send(aux_reg, strlen((char*)aux_reg));
                #endif    
                control_page = FALSE;//Finalizo la escritura del ciclo.
            }
        }
    }

    return resp;
}


