/**
  ******************************************************************************
  * @file    menu.h
  * @author  THE FACTORY HKA C.A.
  * @version v00r00
  * @date    Marzo-2015
  * @brief   Libreria de Manejo de Menus del teclado
  *          
  ******************************************************************************
  */    
    
#ifndef __MENU_H
#define __MENU_H

#include <stdint.h>

#pragma anon_unions

//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//


/*#define MILES               0
#define CIENTOS             1
#define DECENAS             2
#define UNIDADES            3*/


enum{
    MSJ_KEY_MENU = 0,
    MSJ_KEY_OPTIONS
};

/**
    Estructura para asociar una opcion de menu a la función que la atiende. 
*/
typedef struct
{
    uint8_t TipoMenu;
    uint8_t *TextoMenu;
    union
    {
        void (*PtrMenuSeleccion);
        void (*PtrFuncSeleccion)(uint8_t);
    };
} tMENU;



#define MENU_FIN            0
#define MENU_COMANDO        1

#define MENU_BORRAR_PLU   	2
#define MENU_ELIMINAR_PLU   3
#define MENU_ELIMINAR_TODO_PLU  4

#define MENU_BORRAR_ACC  		37
#define MENU_ELIMINAR_ACC   38
#define MENU_ELIMINAR_TODO_ACC  39

#define MENU_ELM_X1 				41
#define	MENU_ELM_Z1 				42
#define MENU_REPORTE_PLU    43
#define MENU_REPORTE_DEP    44
#define MENU_SUBMNU_CLAVE6  5 //SUB MENU CON CLAVE 6 DIGITOS
#define MENU_SUBMENU        6
#define SUBMENU_ANTERIOR    7
#define MENU_MAIN           8
#define FECHA               9
#define NUMERO              10
#define MENU_HISTORICOS_MF  11
#define MENU_HISTORICOS_EJ  12
#define REPORTE_MENSUAL     13
#define PROGRAMACION_FECHA  14
#define PROGRAMACION_HORA   15
#define PROGRAMACION_TASAS  16
#define PROGRAMACION_MEDIOS 17
#define PROGRAMACION_FLAGS  18
#define PROGRAMACION_PLU    19
#define PROGRAMACION_DEP    20
#define MENU_PAGO_TOTAL     21
#define MENU_PAGO_PARCIAL   22

#define MENU_REP_X          23
#define MENU_REP_X1         24
#define MENU_REP_Z          25
#define MENU_REP_Z1         26

#define PROGRAMACION_DIRECTO    27
#define MENU_COMANDO_FIN        28
#define MENU_OTROS_COMANDO_FIN  29

#define MENU_SUBMNU_CLAVEX      30
#define MENU_SUBMNU_CLAVEZ      31
#define MENU_SUBMNU_CLAVES      32
#define MENU_SUBMNU_CLAVEJ			40

#define PROGRAMACION_CAJERO     33
#define PROGRAMACION_CLAVE      34

#define PROGRAMACION_MENSAJE        35
#define PROGRAMACION_CONFIGURACION  36
#define PROGRAMACION_EXISTENCIA			45 

#define MENU_AUM_STOCK     46
#define MENU_DIS_STOCK     47

#define MENU_VENT_PER         48
#define MENU_VENT_NEG         49

#define MENU_BORRAR_FLASH     50

#define MENU_REPRINT_FAC        'f'
#define MENU_REPRINT_NC         'c'
#define MENU_REPRINT_ND         'd'
#define MENU_REPRINT_Z          'z'
#define MENU_REPRINT_RMF        'r'
#define MENU_REPRINT_NF         'n'
#define MENU_REPRINT_COPIAS 'e'

#define MENU_HIST_RESUMEN   'S'
#define MENU_HIST_COMPLETO  'A'
#define MENU_HIST_MENSUAL   'M'

//SUBMENUS
#define NO_SUBMENU 0
#define SUBMENU_X 1

#define SUBMENU_Z 2
#define SUBMENU_Z1 4

#define SUBMENU_S 3
#define SUBMENU_S1 5
#define SUBMENU_S2 7
#define SUBMENU_S3 9

#define SUBMENU_EJ 6
#define SUBMENU_EJ1 8
#define SUBMENU_EJ2 10


//**********************DECLARACION DE VARIABLES LOCALES************************//
extern tMENU *MenuActual;
extern tMENU MenuPrincipal[]; 
extern tMENU tSubMenu1[]; 
extern tMENU tSubMenu2[]; 
extern tMENU tSubMenu3[]; 
extern tMENU tSubMenu4[]; 
extern tMENU tMenuBorrarPLU[];
extern tMENU tMenuBorrarACC[];
//extern tMENU tMenuBorrarFlash[];
extern tMENU tRangos[];
extern tMENU tPagos[];
extern tMENU tDescuentos[];
extern tMENU tHistoricos[];
extern tMENU tHistoricos2[];
extern tMENU tTipoDoc[];
extern tMENU tVentaN[];
extern uint16_t ElementoSeleccionActual;                 
extern uint16_t LineaElemSeleccion;                      
extern uint16_t NumeroElemMenuActual; 
extern void menu_mostrar(tMENU *Menu, uint16_t iElementoMenuSeleccion, uint16_t iLineaElemSel);
//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PUBLICAS                                                //
//******************************************************************************//
extern void menu_principal(void);
extern void menu_cambio_seleccion(int NuevoElementoSeleccion);
extern void menu_ejecutar_opcion(void);
extern void menu_total_parcial(void);
extern void menu_tipo_historico_fecha(void);

extern void menu_cambio_seleccion2(int16_t NuevoElementoSeleccion);
extern void menu_ejecutar_opcion2(void);
extern void menu_mostrar2(size_t iElementoMenuSeleccion, size_t iLineaElemSel);

#endif //END __MENU_H

