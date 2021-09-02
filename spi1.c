#include "includes.h"

#define SPI_FLAG_TFE (0X01)
#define SPI_FLAG_RNE (0X04)
#define SPI_FLAG_BSY (0X10)

//=============================================================================
#define SPI_NCS_HIGH()  do{ \
        GPIOSetValue(0, P0_SPICS, 1);\
    }while(0)

#define SPI_NCS_LOW()   do{ \
        GPIOSetValue(0, P0_SPICS, 0);    \
    }while(0)

#define SPISclkHigh() \
    do{ \
        GPIOSetValue(0, P0_SPICLK, 1);   \
    }while(0)

#define SPISclkLow() \
    do{ \
        GPIOSetValue(0, P0_SPICLK, 0);   \
    }while(0)

#define SPIMOSIHigh() \
    do{ \
        GPIOSetValue(0, P0_SPIMOSI, 1);   \
    }while(0)

#define SPIMOSILow() \
    do{ \
        GPIOSetValue(0, P0_SPIMOSI, 0);   \
    }while(0)

#define SPIMISORead()   (LPC_GPIO0->DATA &(1<<P0_SPIMISO))
//=============================================================================
/*uint16_t SPI1_send(uint8_t data)
{   SPICSLow(); 
    LPC_SSP1->DR=data;//escribe la data a ser transmitida   al registro SPI 
    //while( (LPC_SSP1->SR& SPI_FLAG_TFE) );    // wait until transmit complete
//  while( (LPC_SSP1->SR & SPI_FLAG_RNE) ); // wait until receive complete
    while(!( LPC_SSP1->SR & SPI_FLAG_BSY) );        
            // wait until SPI is not busy anymore
    return LPC_SSP1->DR;        
  // return received data from SPI data register
}*/
    
static void SPIDelay(uint32_t tm)
{
    volatile uint32_t t;

    tm <<= 4;
    t = tm;
    while(t--);
}
//=============================================================================
//static OS_EVENT *spi_sem_event;
static void SPISemInit(void)
{
    //spi_sem_event = OSSemCreate(1);
}

extern void SPISemGet(void)
{
    //uint8_t err;

    //OSSemPend(spi_sem_event, 0, &err);
}

extern void SPISemPut(void)
{
    //OSSemPost(spi_sem_event);
}
//=============================================================================
extern void SPIInit(void)
{
    /* Configure I/O for Flash Chip select */
    GPIOSetDir(0, P0_SPICS, 1);
    LPC_IOCON->PIO0_2 = IOCON_PIO;
    SPICSHigh();

    /* SCLK */
    GPIOSetDir(0, P0_SPICLK, 1);
    LPC_IOCON->SWCLK_PIO0_10 = (2<<3)|(1<<0);
    SPISclkHigh();          // rising edge latch

    /* MOSI */
    GPIOSetDir(0, P0_SPIMOSI, 1);
    LPC_IOCON->PIO0_9 = IOCON_PIO;

    /* MISO */
    GPIOSetDir(0, P0_SPIMISO, 0);
    LPC_IOCON->PIO0_8 = IOCON_PIO;

    SPISemInit();
}
//=============================================================================
extern void SPICSLow(void)
{
    SPI_NCS_LOW();
}

extern void SPICSHigh(void)
{
    SPI_NCS_HIGH();
}
//=============================================================================
extern uint32_t SPISwapDataLSB(uint32_t dat, uint8_t bits)
{
    uint8_t i;
    uint32_t rx;

    for (i=0, rx=0; i<bits; i++)
    {
        if (dat & (1 << i))         // LSB first
        {
            SPIMOSIHigh();
        }
        else
        {
            SPIMOSILow();
        }
        SPISclkLow();
        if(SPIMISORead())
        {
            rx |= (1 << i);
        }
        SPISclkHigh();          // rising edge latch
    }
    return rx;
}

//=============================================================================
extern void SPIShiftDataMSB(uint8_t dat)
{
    uint8_t i;

    i = 7;
    do
    {
        if (dat & (1 << i))         // MSB first
        {
            SPIMOSIHigh();
        }
        else
        {
            SPIMOSILow();
        }
        SPISclkLow();
        SPISclkHigh();          // rising edge latch
    }while(i--);
}
//=============================================================================
extern uint8_t SPISwapDataMSB(uint8_t out)
{
    uint8_t in;
    uint8_t i;

    in = 0;
    i = 7;
    do
    {
        SPISclkLow();       // clk = 0;
        if(out & (1 << i))
        {
            SPIMOSIHigh();
        }
        else
        {
            SPIMOSILow();
        }
        SPIDelay(1);
        if(SPIMISORead())
        {
            in |= (1 << i);
        }
        SPISclkHigh();      // clk = 1;
        SPIDelay(2);
    }while(i--);

    return in;
}
//=============================================================================
extern uint8_t SPISwapDataMSBSlow(uint8_t out)
{
    uint8_t in;
    uint8_t i;

    in = 0;
    i = 7;
    do
    {
        SPISclkLow();       // clk = 0;
        if(out & (1 << i))
        {
            SPIMOSIHigh();
        }
        else
        {
            SPIMOSILow();
        }
        SPIDelay(75);       // 7.5us
        if(SPIMISORead())
        {
            in |= (1 << i);
        }
        SPISclkHigh();      // clk = 1;
        SPIDelay(75);       // 7.5us
    }while(i--);

    return in;
}
//=============================================================================

