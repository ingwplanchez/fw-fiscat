/**
  ******************************************************************************
  * @file    func_plu.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de procesamiento de operaciones con los plu
  *          
  ******************************************************************************
  */	
	
#ifndef __FUNC_PLU_H
#define __FUNC_PLU_H

#include "system.h"


#pragma anon_unions 
#pragma pack(1)


//******************************************************************************//
//                         DEFINICIONES GENERALES                         			//
//******************************************************************************//


//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//											PROTOTIPOS DE FUNCIONES PUBLICAS												//
//******************************************************************************//
extern void func_plu_limite(void);
extern uint8_t func_check_limite(void);
extern uint8_t func_plu_procesa_datos(void);
extern void func_venta_plu(uint8_t par);
extern void func_plu_ini(uint8_t bCleanPLU);
extern void func_plu_directo(uint8_t iPluDirecto);
extern void func_plu(void);
extern void func_plu2(uint16_t index);
extern uint8_t valida_plu(uint16_t iNumRegistro);
extern void func_departamento(void);
extern void func_plu_consulta(void);
extern void func_correccion(void);
extern uint8_t func_comando_plu();
extern uint8_t func_lista_plu(uint16_t iNumRegistro);

extern void func_pago_total(uint8_t cMedio);
extern void func_pago_parcial(void);
extern void func_plu_monto(void);
extern void plu_func_modo(void);
extern void func_plu_descuento(uint8_t bDescRec, uint8_t bPorMonto);
extern void func_plu_comando_retiro_fondo(void);
extern uint8_t func_plu_borrar(uint16_t poslcd);
extern void func_plu_factura_personalizada(uint8_t *buffletras,uint16_t nrodecaracteres);
extern void func_plu_comando_reporte_historicos(void);
extern void func_plu_posicion_lcd(uint16_t posic);

#endif //END __FUNC_PLU_H
