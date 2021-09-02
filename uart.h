#ifndef __UART_H__
#define __UART_H__

#define DEBUG_ENABLE 		1

#define IMPRESOR_BIXOLON_350 0X9B
#define RS485_ENABLED   0
#define TX_INTERRUPT    1   /* 0 si TX usa Polling, 1 por interrupción. */
#define MODEM_TEST      0

#define IER_RBR         (0x01 << 0)
#define IER_THRE        (0x01 << 1)
#define IER_RLS         (0x01 << 2)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01

#define LSR_RDR         (0x01 << 0)
#define LSR_OE          (0x01 << 1)
#define LSR_PE          (0x01 << 2)
#define LSR_FE          (0x01 << 3)
#define LSR_BI          (0x01 << 4)
#define LSR_THRE        (0x01 << 5)
#define LSR_TEMT        (0x01 << 6)
#define LSR_RXFE        (0x01 << 7)

/* RS485 */
#define RS485_NMMEN     (0x1 << 0)
#define RS485_RXDIS     (0x1 << 1)
#define RS485_AADEN     (0x1 << 2)
#define RS485_SEL       (0x1 << 3)
#define RS485_DCTRL     (0x1 << 4)
#define RS485_OINV      (0x1 << 5)

#define RXTIMEOUT       5
#define TXTIMEOUT       5

#define UART_TXBUFFERLEN  136
#define UART_RXBUFFERLEN  192

#define RXESPERA        0
#define RXDATA          1
#define RXCRC           2
#define RXBLOCK         3


typedef struct {
    /* Recepcion */
    uint8_t *rx_dat;
    size_t rx_index;
    size_t rx_cont;
    uint8_t rx_state;
    /* Transmision */
    uint8_t *tx_dat;
    size_t tx_index;
    size_t tx_cont;
    uint8_t tx_busy;
} UART_t;

extern UART_t volatile uart;

void UART_init(uint32_t baudrate);
void UART_send(uint8_t *source, size_t length);
void UART_BufferRxClear(void);

#if DEBUG_ENABLE 
	void UART_sendf(void *format, ...);
#else
	#define UART_sendf(format, ...)
#endif 
	
#endif  /* __UART_H__ */

