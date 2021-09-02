#include "includes.h"
#include "system.h"
#include "uart.h"
#include <stdarg.h>

UART_t volatile uart;

uint8_t tx_buff[UART_TXBUFFERLEN];
uint8_t rx_buff[UART_RXBUFFERLEN];

void UART_init(uint32_t baudrate)
{
	uint32_t fdiv;
	uint32_t volatile dump;

    /* Desactiva la interrupcion */
	NVIC_DisableIRQ(UART_IRQn);
    /* Configuración UART I/O */
	LPC_IOCON->PIO1_6 &= ~0x07;
	LPC_IOCON->PIO1_6 |= 0x01;      /* UART RXD */
	LPC_IOCON->PIO1_7 &= ~0x07;
	LPC_IOCON->PIO1_7 |= 0x01;      /* UART TXD */
	/* Enable UART clock */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);
	LPC_SYSCON->UARTCLKDIV = 0x1;

	LPC_UART->LCR = IMPRESOR_BIXOLON_350;   /* 8 bits paridad par y un bit de parada */
	fdiv = ((SystemAHBFrequency / LPC_SYSCON->UARTCLKDIV) / 16) / baudrate; /* baudrate */
	LPC_UART->DLM = fdiv >> 8;
	LPC_UART->DLL = fdiv & 0xFF;
	LPC_UART->LCR = 0x1B;           /* DLAB = 0 */
	LPC_UART->FCR = 0x07;	        /* Reinicia el FIFO TX y RX */

	/* Limpia linea de status */
	dump = LPC_UART->LSR;
	/* Se encarga de un inicio limpio sin data en TX o RX FIFO. */
	while ((LPC_UART->LSR & (LSR_THRE|LSR_TEMT)) != (LSR_THRE|LSR_TEMT))
    {
    }
	while ( LPC_UART->LSR & LSR_RDR )
	{
		dump = LPC_UART->RBR;
	}
	/* Inicialización de estructura para transmisión */
	uart.tx_dat = tx_buff;
	uart.tx_index = 0;
	uart.tx_cont = 0;
	uart.tx_busy = FALSE;
    /* Inicialización de estructura para recepción */
	uart.rx_dat = rx_buff;
	UART_BufferRxClear();
    /* Activa interrupcion */
	NVIC_EnableIRQ(UART_IRQn);
    /* Configura el nivel de interrupción */
    NVIC_SetPriority(UART_IRQn, 3);

#if TX_INTERRUPT
	LPC_UART->IER = IER_RBR | IER_THRE | IER_RLS;
#else
	LPC_UART->IER = IER_RBR | IER_RLS;
#endif

}

void UART_send(uint8_t *source, size_t length)
{
    while(uart.tx_busy == TRUE)
    {
    }
	if( length < UART_TXBUFFERLEN )
	{
        /* Copia al buffer de transmision */
        memcpy(uart.tx_dat, source, length);
        uart.tx_cont = length;
        uart.tx_index = 0;
        /* Inicia la transmision */
        LPC_UART->THR = uart.tx_dat[uart.tx_index++];
        uart.tx_busy = TRUE;
	}
    else
    {
		// TODO RETORNAR FALSE O ALGO ASI
    }
}

#if DEBUG_ENABLE  
void UART_sendf(void *format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    UART_send((uint8_t *)"debug> ", 7);
    UART_send((uint8_t *)buffer, strlen(buffer));
    UART_send((uint8_t *)"\r\n", 2);
}
#endif

void UART_BufferRxClear(void)
{
    uart.rx_index = uart.rx_cont = 0;
    uart.rx_state = RXESPERA;
}

 void UART_IRQHandler(void)
{
	uint8_t IIRValue;
    uint8_t LSRValue;
    uint8_t volatile dummy;
    uint8_t volatile c;

	IIRValue = LPC_UART->IIR;
	IIRValue >>= 1;
	IIRValue &= 0x07;
	if (IIRValue == IIR_RLS)
	{
		LSRValue = LPC_UART->LSR;

		if (LSRValue & (LSR_OE | LSR_PE | LSR_FE | LSR_RXFE | LSR_BI))
		{
			dummy = LPC_UART->RBR;
			return;
		}
		if (LSRValue & LSR_RDR)
		{
			c = LPC_UART->RBR;
            if(uart.tx_cont > UART_RXBUFFERLEN)
            {
                UART_BufferRxClear();
            }
            if(uart.rx_state == RXESPERA)
            {
                if(c == ACK)
                {
                    tSys_flag.bAckRec = TRUE;
                }
                if(c == NACK)
                {
                    tSys_flag.bNackRec = TRUE;
                }
                /* Se espera para procesar un nuevo comando */
                if((c == STX) && (tSys_flag.bCmdRecive == FALSE))
                {
                    uart.rx_cont = 0;
                    uart.rx_index = 0;
                    uart.rx_state = RXDATA;
                }
            }
            else if(uart.rx_state == RXDATA)
            {
                if(c == ETX)
                {
                    uart.rx_state = RXCRC;
                }
                else
                {
                    uart.rx_dat[uart.rx_cont] = c;
                    uart.rx_cont++;
                }
            }
            else if(uart.rx_state == RXCRC)
            {
                uart.rx_dat[uart.rx_cont] = c;
                ++uart.rx_cont;
                if (tSys_flag.bWaitData == FALSE)
                {
                    tSys_flag.bCmdRecive = TRUE;
                }
                else
                {
                    tSys_flag.bWaitData = FALSE;
                }
                uart.rx_state = RXBLOCK; //TODO
            }
            else
            {
                if(tSys_flag.bCmdRecive == FALSE)
                {
                    UART_BufferRxClear();
                }
            }
		}
	}
	else if (IIRValue == IIR_RDA)
	{
        c = LPC_UART->RBR;
        if(uart.tx_cont > UART_RXBUFFERLEN)
        {
            UART_BufferRxClear();
        }
        if(uart.rx_state == RXESPERA)
        {
            if (c == ACK)
            {
                tSys_flag.bAckRec = TRUE;
            }
            if (c == NACK)
            {
                tSys_flag.bNackRec = TRUE;
            }
            /* Se espera para procesar un nuevo comando */
            if ((c == STX) && (tSys_flag.bCmdRecive == FALSE))
            {
                uart.rx_cont = 0;
                uart.rx_index = 0;
                uart.rx_state = RXDATA;
            }
        }
        else if (uart.rx_state == RXDATA)
        {
            if (c == ETX)
            {
                uart.rx_state = RXCRC;
            }
            else
            {
                uart.rx_dat[uart.rx_cont] = c;
                ++uart.rx_cont;
            }
        }
        else if (uart.rx_state == RXCRC)
        {
                uart.rx_dat[uart.rx_cont] = c;
                ++uart.rx_cont;
                if (tSys_flag.bWaitData == FALSE)
                {
                    tSys_flag.bCmdRecive = TRUE;
                }
                else
                {
                    tSys_flag.bWaitData = FALSE;
                }
                uart.rx_state = RXBLOCK; //TODO
        }
        else
        {
            if (tSys_flag.bCmdRecive == FALSE)
            {
                UART_BufferRxClear();
            }
        }
	}
	else if (IIRValue == IIR_CTI)
    {
	}
	else if (IIRValue == IIR_THRE)
	{
		LSRValue = LPC_UART->LSR;
		if (LSRValue & LSR_THRE)
		{
			if (uart.tx_index < uart.tx_cont)
			{
				LPC_UART->THR = uart.tx_dat[uart.tx_index];
				++uart.tx_index;
			}
			else
			{
				uart.tx_busy = FALSE;
				uart.tx_index = 0;
				uart.tx_cont = 0;
			}
		}
	}
	return;
}

