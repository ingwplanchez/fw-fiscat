/*******************************************************************************
  * @file    cmd_recibido_pc.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de manejo de memoria 
  *          
  ******************************************************************************
  */    
    
#ifndef __PC_H
#define __PC_H

#include "system.h"

#define REGISTROLEN                         0X100
#define COMANDO_GUARDAR                     0X50
#define COMANDO_LECTURA                     0X55
#define SUBCOMANDO_PRODUCTO                 0X50
#define SUBCOMANDO_DEPARTAMENTO             0X44
#define SUBCOMANDO_TECLA                    0X54
#define SUBCOMANDO_MENSAJE_COMERCIAL        'M'
#define COMANDO_VERSION                  		'V'
#define BORRA_LOTE_PLU                     3002
#define BORRA_LOTE_ACC                     19
#define GUARDA_LOTE_ACC                    17

#pragma anon_unions //Define para poder usar estructuras y uniones anonimas.
#pragma pack(1)


//****************************************************************************/
//                         DEFINICIONES GENERALES                             //
//****************************************************************************//
uint8_t pc_cmd_guardar_registro(uint8_t cSubCmd, uint8_t *tramacmd, size_t length);
uint8_t pc_cmd_leer_registro(uint8_t cSubCmd, uint8_t *tramacmd, size_t length);
uint8_t pc_version(void);
    
#endif      // __PC_H__
