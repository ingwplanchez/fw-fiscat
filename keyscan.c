#include "includes.h"
#include "stdlib.h"
#include "gpio.h"
#include "stdio.h"
#include "system.h"
#include "display.h"
#include "menu.h"
#include "func.h"
#include "func_plu.h"
#include "keyboard.h"
#include "uart.h"

//****************************************************************************//
//                         DEFINICIONES GENERALES                             //
//****************************************************************************//
typedef struct {
    uint8_t x;
    uint8_t y;
} Tecla;

//============================================================================================================================

uint16_t bdoblecero;
//uint8_t bzerokey; //wplanchez

//************************DEFINICION DE ESTRUCTURAS****************************//

/**
    Estructura para asociar una opcion de menu a la función que la atiende.
*/
typedef struct
{
    union
    {
        void (*PtrFuncPri)();           //Puntero a función de primer nivel
        void (*PtrFuncPriP)(uint8_t);   //Puntero a función de primer nivel
    };
    union
    {
        void (*PtrFuncSec)();           //Puntero a función de segundo nivel
        void (*PtrFuncSecP)(uint8_t);   //Puntero a función de segundo nivel
    };
    uint8_t p1,p2;
} tTECLADO;

tTECLADO tFUNC_TECLADO[5][8]=
{
    //Fila 0
    {   {&func_tecla_shift,&func_tecla_shift,0,0},
        {&func_price, &func_plu_consulta,0,0},                                  //Columnas 0 y 1
        {&func_tecla_borrar, &func_cmd_simple_fac,0,'k'},
        {&func_anular_plu, &func_cmd_simple_fac,0,'7'},                 //Columnas 2 y 3
        {&func_teclas_direc_up, &func_teclas_direc_up,0,0},
        {&func_teclas_direc_down, &func_teclas_direc_down,0,0}, //Columnas 4 y 5
        {&func_descuento_menu, &func_descuento_menu,0,1},
        {&func_cajero_menu, &func_menu,0,0}},                               //Columnas 6 y 7
    //Fila 1
    {   {&func_departamento, &func_departamento},
        {&tecla_numerica,&tecla_numerica,'7','7'},                          //Columnas 0 y 1
        {&tecla_numerica,&tecla_numerica,'8','8'},
        {&tecla_numerica, &tecla_numerica,'9','9'},                             //Columnas 2 y 3
        {&func_plu_directo, &func_plu_directo,3,11},
        {&func_plu_directo, &func_plu_directo,7,15},                            //Columnas 4 y 5
        {&func_fondo_retiro_menu, &func_fondo_retiro_menu,0,1},
        {&func_pago_total, &func_pago_total,1,2}},//Columnas 6 y 7
    //Fila 2
        {{&func_venta_plu, &func_venta_plu,1,1 },
        {&tecla_numerica, &tecla_numerica,'4','4'},                                         //Columnas 0 y 1
        {&tecla_numerica,&tecla_numerica,'5','5'},
        {&tecla_numerica, &tecla_numerica,'6','6'},                                         //Columnas 2 y 3
        {&func_plu_directo, &func_plu_directo,2,10},
        {&func_plu_directo, &func_plu_directo,6,14},                            //Columnas 4 y 5
        {&func_personalizar_factura,0,0,0},
        {&func_pago_total, &func_pago_total,3,4}},                                                                                  //Columnas 6 y 7
    //Fila 3
    {   {&func_tecla_x, &func_hora_fecha,0,0},
        {&tecla_numerica, &tecla_numerica,'1','1'},                                         //Columnas 0 y 1
        {&tecla_numerica,&tecla_numerica,'2','2' },
        {&tecla_numerica, &tecla_numerica,'3','3'},                                         //Columns 2 y 3
        {&func_plu_directo, &func_plu_directo,1,9},
        {&func_plu_directo, &func_plu_directo,5,13},                        //Columnas 4 y 5
        {&func_nota_deb_cred,&func_nota_deb_cred,EST_NOTA_CREDITO,EST_NOTA_DEBITO},
        {&func_cmd_simple_fac, &func_apertura_gaveta,'3'}},         //Columnas 6 y 7
    //Fila 4
    {   {&func_salir, &func_salir,0,0},
        {&tecla_numerica, &tecla_numerica,'0','0'},                                         //Columnas 0 y 1
        {&tecla_numerica,&tecla_numerica,'C','C'},
        {&tecla_numerica, &tecla_numerica,',',','},                                         //Columnas 2 y 3
        {&func_plu_directo, &func_plu_directo,0,8},
        {&func_plu_directo, &func_plu_directo,4,12},                            //Columnas 4 y 5
        {&func_calculadora, &func_reimpresion_ultimo,0,0},
        {&func_pago_directo0,&func_pago_menu,0,0}},                         //Columnas 6 y 7
};

//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//

static void keyscan_process(uint8_t cFila,uint8_t cCol);

/**
Detecta el evento de tecla presionada
@*buf Buffer con el mapa de caracteres
@screen Texto a imprimir
@retval Resultado de la operacion
@note  Nota importante.
*/
void keyScan_task(void)
{
    KeyBoardKey key;

    static Tecla const tecla[40] = {
        /* Fila 0 */
        [23].x = 0, [23].y = 0,
        [22].x = 0, [22].y = 1,
        [21].x = 0, [21].y = 2,
        [20].x = 0, [20].y = 3,
        [19].x = 0, [19].y = 4,
        [18].x = 0, [18].y = 5,
        [17].x = 0, [17].y = 6,
        [16].x = 0, [16].y = 7,
        /* Fila 1 */
        [15].x = 1, [15].y = 0,
        [14].x = 1, [14].y = 1,
        [13].x = 1, [13].y = 2,
        [12].x = 1, [12].y = 3,
        [11].x = 1, [11].y = 4,
        [10].x = 1, [10].y = 5,
        [9].x = 1,  [9].y = 6,
        [8].x = 1,  [8].y = 7,
        /* Fila 2 */
        [7].x = 2, [7].y = 0,
        [6].x = 2, [6].y = 1,
        [5].x = 2, [5].y = 2,
        [4].x = 2, [4].y = 3,
        [3].x = 2, [3].y = 4,
        [2].x = 2, [2].y = 5,
        [1].x = 2, [1].y = 6,
        [0].x = 2, [0].y = 7,
        /* Fila 3 */
        [31].x = 3, [31].y = 0,
        [30].x = 3, [30].y = 1,
        [29].x = 3, [29].y = 2,
        [28].x = 3, [28].y = 3,
        [27].x = 3, [27].y = 4,
        [26].x = 3, [26].y = 5,
        [25].x = 3, [25].y = 6,
        [24].x = 3, [24].y = 7,
        /* Fila 4 */
        [39].x = 4, [39].y = 0,
        [38].x = 4, [38].y = 1,
        [37].x = 4, [37].y = 2,
        [36].x = 4, [36].y = 3,
        [35].x = 4, [35].y = 4,
        [34].x = 4, [34].y = 5,
        [33].x = 4, [33].y = 6,
        [32].x = 4, [32].y = 7,
    };

    key = KeyBoard_getKey();
    if (key != 0) {
        --key;
        keyscan_process(tecla[key].x, tecla[key].y);
    }
}

/**
Función que procesa los eventos del teclado a partir de las coordenadas de la tecla presionada
@cFila      Coordenada y de la tecla presionada
@cColumna Coordenada x de la tecla presionada
@retval Resultado de la operacion
@note  Nota importante.
*/
void keyscan_process(uint8_t cFila, uint8_t cColumna)
{
    void (*PtrFuncion)(uint8_t);

    //Se llama a la funcion de procesamiento segun el nivel activo
    if(tSys_flag.bShiftActivo==0)
    {
        PtrFuncion=(void(*)(uint8_t))tFUNC_TECLADO[cFila][cColumna].PtrFuncPri;
        if(PtrFuncion!=0) (*PtrFuncion)(tFUNC_TECLADO[cFila][cColumna].p1);
    }
    else
    {
        PtrFuncion=(void(*)(uint8_t))tFUNC_TECLADO[cFila][cColumna].PtrFuncSec;
        if(PtrFuncion!=0) (*PtrFuncion)(tFUNC_TECLADO[cFila][cColumna].p2);
    }
}

/**
Busqueda del numero o caracter presionado
@retval Resultado de la operacion
@note  Nota importante.
*/
void tecla_numerica(uint8_t cTecla)
{	
	
//		if(tSys_flag.bModPC = TRUE)return;
//		UART_sendf("tecla numerica Modo PC:%d",tSys_flag.bModPC);
	
		if(tSys_flag.bModPC == TRUE)
		{
//			UART_sendf("Return PC");
			return;
		}
		
    if(cTecla=='C')
    {
        if(tSys_flag.bTecAlfabetico==0)
        {
            bdoblecero=1;
            cTecla='0';
        }
        else
        {
            cTecla=' ';
        }
    }
    //Si es una tecla numerica
    if((tSys_flag.bTecAlfabetico==0)&&(cTecla!=0))
    {		

				if((wAns != 0 && tFlag.bCalc2!= 1 && tFlag.bCalc==1)){
					tControl.cEst=EST_ESPERA;
					tControl.cOp=OP_NO;
					func_calculadora();
				}
        func_evento_teclado_numerico(cTecla,bdoblecero);
        bdoblecero=0;
    }
    //Si es una tecla alfabetica
    if((tSys_flag.bTecAlfabetico==1)&&(cTecla!=0))
    {
        func_teclado_alfabetico(cTecla);
    }
}

