#include "system.h"
#include "cmd.h"
#include "uart.h"
#include "display.h"
#include "func_plu.h"
#include "func.h"
#include "Timer.h"
#include "Includes.h"
#include "pc.h"

//******************************************************************************//
//                         DEFINICIONES GENERALES                                   //
//******************************************************************************//


//**********************DECLARACION DE VARIABLES LOCALES************************//


//******************************************************************************//
//                                          PROTOTIPOS DE FUNCIONES PRIVADAS                                                //
//******************************************************************************//
uint8_t recibo_ack(uint8_t cContAck, int16_t nrodemensaje);
uint8_t cal_bcc(uint8_t *pBuf, int16_t iLen);
uint8_t cmd_wait_resp(void);
//******************************************************************************//
//                                                  DEFINICION DE FUNCIONES                             //
//******************************************************************************//

/**
Funcion de encio de comandos
@cBuffer Trama del comando
@iLen Tamaño de la trama
@cContAck Nro de Ack que se debe recibir
@bMensajeIni Indicador de impresion de mensaje inicial(b0) y final(b1)
@nrodemensaje Mensaje que debe aparecer en pantalla luego de ejecutado el comando
@note  Nota importante.
*/

uint8_t cmd_send_recv(uint8_t *buffer, size_t length, uint8_t cmd)
{
    uint8_t resp;
    uint8_t aux[256];

    /* Limpia el buffer */
    memset(aux, 0 , sizeof(aux));

    if (cmd != FALSE)
    {
        aux[0] = STX;
        memcpy(aux + 1, buffer, length);
        aux[length + 1] = ETX;
        aux[length + 2] = cal_bcc(aux + 1, length + 1);
        length += 3;
    }
    else
    {
        memcpy(aux, buffer, length);
    }

    UART_BufferRxClear();
    tSys_flag.bAckRec = FALSE;
    tSys_flag.bNackRec = FALSE;
    tSys_flag.bWaitData = TRUE;
    UART_send(aux, length);

    set_timeout(10000);
    while (!timeout_expire() && (uart.rx_state < RXBLOCK))
    {
    }

    if (timeout_expire())
    {
        resp = FALSE;
    }
    else
    {
        resp =  TRUE;
    }

    return resp;

}

uint8_t cmd_send(uint8_t *cBuffer, uint16_t iLen, uint8_t cContAck, uint8_t bMensajeIni, int16_t nrodemensaje)
{
    uint8_t cResp=FALSE, cAux[256]={NULL};

    //Armado del comando segun protocolo
    cAux[0] = 0x02;
    memcpy(cAux + 1, cBuffer, iLen);
    cAux[iLen + 1] = 0x03;
    cAux[iLen + 2] = cal_bcc(cAux + 1, iLen + 1);
    iLen += 3;

    //Mensaje de envio
    if (nrodemensaje >= 0) mensaje_display(MENSAJE_ENVIO,DISPLAY_OPERADOR);

    //Envio de comando
    UART_BufferRxClear();
    tSys_flag.bAckRec = FALSE;
    tSys_flag.bNackRec = FALSE;
    UART_send(cAux, iLen);

    //Recepcion de respuesta

    cResp = recibo_ack(cContAck, nrodemensaje);

    //Mensaje Final

    return cResp;
}

uint8_t cmd_wait_resp(void)
{

    set_timeout(10000);

    while((tSys_flag.bAckRec == FALSE ) && (tSys_flag.bNackRec == FALSE ) && (!timeout_expire()));

    if(timeout_expire())
        return FALSE;

    return TRUE;

}

uint8_t cal_bcc(uint8_t *pBuf, int16_t iLen)
{
    uint8_t  cCRC = 0;
    int16_t i;

    for(i=0; i<iLen; i++) cCRC^= (uint8_t)pBuf[i];

    return cCRC;
}

//******************************************************************************//
//                                                  PROCESAMIENTO DE COMANDOS                           //
//******************************************************************************//
uint8_t recibo_ack(uint8_t cContAck, int16_t nrodemensaje)
{
    uint8_t cResp=FALSE, i=0;

    //Recepcion de la respuesta
    for(i=0;i<cContAck;i++)
    {
        //En caso de timeout se rompe el ciclo
        if(cmd_wait_resp()==FALSE)
        {
            tSys_flag.bTimeout=1;
            break;
        }
        //Si se respondio NACK no se espera las demas respuestas
        if(tSys_flag.bNackRec==1)break;
    }

    //Envio exitoso
    if(tSys_flag.bAckRec==1)
    {
        tSys_flag.bAckRec=0;
        tSys_flag.bShiftActivo=0;
        tSys_flag.bDecimal=0;
        if(nrodemensaje >= 0)
        {
            mensaje_display(nrodemensaje, DISPLAY_OPERADOR);
            OSTimeDly(1000);
        }
        cResp=TRUE;
    }
    //Comando invalido o timeout
    else if(nrodemensaje>=0)
    {
        if(tSys_flag.bTimeout==FALSE)
        {
            mensaje_display(MENSAJE_ERROR_COMANDO,DISPLAY_OPERADOR);
        }
        else
        {
            mensaje_display(MENSAJE_TIMEOUT,DISPLAY_OPERADOR);
            }
        OSTimeDly(1000);
        display_erase_line(AMBAS_LINEAS,DISPLAY_OPERADOR);
    }
    func_clean();
    func_plu_ini(TRUE);
    tSys_flag.bDecimal=0;
    tSys_flag.bShiftActivo=0;
    tSys_flag.bNackRec=0;
    tSys_flag.bAckRec=0;

    return cResp;
}

void cmd_task(void)
{
    uint8_t crc, cCmd, cSubCmd;
    uint8_t trama[2];
    uint8_t cEnviarResp = FALSE, cResp = FALSE;

    crc = cal_bcc((uint8_t *)uart.rx_dat, uart.rx_cont);

    if((crc ^ ETX) == 0)
    {
        cCmd = uart.rx_dat[0];
        switch (cCmd)
        {
            case 0x54:  /* Empieza modo PC -> T */ 
            {
                tSys_flag.bModPC = TRUE;
								
                cResp = TRUE;
                cEnviarResp = TRUE;
                tSys_flag.bTerceralinea = 0;
                tSys_flag.bErase=1;
                display_erase_line(AMBAS_LINEAS, DISPLAY_AMBOS);
                tSys_flag.bTerceralinea = 0;
                tSys_flag.bErase=1;
                mensaje_display(MENSAJE_MODO_PC, DISPLAY_AMBOS);
                break;
            }
            case 0x53:  /* Termina modo PC -> S */ 
            {
                tSys_flag.bModPC = FALSE;
                cResp = TRUE;
                cEnviarResp = TRUE;
                display_message_ini();
                break;
            }
            case 0x50:  /* Guardar registro, requiere modo PC -> P*/
            {
                if (tSys_flag.bModPC == TRUE)
                {
                    cSubCmd = uart.rx_dat[1];
                    cResp = pc_cmd_guardar_registro(cSubCmd, (uint8_t *)(uart.rx_dat + 2), uart.rx_cont - 2);
                    cEnviarResp = TRUE;
                }
                break;
            }
            case 0x55: /* Muestra registro U*/
            {
                if (tSys_flag.bModPC == TRUE)
                {
                    cSubCmd = uart.rx_dat[1];
                    cResp = pc_cmd_leer_registro(cSubCmd, (uint8_t *)(uart.rx_dat + 2), uart.rx_cont - 2);
                    cEnviarResp = FALSE;
                }
                break;
            }
            case COMANDO_VERSION:
            {
                if (tSys_flag.bModPC == TRUE)
                {
                    cResp = pc_version();
                    cEnviarResp = FALSE;
                }

                break;
            }
            default:
            {
                cResp = FALSE;
                cEnviarResp = TRUE;
                break;
            }
        }
    }
    else
    {
        cResp = FALSE;
        cEnviarResp = TRUE;
    }

    if(cEnviarResp == TRUE)
    {
        if(cResp == TRUE)
        {
            trama[0] = ACK;
        }
        else
        {
            trama[0] = NACK;
        }
        UART_send(trama, 1);
    }
    UART_BufferRxClear();
}


