#ifndef _HARDWARE_H_

#define CUSTLCD_CS (1<<2)
#define MAINLCD_CS (1<<1)

#define P0_RESET		(0)
#define P0_LCDCS0		(1)
#define P0_SPICS		(2)

/* SPI */
#define P0_SPIMISO      (8)
#define P0_SPIMOSI      (9)
#define P0_SPICLK       (10)

/* UART */
#define P1_UART_RXD		(6)
#define P1_UART_TXD		(7)

/* Keyboard */
#define P0_KEYI0    	(3)
#define P0_KEYI1    	(4)
#define P0_KEYI2    	(5)
#define P1_KEYI3		(9)
#define P3_KEYI4		(4)
#define P3_KEYI5		(5)
#define P0_KEYI6		(6)
#define P0_KEYI7		(7)

#define P1_KEYO0		(10)
#define P0_KEYO1		(11)
#define P1_KEYO2		(0)
#define P1_KEYO3		(1)
#define P1_KEYO4		(2)

/* LCD */
#define P1_LCDCD		(5)
#define P1_LCDCS1		(8)
#define P2_LIGHT		(0)

#define ApplicationAddress  0x00001000


#define IOCON_PIO       ((1<<7)|(0<<0))
#define IOCON_R_PIO     ((1<<7)|(1<<0))

#endif

