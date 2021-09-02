/* Standard Includes */
#include <string.h>
#include <stdio.h>

/* Application Includes */
#include "menu.h"
#include "system.h"
#include "display.h"
#include "cmd.h"
#include "func.h"
#include "func_plu.h"
#include "uart.h"
#include "mem.h"
#include "includes.h"

//******************************************************************************//
//                         DEFINICIONES GENERALES                               //
//******************************************************************************//

//**********************DEFINICION DE ESTRUCTURAS************************//

//**********************DECLARACION DE VARIABLES LOCALES************************//

tMENU *MenuActual=NULL;
tMENU *MenuAuxClave=NULL;
uint16_t ElementoSeleccionActual=1;
uint16_t LineaElemSeleccion=0;
uint16_t NumeroElemMenuActual=0;

////////////////////////////////
//uint8_t cCant;
uint8_t cAux[128] = { NULL };
////////////////////////////////

// Usado para el efecto de barrido de los elementos de un menu..
int menuElemIndex;

void (*FuncionActualEjecucion)(unsigned char);
void (*FuncionPeriodica)(void);

/* ** Declaramos SubMenu para utilizar el direccionamiento ** */
tMENU tSubMenu1[]; // X
tMENU tSubMenu2[]; // Z
tMENU tSubMenu4[]; // SET
tMENU tRangos[];
tMENU tExistencia[];
tMENU tVentaN[];
tMENU tMenuBorrarPLU[];
tMENU tMenuBorrarACC[];
//tMENU tMenuBorrarFlash[];

//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//
void menu_dibujar_elemento(uint16_t NElemento);
///////////////////////////////////////////////////
void menu_dibujar_plus(int16_t NElemento);
void menu_centrar_string(char *target, char const *source);
void menu_resaltar_string(char *source);
///////////////////////////////////////////////////
void menu_eventos_submenu(unsigned char Evento);
void menu_salir(void);

/************** INICIALIZACION DE ESTRUCTURAS GLOBALES ****************/

tMENU tMenuPrincipal[]=
{
    {MENU_MAIN,             (uint8_t*)"MENU PRINCIPAL",     &menu_salir},
    {MENU_SUBMNU_CLAVEX,    (uint8_t*)"Modo X",             &tSubMenu1},
    {MENU_SUBMNU_CLAVEZ,    (uint8_t*)"Modo Z",             &tSubMenu2},
    {MENU_SUBMNU_CLAVES,    (uint8_t*)"Modo S",             &tSubMenu3},
    {MENU_SUBMNU_CLAVEJ	,    (uint8_t*)"Modo EJ",           &tSubMenu4},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0},
};

// Incidencia 568 etiquetas Reporte Reporte X2 y Reporte Z2

//SUBMENUS PRINCIPALES (MODOS)
tMENU tSubMenu1[]=
{
    {SUBMENU_ANTERIOR,      (uint8_t*)"MODO X",             &tMenuPrincipal},
    {MENU_REP_X,            (uint8_t*)"Rep. Diario X",      &func_reporte},
    {MENU_REP_X1,           (uint8_t*)"Reporte X2",        &func_reporte},
		{MENU_ELM_X1,          (uint8_t *)"Borrar Acum.X2",    	&func_reporte },
    {MENU_SUBMENU,          (uint8_t*)"Regresar",           &tMenuPrincipal},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tSubMenu2[]=
{
    {SUBMENU_ANTERIOR,      (uint8_t*)"MODO Z",              &tMenuPrincipal},
    {MENU_REP_Z,            (uint8_t*)"Rep. Diario Z",       &func_reporte},
    {MENU_REP_Z1,           (uint8_t*)"Reporte Z2",         &func_reporte},
		{MENU_ELM_Z1,          (uint8_t *)"Borrar Acum.Z2",    	 &func_reporte },
    {MENU_HISTORICOS_MF,    (uint8_t*)"Rep Mem Fiscal",      &tRangos},
    {MENU_SUBMENU,          (uint8_t*)"Regresar",            &tMenuPrincipal},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",               0},
    {MENU_FIN,              (uint8_t*)"",                    0}
};
//MENU_BORRAR_FLASH 
tMENU tSubMenu3[]=
{
    {SUBMENU_ANTERIOR,              (uint8_t*)"MODO S",                 &tMenuPrincipal},
    {PROGRAMACION_CLAVE,            (uint8_t*)"Prog. Clave",            &func_programacion},
    {PROGRAMACION_CAJERO,           (uint8_t*)"Prog. Cajero",           &func_programacion},
    {PROGRAMACION_FECHA,            (uint8_t*)"Prog. Fecha",            &func_programacion},
    {PROGRAMACION_HORA,             (uint8_t*)"Prog. Hora",             &func_programacion},
    {PROGRAMACION_MEDIOS,           (uint8_t*)"Prog. Medios",           &func_programacion},
    {PROGRAMACION_FLAGS,            (uint8_t*)"Prog. Flags",            &func_programacion},
    {PROGRAMACION_PLU,              (uint8_t*)"Prog. PLU",              &func_programacion},
    {PROGRAMACION_DEP,              (uint8_t*)"Prog. Dep",              &func_programacion},
    {PROGRAMACION_DIRECTO,          (uint8_t*)"Prog. Directo",          &func_programacion},
    {PROGRAMACION_CONFIGURACION,    (uint8_t*)"Prog. Config",           &func_programacion},
		{PROGRAMACION_EXISTENCIA,   		(uint8_t*)"Existencia",             &tExistencia},
    {MENU_COMANDO,                  (uint8_t*)"Reporte Prog.",          &func_rep_programacion},
		{MENU_REPORTE_PLU,   				  	(uint8_t *)"Reporte PLU",       		&func_rango_numero},
		{MENU_REPORTE_DEP,   				  	(uint8_t *)"Reporte DEP",       		&func_rango_numero},
		{MENU_BORRAR_PLU,   					  (uint8_t *)"Borrar PLUs",      			&tMenuBorrarPLU },
		{MENU_BORRAR_ACC,   					  (uint8_t *)"Borrar Accesos",      	&tMenuBorrarACC },
		{MENU_BORRAR_FLASH,   					(uint8_t *)"Borrar Flash",      	  &F25L_ChipErase},
    {MENU_SUBMENU,                  (uint8_t*)"Regresar",               &tMenuPrincipal},
    {MENU_COMANDO_FIN,              (uint8_t*)"Salir",                  0},
    {MENU_FIN,                      (uint8_t*)"",                       0}
};

tMENU tSubMenu4[]=
{
    {SUBMENU_ANTERIOR,      (uint8_t*)"MODO EJ",            &tMenuPrincipal},
    {MENU_COMANDO,          (uint8_t*)"Estado EJ",          &func_estatus_EJ},
    {MENU_HISTORICOS_EJ,    (uint8_t*)"Reimpresiones",      &tRangos},
    {MENU_SUBMENU,          (uint8_t*)"Regresar",           &tMenuPrincipal},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tHistoricos[]=
{   {MENU_MAIN,             (uint8_t*)"MENU PRINCIPAL",     &menu_salir},
    {MENU_HIST_COMPLETO,    (uint8_t*)"Completo",           &func_historico},
    {MENU_HIST_RESUMEN,     (uint8_t*)"Resumen",            &func_historico},
    {MENU_HIST_MENSUAL,     (uint8_t*)"Mensual",            &func_historico},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tHistoricos2[]=
{   {MENU_MAIN,             (uint8_t*)"MENU PRINCIPAL",     &menu_salir},
    {MENU_HIST_COMPLETO,    (uint8_t*)"Completo",           &func_historico},
    {MENU_HIST_RESUMEN,     (uint8_t*)"Resumen",            &func_historico},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tVentaN[]=
{   {MENU_MAIN,             (uint8_t*)"MENU PRINCIPAL",     &menu_salir},
    {MENU_VENT_PER,   			(uint8_t*)"Permitir",           &func_plu},
    {MENU_VENT_NEG,     		(uint8_t*)"Negar",           		&func_salir},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tTipoDoc[]=
{   {MENU_MAIN,             (uint8_t*)"TIPO REIMPRESION",   &menu_salir},
    {MENU_REPRINT_FAC,      (uint8_t*)"Facturas",           &func_reimpresion},
    {MENU_REPRINT_NC,       (uint8_t*)"Notas de Credito",   &func_reimpresion},
    {MENU_REPRINT_ND,       (uint8_t*)"Notas de Debito",    &func_reimpresion},
    {MENU_REPRINT_Z,        (uint8_t*)"Reportes Z",         &func_reimpresion},
    {MENU_REPRINT_RMF,      (uint8_t*)"Reportes MF",        &func_reimpresion},
    {MENU_REPRINT_NF,       (uint8_t*)"No Fiscales ",       &func_reimpresion},
    {MENU_REPRINT_COPIAS,   (uint8_t*)"Copias",             &func_reimpresion},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tRangos[]=
{
    {MENU_COMANDO,              (uint8_t*)"TIPO RANGOS",    &menu_salir},
    {MENU_OTROS_COMANDO_FIN,    (uint8_t*)"Fecha",          &func_rango_fecha},
    {MENU_OTROS_COMANDO_FIN,    (uint8_t*)"Numero",         &func_rango_numero},
    {MENU_COMANDO_FIN,          (uint8_t*)"Salir",          0},
    {MENU_FIN,                  (uint8_t*)"",0}
};

tMENU tExistencia[]=
{
    {MENU_COMANDO,              (uint8_t*)"EXISTENCIA ",   					 &menu_salir},
    {MENU_AUM_STOCK,				    (uint8_t*)"Aumt. Stock",          &func_programacion},
    {MENU_DIS_STOCK,  				  (uint8_t*)"Dism. Stock",         &func_programacion},
    {MENU_COMANDO_FIN,          (uint8_t*)"Salir",          0},
    {MENU_FIN,                  (uint8_t*)"",0}
};

//Otros Menus
tMENU tPagos[]=
{   {MENU_MAIN,             (uint8_t*)"MENU PAGOS",         &menu_salir},
    {MENU_PAGO_TOTAL,       (uint8_t*)"Total",              &func_pago_lista},
    {MENU_PAGO_PARCIAL,     (uint8_t*)"Parcial",            &func_pago_lista},
    {MENU_COMANDO_FIN,      (uint8_t*)"Salir",              0},
    {MENU_FIN,              (uint8_t*)"",                   0}
};

tMENU tDescuentos[]=
{   {MENU_MAIN,                 (uint8_t*)"MENU DESCUENTOS",    &menu_salir},
    {MENU_OTROS_COMANDO_FIN ,   (uint8_t*)"Descuentos",         &func_descuento},
    {MENU_OTROS_COMANDO_FIN,    (uint8_t*)"Recargos",           &func_recargo},
    {MENU_COMANDO_FIN,          (uint8_t*)"Salir",              0},
    {MENU_FIN,                  (uint8_t*)"",                   0}
};

tMENU tMenuBorrarPLU[] = {
    { MENU_MAIN,                (uint8_t *)"MENU BORRAR PLUs",   0 },
    { MENU_ELIMINAR_PLU, 				(uint8_t *)"Borrar PLU",         &func_eliminar_plu},
    { MENU_ELIMINAR_TODO_PLU, 	(uint8_t *)"Borrar Todos",   		 &func_borrado_total_plu},
    { MENU_FIN,                 (uint8_t *)"",                   0 }
};

tMENU tMenuBorrarACC[] = {
    { MENU_MAIN,                (uint8_t *)"MENU BORRAR ACC",   0 },
    { MENU_ELIMINAR_ACC, 				(uint8_t *)"Borrar Acceso",     &func_eliminar_acc},
    { MENU_ELIMINAR_TODO_ACC, 	(uint8_t *)"Borrar Todos",      &func_borrar_accesos},
    { MENU_FIN,                 (uint8_t *)"",                  0 }
};

//tMENU tMenuBorrarFlash[] = {
//    { MENU_MAIN,                (uint8_t *)"MENU BORRAR MEM",   0 },
//    { MENU_ELIMINAR_ACC, 				(uint8_t *)"Borrar FLASH",     &func_eliminar_acc},
//    { MENU_FIN,                 (uint8_t *)"",                  0 }
//};

//******************************************************************************//
//                            DEFINICION DE FUNCIONES                           //
//******************************************************************************//

/**
Muestra el menu principal de opciones
@retval Resultado de la operacion
@note  Nota importante.
*/
			
void menu_principal(void)
{		
		//	func_mensaje_error_ingreso
		// MENU X
		if(cMenuAct ==SUBMENU_X){menu_mostrar(tSubMenu1, 1, LINEA_SUPERIOR);}
		// MENU Z || SUBMENU Z
		else if(cMenuAct==SUBMENU_Z || cMenuAct==SUBMENU_Z1){menu_mostrar(tSubMenu2, 1, LINEA_SUPERIOR);}
		// MENU S || SUBMENU S
		else if(cMenuAct==SUBMENU_S||cMenuAct==SUBMENU_S1){menu_mostrar(tSubMenu3, 1, LINEA_SUPERIOR);}	
		// SUBMENU EJ
		else if(cMenuAct == SUBMENU_EJ || cMenuAct == SUBMENU_EJ1  ){menu_mostrar(tSubMenu4, 1, LINEA_SUPERIOR);}	
		
		else if(cMenuAct == SUBMENU_EJ2){menu_mostrar(tRangos, 1, LINEA_SUPERIOR);}	
		// SUBMENU BORRAR PLU
		else if(cMenuAct == SUBMENU_S2 ){
			menu_mostrar(tMenuBorrarPLU, 1, LINEA_SUPERIOR);
		}
		// SUBMENU BORRAR ACCESOS
		else if(cMenuAct == SUBMENU_S3 ){
			menu_mostrar(tMenuBorrarACC, 1, LINEA_SUPERIOR);
		}
//func_mensaje_error_ingreso
		if(cMenuAct == SUBMENU_S2 || cMenuAct == SUBMENU_S3){cMenuAct = SUBMENU_S1;}
//		if(cMenuAct >2 && cMenuAct <=10 && cMenuAct !=6 && cMenuAct != 3){cMenuAct = cMenuAct - 2; }
		else if(cMenuAct == SUBMENU_EJ2 ){cMenuAct = SUBMENU_EJ1;}
		else if(cMenuAct == SUBMENU_EJ1 ){cMenuAct = SUBMENU_EJ;}
		else if (cMenuAct == SUBMENU_S1 ){cMenuAct = SUBMENU_S ;}
		else if (cMenuAct == SUBMENU_Z1 ){cMenuAct = SUBMENU_Z;}
		else if(cMenuAct == SUBMENU_X || cMenuAct == SUBMENU_Z || cMenuAct == SUBMENU_S || cMenuAct == SUBMENU_EJ)
		{
			cMenuAct = NO_SUBMENU;
			tSys_flag.bErase = 1;
			display_erase_line(AMBAS_LINEAS, DISPLAY_OPERADOR);
			menu_mostrar(tMenuPrincipal, 1, LINEA_SUPERIOR);
		}
		else{
			tSys_flag.bErase = 1;
			display_erase_line(AMBAS_LINEAS, DISPLAY_OPERADOR);
			menu_mostrar(tMenuPrincipal, 1, LINEA_SUPERIOR);
		}
}

/**
Selecciona y dibuja un Menú/SubMenu, luego carga en puntero funcion
que atiende sus eventos
@*buf Buffer con el mapa de caracteres
@screen Texto a imprimir
@retval Resultado de la operacion
@note  Nota importante.
*/
void menu_mostrar(tMENU *Menu, uint16_t iElementoMenuSeleccion, uint16_t iLineaElemSel)
{
    int i;

    /* ** Guardamos el Menu actual que se visualiza y trabaja ** */
    MenuActual = Menu;
    ElementoSeleccionActual = iElementoMenuSeleccion;
    LineaElemSeleccion = iLineaElemSel;  // 0= Linea arriba, 1= Linea abajo

    i = 0;
    while(MenuActual[i + 1].TipoMenu != MENU_FIN)
    {
        i++;
    }
    NumeroElemMenuActual = i;

    if(MenuActual[iElementoMenuSeleccion].TipoMenu != MENU_FIN)
    {
        menu_dibujar_elemento(iElementoMenuSeleccion);
    }

    //Se activa la bandera del sistema indicando menu activo
    tSys_flag.bMenuAtivo=1;

    //Se actualiza la variable de control
    tControl.cOp = OP_MENU;
}


/* ** Dibuja elementos del submenu ** */
void menu_dibujar_elemento(uint16_t NElemento)
{
    uint8_t texto[21];
    int NumElemAux;
    int len;
    int len2;
    int pos;

    // Longitud del texto del submenu debe ser menor o igual a 16
    len2 = strlen((char*)MenuActual[NElemento].TextoMenu);
    memset(texto, 0x00, sizeof(texto));

    //Centrar el texto en el display
    if(len2 >= 14)
    {
        pos = 1;
    }
    else
    {
        pos = (int)((16 - len2) / 2);
    }
    memset(texto, 0x20, pos);
    strncat((char*)texto, (char*)MenuActual[NElemento].TextoMenu, 14);
    len = strlen((char*)texto);
    memset(texto+len, 0x20, 16-len);
    tSys_flag.bTerceralinea = 0;
    //Se imprime la primera linea del menu
    tSys_flag.bErase = 1;
    display_print(texto, LineaElemSeleccion, FONDO_BLANCO, DISPLAY_OPERADOR);

    if (LineaElemSeleccion == 0)
    {
        if(ElementoSeleccionActual == NumeroElemMenuActual)
        {
            NumElemAux = 1;
        }
        else
        {
            NumElemAux = ElementoSeleccionActual+1;
        }
    }
    else
    {
        if(ElementoSeleccionActual == 1)
        {
            NumElemAux = NumeroElemMenuActual;
        }
        else
        {
            NumElemAux = ElementoSeleccionActual - 1;
        }
    }

    // Longitud del texto del submenu debe ser menor o igual a 16
    len2 = strlen((char*)MenuActual[NumElemAux].TextoMenu);
    memset(texto, 0x00, sizeof(texto));

    //Centrar el texto en el display
    if(len2 >= 14)
    {
        pos = 1;
    }
    else
    {
        pos = (int)((16 - len2) / 2);
    }
    memset(texto, 0x20, pos);
    strncat((char*)texto, (char*)MenuActual[NumElemAux].TextoMenu, 14);
    len = strlen((char*)texto);
    memset(texto + len, 0x20, 16 - len);

    //Se imprime la linea no seleccionada
    Display_rotateUnSet();
    display_print(texto, !LineaElemSeleccion, FONDO_NEGRO,DISPLAY_OPERADOR);
}

/* ** Se cambia de elemento seleccionado en el submenu ** */
void menu_cambio_seleccion(int NuevoElementoSeleccion)
{

    if((NuevoElementoSeleccion>=1) && (MenuActual[NuevoElementoSeleccion].TipoMenu!=MENU_FIN) && (NuevoElementoSeleccion!=ElementoSeleccionActual))
    {
        ElementoSeleccionActual = NuevoElementoSeleccion;

    }
    else if (NuevoElementoSeleccion == 0)
    {
        ElementoSeleccionActual = NumeroElemMenuActual;
        LineaElemSeleccion = LINEA_INFERIOR;
    }
    else
    {
        ElementoSeleccionActual = 1;
        LineaElemSeleccion = LINEA_SUPERIOR;
    }
    menu_dibujar_elemento(ElementoSeleccionActual); // dibuja el nuevo elemento seleccionado
}


void menu_mostrar_submenu(void)
{
    tMENU* tSubmenu;

    tSubmenu = (tMENU*)MenuActual[ElementoSeleccionActual].PtrMenuSeleccion;
    menu_mostrar(tSubmenu, 1,LINEA_SUPERIOR);

}

void menu_clave(void)
{
    tSys_flag.bTecAlfabetico=FALSE;
    tSys_flag.bIngresoDoble=FALSE;
    MenuActual=(tMENU*)MenuActual[ElementoSeleccionActual].PtrMenuSeleccion;
    func_print_etiqueta(MENSAJE_CLAVE, 0, 6, LINEA_SUPERIOR, DISPLAY_OPERADOR);//wplanchez	
}

void menu_salir()
{
    if(tSys_flag.bMenuOtrosActivo==0)
    {
        display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);
        display_message_ini();
        tSys_flag.bTerceralinea=1;
        tSys_flag.bShiftActivo=0;
        tControl.cOp=OP_NO;
    }
    tSys_flag.bMenuAtivo=0;
    tSys_flag.bMenuOtrosActivo=0;
}

void menu_ejecutar_opcion(void)
{
    tMENU* tMenuAuxiliar=NULL;  //Variable auxiliar para el menu final
    uint8_t cTipoMenu=0;
    void (*PtrComando)(void); // puntero a funcion que retorna void y no tiene parametros
    void (*PtrComandoPar)(uint8_t); // puntero a funcion que retorna void y no tiene parametros

    cTipoMenu = MenuActual[ElementoSeleccionActual].TipoMenu;

    switch(cTipoMenu)
    {
        //Se ejecuta la funcion apuntada por la estructura
        case MENU_COMANDO:
            PtrComando=(void(*)(void))MenuActual[ElementoSeleccionActual].PtrMenuSeleccion;
            //Se ejecuta la funcion
            if(PtrComando!=0) (*PtrComando)();
            //Se vuelve a mostrar el menu actual despues de ejecutar el comando
            tMenuAuxiliar = MenuActual;
            break;
        //Se ejecuta la funcion apuntada por la estructura
        case MENU_COMANDO_FIN:
            PtrComando=(void(*)(void))MenuActual[ElementoSeleccionActual].PtrMenuSeleccion;
            //Se ejecuta la funcion
            if(PtrComando!=0) (*PtrComando)();
            //Se cierra el menu
            tSys_flag.bMenuOtrosActivo=0;
            menu_salir();
            break;
        //Se ejecuta la funcion apuntada por la estructura
        case MENU_OTROS_COMANDO_FIN:
            PtrComando=(void(*)(void))MenuActual[ElementoSeleccionActual].PtrMenuSeleccion;
            //Se ejecuta la funcion
            if(PtrComando!=0) (*PtrComando)();
            //Se cierra el menu
            menu_salir();
            break;
        //Se dibuja el menu apuntado por la estructura
        case MENU_SUBMNU_CLAVEX:
					
						cMenuAct = SUBMENU_X;
						
            tControl.cOp=OP_CLAVE_X;
            menu_clave();
            break;
						
        case MENU_SUBMNU_CLAVEZ:
					
						cMenuAct = SUBMENU_Z;
						
            tControl.cOp=OP_CLAVE_Z_EJ;
            menu_clave();
            break;
				
				case MENU_SUBMNU_CLAVEJ:
					
						cMenuAct = SUBMENU_EJ;
						
            tControl.cOp=OP_CLAVE_Z_EJ;
            menu_clave();
            break;
				
        case MENU_SUBMNU_CLAVES:
					
						cMenuAct = SUBMENU_S;
            tControl.cOp=OP_CLAVE_S; 
            menu_clave();
						
            break;
        case MENU_SUBMENU:
        case SUBMENU_ANTERIOR:
        case MENU_MAIN:
            menu_mostrar_submenu();
            break;
				case PROGRAMACION_EXISTENCIA:
					cMenuAct = SUBMENU_S1; //SUBMENU_S1;
          menu_mostrar_submenu();
					break;
				case MENU_BORRAR_PLU:
						cMenuAct = SUBMENU_S1;
            menu_mostrar_submenu();
            break;
				case MENU_BORRAR_ACC:
						cMenuAct = SUBMENU_S1;
						menu_mostrar_submenu();
						break;
        case MENU_HISTORICOS_MF:
						cMenuAct = SUBMENU_Z1;
            menu_mostrar_submenu();
            tControl.cEst=EST_REPORTES_MF;
            break;
        case MENU_HISTORICOS_EJ:
						cMenuAct = SUBMENU_EJ1;
            menu_mostrar_submenu();
            tControl.cEst=EST_REPORTES_EJ;
            break;
        case MENU_HIST_RESUMEN:
        case MENU_HIST_COMPLETO:
        case MENU_HIST_MENSUAL:
            PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            //Se vuelve a mostrar el menu anterior despues de ejecutar el comando
            tMenuAuxiliar = tSubMenu2;
            break;
        case MENU_REPRINT_FAC:
        case MENU_REPRINT_NC:
        case MENU_REPRINT_ND:
        case MENU_REPRINT_Z:
        case MENU_REPRINT_RMF:
        case MENU_REPRINT_NF:
        case MENU_REPRINT_COPIAS:
            PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            //Se vuelve a mostrar el menu anterior despues de ejecutar el comando
            tMenuAuxiliar = tSubMenu4;
            break;
        case MENU_REP_X:
				case MENU_REP_X1:
        case MENU_REP_Z:
        case MENU_REP_Z1:
				case MENU_ELM_X1:
				case MENU_ELM_Z1:
            //Se ejecuta la funcion y vuelve a mostrarse el menu actual
            tMenuAuxiliar= MenuActual;
        case PROGRAMACION_FECHA:
        case PROGRAMACION_HORA:
        case PROGRAMACION_TASAS:
        case PROGRAMACION_MEDIOS:
        case PROGRAMACION_FLAGS:
        case PROGRAMACION_PLU:
        case PROGRAMACION_DEP:
        case PROGRAMACION_DIRECTO:
        case PROGRAMACION_CONFIGURACION:
        case PROGRAMACION_CAJERO:
        case PROGRAMACION_CLAVE:
        case PROGRAMACION_MENSAJE: 
//				case MENU_BORRAR_FLASH:
					cMenuAct = SUBMENU_S1;
				case MENU_PAGO_TOTAL:
        case MENU_PAGO_PARCIAL:
				case MENU_AUM_STOCK:
				case MENU_DIS_STOCK:
				case MENU_VENT_PER:
				case MENU_VENT_NEG:
						if(cTipoMenu==MENU_VENT_PER)
							tControl.cOp = OP_NO;
				
            PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            break;
				case MENU_BORRAR_FLASH:
						func_print_etiqueta(MENSAJE_BORRADO_TODOS, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
					   PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
						mensaje_display(MENSAJE_BORRADO_EXITO, DISPLAY_OPERADOR);
						OSTimeDly(1000);
						func_salir(TRUE);
						break;
				case MENU_REPORTE_PLU:
						cMenuAct = SUBMENU_S1;
						tControl.cEst=EST_REPORTES_PLU;
				    PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            break;
				case MENU_REPORTE_DEP:
						cMenuAct = SUBMENU_S1;
						tControl.cEst=EST_REPORTES_DEP;
				    PtrComandoPar=(void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            //Se ejecuta la funcion
            if(PtrComandoPar!=0) (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            break;
				case MENU_ELIMINAR_PLU:
						cMenuAct = SUBMENU_S2;
						tControl.cOp = OP_RANGO_PLU_NUMERO;
						PtrComandoPar = (void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            /* Se ejecuta la funcion */
            if (PtrComandoPar != NULL) {
                (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            }
           	break;
					
					case MENU_ELIMINAR_TODO_PLU:
						cMenuAct = SUBMENU_S2;
						tControl.cEst = EST_BORRAR_TODO_PLU;
						func_print_etiqueta(MENSAJE_BORRADO_TODOS, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
						//func_display_etiqueta(MENSAJE_BORRADO_TODOS);
						tPluActual.wInicio=0;
						cIntentos=0;
						PtrComandoPar = (void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            /* Se ejecuta la funcion */
            if (PtrComandoPar != NULL) {
                (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            }
					break;
						
					case MENU_ELIMINAR_ACC:
						cMenuAct = SUBMENU_S3;
						tControl.cOp = OP_RANGO_ACC_NUMERO;
						PtrComandoPar = (void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            /* Se ejecuta la funcion */
            if (PtrComandoPar != NULL) {
							(*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            }
						break;
					
					case MENU_ELIMINAR_TODO_ACC:
						cMenuAct = SUBMENU_S2;
						tControl.cEst = EST_BORRAR_ACCESOS;
						func_print_etiqueta(MENSAJE_BORRADO_TODOS, 0, 0, LINEA_SUPERIOR, DISPLAY_OPERADOR);
						tPluActual.wInicio=0;
						cIntentos=0;
						PtrComandoPar = (void(*)(uint8_t))MenuActual[ElementoSeleccionActual].PtrFuncSeleccion;
            /* Se ejecuta la funcion */
            if (PtrComandoPar != NULL) {
                (*PtrComandoPar)(MenuActual[ElementoSeleccionActual].TipoMenu);
            }
						break;
											
        default:
        break;
    }

    //Se muestra el menu final si es necesario
    if(tMenuAuxiliar!=NULL)
    {
        menu_mostrar(tMenuAuxiliar, 1,LINEA_SUPERIOR);
    }

}

void menu_centrar_string(char *target, char const *source){
	
	size_t i;
	size_t length;

	/* Se llena el buffer destino con un string de espacios */
	memset(target, ' ', MAX_DISPLAY);
	target[MAX_DISPLAY] = 0;
	/* Se determina la longitud del string fuente */
	length = strlen(source);

	if (length <= (MAX_DISPLAY - 2 * MARGEN)) {    /* Se centra en el buffer destino */
		for (i = 0, target += MARGEN + (((MAX_DISPLAY - 2 * MARGEN) - length) / 2); i < length; ++i) {
			target[i] = source[i];
		}
	}
	else {                                  /* Se trunca en el string destino */
		for (i = 0, target += MARGEN; i < (MAX_DISPLAY - 2 * MARGEN); ++i) {
			target[i] = source[i];
		}
	}
	tSys_flag.bTerceralinea = 0;
	//Se imprime la primera linea del menu
	tSys_flag.bErase = 1;
}

void menu_resaltar_string(char *source)
{
    source[0] = 0xF6;
    source[MAX_DISPLAY - 1] = 0xF7;
}

void menu_ejecutar_opcion2(void)
{		
	uint8_t *pbuffer;
	uint8_t resp = FALSE;
	
	tPLU_VENTA *pPluVenta = (tPLU_VENTA *)pbuffer;
	tPLU_INFO *pPluAux = (tPLU_INFO *)cBufAux;
	tDEP_INFO *pDepAux = (tDEP_INFO *)cBufAux2;
	
	MEM_leerVENTA(pPluVenta->cValor, ElementoSeleccionActual);
	
	tPluActual.iIndex = pPluVenta->iIndex;	
	tPluActual.wCantidad = pPluVenta->wCantidad;
	tPluActual.cMarca = pPluVenta->cMarca;
	tPluActual.wPrecio = pPluAux->wPrecio;
	
	if(pPluVenta->cTipo == 0)
	{
		MEM_leerPLU(pPluAux->cValor,tPluActual.iIndex);
		strcpy((char *)tPluActual.cDescripcion, (char *)pPluAux->cDescripcion);
		
		tPluActual.wPrecio = pPluAux->wPrecio;
		
		resp = func_comando_plu();
	}
	
	else
	{
		MEM_leerDepartamento(pDepAux->cValor,tPluActual.iIndex);
		strcpy((char *)tPluActual.cDescripcion, (char *)pDepAux->cDescripcion);
		tPluActual.wPrecio = pDepAux->wPrecioDepartamento;
		resp = func_comando_plu();
		
	}	
}

void menu_cambio_seleccion2(int16_t NuevoElementoSeleccion)
{
		MEM_leerCONTADOR(&cCONT,0);
		if(NuevoElementoSeleccion==cCONT+1)
		{
			ElementoSeleccionActual=0;
			LineaElemSeleccion = LINEA_SUPERIOR;
		}
		else if(NuevoElementoSeleccion==-1)
		{
			ElementoSeleccionActual=cCONT;
			LineaElemSeleccion = LINEA_INFERIOR;
		}
		else
		{
			ElementoSeleccionActual=NuevoElementoSeleccion;
		}
    /* Dibuja el nuevo elemento seleccionado */
    menu_dibujar_plus(ElementoSeleccionActual);
}

void menu_mostrar2(size_t iElementoMenuSeleccion, size_t iLineaElemSel)
{
    /* Se guarda el menu actual que se visualiza y trabaja */
    ElementoSeleccionActual = iElementoMenuSeleccion;
    LineaElemSeleccion = iLineaElemSel;  /* 0 = Linea arriba, 1 = Linea abajo */

		MEM_leerCONTADOR(&cCONT,0);

		menu_dibujar_plus(iElementoMenuSeleccion);
   
    /* Se actualiza la variable de control */

    tControl.cOp = OP_MENU2;
}

void menu_dibujar_plus(int16_t NElemento)
{
	
		uint8_t resp = FALSE;
		char buffer[MAX_DISPLAY + 1];
    size_t NumElemAux;

    /* Se determina el elemento no seleccionado */
    if (LineaElemSeleccion == 0) 
		{
			NumElemAux = NElemento + 1;     
    }
    else 
		{
      NumElemAux = NElemento - 1;
    }
		
		MEM_leerCONTADOR(&cCONT,0);
		
		if (LineaElemSeleccion == 0){
			resp = MEM_leerVENTA(tPluActual.cValor, NElemento);
			while(resp == FALSE){
				NElemento--;
				if(NElemento==-1)
					NElemento=cCONT;
					resp = MEM_leerVENTA(tPluActual.cValor, NElemento);
				ElementoSeleccionActual=NElemento;
		}
	}

		resp = MEM_leerVENTA(tPluActual.cValor, NElemento);
			
		if(resp)
		{

			memset(cBufGen, 0x20, MAX_DISPLAY);
			snprintf((char *)(cBufGen), 6,"%s", (char *)tPluActual.cDescripcion);
			sprintf((char *)cAux, " %llu,%02llu",tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
			strcat((char*)cBufGen, (char *)cAux);		
			/* Se imprime la linea selecciondada */
			menu_centrar_string(buffer, (char *)cBufGen);
			display_print((uint8_t*)buffer, LineaElemSeleccion, FONDO_BLANCO, DISPLAY_OPERADOR);
		}
		
		else
		{
			while(resp==FALSE && NElemento<=cCONT)
			{			
				/* Se actualiza el auxiliar */
				if (LineaElemSeleccion == 0) 
				{
          NumElemAux = (NElemento-1) + 1;
        }
				else {        
          NumElemAux = (NElemento-1) - 1;
        }
				
				resp = MEM_leerVENTA(tPluActual.cValor, NElemento++);
				
				if(resp)
				{
					
					ElementoSeleccionActual=NElemento-1;
					memset(cBufGen, 0x20, MAX_DISPLAY);
					snprintf((char *)(cBufGen), 6,"%s", (char *)tPluActual.cDescripcion);
					sprintf((char *)cAux, " %llu,%02llu",tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
					strcat((char*)cBufGen, (char *)cAux);
					/* Se imprime la linea selecciondada */
					menu_centrar_string(buffer, (char *)cBufGen);
					display_print((uint8_t*)buffer, LineaElemSeleccion, FONDO_BLANCO, DISPLAY_OPERADOR);
				}
				else
				{
					memset(cBufGen, 0x20, MAX_DISPLAY);
					/* Se imprime la linea selecciondada */
					menu_centrar_string(buffer, (char *)cBufGen);
					display_print((uint8_t*)buffer, LineaElemSeleccion, FONDO_BLANCO, DISPLAY_OPERADOR);
				}
			}
		}
		
		/* Se pregunta si el siguiente ya supero el maximo de registrados  */

		MEM_leerCONTADOR(&cCONT,0);
		if (LineaElemSeleccion == 1)
		{
			resp = MEM_leerVENTA(tPluActual.cValor, NumElemAux);
			while(resp){
				NumElemAux--;
				if(NumElemAux==-1)
					NumElemAux=cCONT;
					resp = MEM_leerVENTA(tPluActual.cValor, NumElemAux);
			}
		}

		if(cCONT >1){		
			if(NumElemAux>cCONT)
			{

				memset(cBufGen, 0x20, MAX_DISPLAY);
				/* Se imprime la linea no seleccionada */
				menu_centrar_string(buffer, (char *)cBufGen);
				display_print((uint8_t*)buffer, !LineaElemSeleccion, FONDO_NEGRO, DISPLAY_OPERADOR);
			}
			else
			{
				resp = MEM_leerVENTA(tPluActual.cValor, NumElemAux+1);
							
				if(resp)
				{
					memset(cBufGen, 0x20,MAX_DISPLAY);
					snprintf((char *)(cBufGen), 6,"%s", (char *)tPluActual.cDescripcion);
					sprintf((char *)cAux, " %llu,%02llu",tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
					strcat((char*)cBufGen, (char *)cAux);
					/* Se imprime la linea no seleccionada */
					menu_centrar_string(buffer, (char *)cBufGen);
					display_print((uint8_t*)buffer, !LineaElemSeleccion, FONDO_NEGRO, DISPLAY_OPERADOR);
				}
				
				else
				{
				
					while(resp==FALSE && NumElemAux<=cCONT)
					{
						resp = MEM_leerVENTA(tPluActual.cValor, NumElemAux++);

						if(resp)
						{
							memset(cBufGen, 0x20, MAX_DISPLAY);
							snprintf((char *)(cBufGen), 6,"%s", (char *)tPluActual.cDescripcion);
							sprintf((char *)cAux, " %llu,%02llu",tPluActual.wPrecio / 100llu, tPluActual.wPrecio % 100llu);
							strcat((char*)cBufGen, (char *)cAux);
							/* Se imprime la linea no seleccionada */
							menu_centrar_string(buffer, (char *)cBufGen);
							display_print((uint8_t*)buffer, !LineaElemSeleccion, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
						else
						{
							memset(cBufGen, 0x20, MAX_DISPLAY);
							/* Se imprime la linea no seleccionada */
							menu_centrar_string(buffer, (char *)cBufGen);
							display_print((uint8_t*)buffer, !LineaElemSeleccion, FONDO_NEGRO, DISPLAY_OPERADOR);
						}
				}
			}
		}
	}		
}

