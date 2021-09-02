#include "config.h"
#include "system.h"
#include "includes.h"
#include "F25L016A.h"
#include "Keyscan.h"
#include "keyboard.h"
#include "display.h"
#include "func_plu.h"
#include "func.h"
#include "gpio.h"
#include "uart.h"
#include "cmd.h"
#include "pc.h"
#include "reporte_plu.h"
#include "mem.h"

void ReAllocateNVIC(void)
{
    size_t size;
    uint32_t *src;
    uint32_t *dst;

    /* copy vector table */
    src = (uint32_t *)ApplicationAddress;
    dst = (uint32_t *)0x10000000;
    size = 192;
    __disable_irq();
    while(size > 0)
    {
        *dst++ = *src++;
        size -= 4;
    }
     LPC_SYSCON->SYSMEMREMAP = 0x1;     /* remap to internal RAM */
    __enable_irq();
}

void system_init(void)
{
    ReAllocateNVIC();
    SystemInit();
    GPIOInit();
    Tim4Init();
    UART_init(9600);
    SPIInit();    
    GPIOSetDir(2, 0, 1);
    F25L_Init();
    OSTimeDly(100);
    SPEC12832Init(); // Inicializador del Display
    
    memset(tSys_flag.cFlag, 0, sizeof(tSys_flag.cFlag));
    memset(g_tasas.valor, 0, sizeof(g_tasas.valor));
}

void Anulacion(void){
		if((tAns.bAnulacionActiva == 1 && tAns.bAnularFactura == 1)||(tAns.bAnulacionActiva == 1 && tAns.bAnularPlu == 1))
		{
			tControl.cOp=OP_CLAVE_S; 
			tSys_flag.bTecAlfabetico=FALSE;
			tSys_flag.bIngresoDoble=FALSE;

			func_clean();
			func_print_etiqueta(MENSAJE_CLAVE, 0, 6, LINEA_SUPERIOR, DISPLAY_OPERADOR); 
			tAns.bAnulacionActiva = 0;
		}
}

int main(void)
{
	system_init();
	Keyboard_init();
	display_ini();
	func_plu_ini(TRUE);
	func_clean();
	memset(&tControl, 0, sizeof(tControl));
	func_config_leer();
	tSys_flag.bTerceralinea = 1; 
	
	tAns.bzerokey = 0;
	tAns.bBorrarPlu = 0;
	tAns.bBorrarACC = 0;
	tAns.bAnularFactura = 0;
	tAns.bAnularPlu = 0;
	tAns.bAnulacionActiva = 0;	
	tAns.bDone = 0;
	tAns.bVenta = 1;
	tFlag.bCalc = 0;
	
	display_message_ini();
	//F25L_ChipErase();
	for (;;)
	{		
		/* Revisa el estatus del teclado e impresor */
		func_status_impresor();
		/* Atiende los eventos del teclado */
		keyScan_task();
		
		/* Procesa el borrado total de los PLU */
		func_borrado_total_plu();
		/* Procesa el borrado total de los DEP */
		func_borrar_accesos();
		/* Procesa el Reporte PLU */
		Reporte_PLU();
		/* Procesa el Reporte DEP */
		Reporte_DEP();
		/* Atiende los eventos de comandos por puerto */
		
		if (tSys_flag.bCmdRecive)
		{
				tSys_flag.bCmdRecive = FALSE;
				cmd_task();
		}
			
		if (tSys_flag.bKeyboardScan)
		{   tSys_flag.bKeyboardScan = FALSE;
				Keyboard_scan();
		}
		
		if (tAns.bAnularFactura == 1 || tAns.bAnularPlu == 1)
		{
			Anulacion();
		}
			
		else
		{
			if (tSys_flag.bDisplayTick)
			{
					tSys_flag.bDisplayTick = FALSE;
					Display_rotateTick();
			}
		}
	}
}


