#ifndef _F25L016A_H_
#define _F25L016A_H_
//======================================================================================================
extern void F25L_Init(void);
extern void F25L_ReadJedecId(uint8_t *buf);
extern void F25L_ReadId(uint8_t *buf);
extern uint8_t F25L_ReadElectronicSignature(void);
extern void F25L_Read(uint32_t addr, uint8_t *buf, uint32_t bytes);
extern void F25L_FastRead(uint32_t addr, uint8_t *buf, uint32_t bytes);
extern uint8_t F25L_BuffProgram(uint32_t addr, uint8_t *dat, uint32_t bytes);
extern void F25L_BlockErase(uint16_t block);
extern void F25L_SectorErase(uint16_t sector);
extern void F25L_ChipErase(void);
extern uint8_t F25L_BuffCmpCanWrite(uint32_t addr, uint8_t *buf, uint32_t bytes);
//======================================================================================================
#endif
