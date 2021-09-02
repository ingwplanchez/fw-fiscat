/**
  ******************************************************************************
  * @file    func.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de procesamiento de comandos generales
  *          
  ******************************************************************************
  */	
	
#ifndef __FUNC_H
#define __FUNC_H

#include "system.h"


#pragma anon_unions //Define para poder usar estructuras y uniones anonimas.
#pragma pack(1)


//******************************************************************************//
//                         DEFINICIONES GENERALES                         			//
//******************************************************************************//
typedef struct
{
	uint8_t cCmd;
  uint8_t cSubCmd;
  uint8_t cTipo; 
}tArgReportes;


//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//											PROTOTIPOS DE FUNCIONES PUBLICAS												//
//******************************************************************************//

//FUNCIONES DE VENTAS
extern void func_nota_deb_cred(uint8_t cOp);
extern void func_nota_debito(void);
extern void func_nota_credito(void);
extern void func_subtotal(void);
extern void func_anular_factura(void);
extern void func_anular_plu(void);
extern void func_tecla_x(void);
extern void func_price(void);
extern void func_cmd_simple_fac(uint8_t cCmd);

//FUNCIONES DE PAGOS DIRECTOS Y MENU PAGOS
extern void func_pago_directo0(void);
extern void func_pago_directo1(uint8_t cTipo);
extern void func_pago_directo2(void);
extern void func_pago_directo3(void);
extern void func_pago_directo4(void);
extern void func_pago_menu(void);
extern void func_pago_lista(uint8_t cTipo);
extern void func_pago_total_menu(void);

//FUNCIONES TECLADO
extern void func_evento_teclado_numerico(uint8_t numero,uint16_t bdoblecero);
extern void func_ejecutar(void);
extern void func_tecla_shift(void);
extern void func_tecla_borrar(void);
extern void func_salir(uint8_t bEstado);
extern void func_descuento_menu(uint8_t cTipo);
extern void func_descuento_porcentaje(void);
extern void func_descuento_monto(void);
extern void func_descuento(void);
extern void func_recargo(void);
extern void func_fondo_retiro_menu(uint8_t cTipo);
extern void func_retiro_caja(void);
extern void func_fondo_caja(void);
extern void func_fin_fondo_retiro(void);
extern void func_fondo_retiro(uint8_t cTipo);
extern void func_hora_fecha(void);
extern void func_personalizar_factura(void);
extern void func_cajero_menu(void);
extern void func_activar_cajero(void);
extern void func_prog_cajero(void);
extern void func_reimpresion_ultimo(void);
extern void func_apertura_gaveta(void);
extern void func_prog_clave(void);
extern void func_prog_config(void);
extern void func_prog_mensaje(void);
extern void func_status_impresor(void);

//FUNCIONES DE MODO CALCULADORA
extern void func_calculadora(void);
extern void func_operacion_calculadora(void);
extern void func_mult(void);
extern void func_div(void);
extern uint64_t Divisiones (uint64_t *wDividendo, uint64_t *wDivisor);
extern void func_suma(void);
void func_calc_clean(void);
extern void func_resta(uint64_t wNum1,uint64_t wNum2);
extern void func_mult_sum(void);
//void tostring(char str[], long long int num);
extern void func_op_calculadora(void);
extern void func_op_calc(uint8_t operacion);

extern void func_procesar_stock();

//FUNCIONES DE MODO MENU
extern void func_reporte(uint8_t cTipo);

extern void func_estatus_EJ(void);
extern void func_reimpresion_lista(void);
extern void func_teclas_direc_up(void);
extern void func_teclas_direc_down(void);
extern void func_menu(void);
extern void func_habilitar_menu(void);

//FUNCIONES DE REPORTES
extern void func_historico(uint8_t cTipo);
extern void func_reporte_completo(void);
extern void func_reporte_mensual(void);
extern void func_reporte_resumen(void);
extern void func_rango_fecha(void);
extern void func_rango_numero(void);
extern void func_menu_historicos(void);
extern void func_menu_reimpresiones(void);
extern void func_reimpresion(uint8_t cTipo);
extern void func_reimprimir_factura(void);

//FUNCIONES DE PROGRAMACION
extern void func_prog_fecha(uint8_t cTipo);
extern void func_prog_medio(void);
extern void func_prog_flag(void);
extern void func_rep_programacion(void);
extern void func_programacion(uint8_t cTipo);
extern void func_prog_acceso_directo(void);
extern void func_prog_plu(void);
extern void func_prog_dep(void);
extern void func_prog_datos(void);

//FUNCIONES DE BORRADO
extern void func_eliminar_plu(void);
extern void func_elimina_plu(void);
extern void func_borrado_total_plu(void);

extern void func_eliminar_acc(void);
extern void func_elimina_acc(void);
extern void func_borrar_accesos(void);
extern void func_rep_plu(void);
extern void Reporte_PLU(void);
extern void Reporte_DEP(void);

	

//FUNCIONES AUXILIARES
extern void func_void(void);
extern void func_teclado_alfabetico(uint8_t hexadecimal);
extern void func_clean(void);
extern void func_monto(void);
extern void func_print_etiqueta(uint8_t cMensaje1, uint8_t cMensaje2, uint8_t cPos,uint8_t cLinea,  uint8_t cDisplay);
extern void func_config_leer(void);
extern void func_mensaje_error_ingreso(uint8_t bEstado);
extern uint64_t func_ingreso_obtenerNumero(void);

extern uint8_t func_enq(uint16_t *flags);
extern uint8_t func_actualizar_tasas(void);
extern void func_verifica_tasas(void);
uint8_t func_verifica_operacion_fiscal(void);

void func_verifica_anulacion(void);

#endif //END __FUNC_H



