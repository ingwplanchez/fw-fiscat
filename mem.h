/**
  ******************************************************************************
  * @file    mem.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de manejo de memoria
  *
  ******************************************************************************
  */

#ifndef __MEM_H
#define __MEM_H

#include "system.h"
#include "cmd.h"

//Modo de operación
#define MODO_ESCRITURA 1
#define MODO_LECTURA  0

#define _DEBUGG_MEM  0

//Definiciones de modo de guardado en memoria
#define MEM_PLU             0
#define MEM_DEPARTAMENTO    1
#define MEM_ACCESO_DIR      2
#define MEM_IMAGEN          3
#define MEM_CONFIG          4
#define MEM_MSG_COMERCIAL   5
#define MEM_INVENTARIO      6
#define MEM_PLU_VENTA       7
#define MEM_PLU_CONT        8

#define MEM_MAX_DEP 100
#define MEM_MAX_PLU 3000
#define MEM_MAX_ACC 16

//******************************************************************************//
//                         DEFINICIONES GENERALES                                //
//******************************************************************************//


//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PUBLICAS    //
//******************************************************************************//

extern uint8_t mem_read_write(uint8_t cTipo, uint8_t *cBuf, uint16_t iIndex, uint8_t bReadWrite);
extern uint8_t MEM_escribirPLU(uint8_t *buffer, size_t indice, uint8_t MARK);
extern uint8_t MEM_leerPLU(uint8_t *buffer, size_t indice);
extern uint8_t MEM_escribirAccesoDirecto(uint8_t *buffer, size_t indice,uint8_t MARK);
extern uint8_t MEM_leerAccesoDirecto(uint8_t *buffer, size_t indice);
extern uint8_t MEM_escribirDepartamento(uint8_t *buffer, size_t indice);
extern uint8_t MEM_leerDepartamento(uint8_t *buffer, size_t indice);
extern uint8_t MEM_escribirInventario(int32_t lStock, uint16_t indice);
extern uint8_t MEM_leerInventario(int32_t *lStock, size_t indice);

extern uint8_t MEM_escribirVENTA(uint8_t *buffer, size_t indice, uint8_t MARK);
extern uint8_t MEM_leerVENTA(uint8_t *buffer, size_t indice);
extern uint8_t MEM_escribirCONTADOR(uint8_t *buffer, size_t indice);
extern uint8_t MEM_leerCONTADOR(uint8_t *buffer, size_t indice);
#endif      // __CMD_RECIBIDO_PC_H__
