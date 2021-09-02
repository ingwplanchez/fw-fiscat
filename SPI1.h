#ifndef __SPI1_H__
#define __SPI1_H__

extern void SPISemGet(void);
extern void SPISemPut(void);
extern void SPIInit(void);
extern void SPICSLow(void);
extern void SPICSHigh(void);
extern uint32_t SPISwapDataLSB(uint32_t dat, uint8_t bits);
extern void SPIShiftDataMSB(uint8_t dat);
extern uint8_t SPISwapDataMSB(uint8_t dat);
extern uint8_t SPISwapDataMSBSlow(uint8_t out);

#endif

