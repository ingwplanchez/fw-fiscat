#include "includes.h"

#define opcodeStatusRegisterRead                                (0x05)
#define BUSY                                                    (1 << 0)
#define WEL                                                     (1 << 1)
#define BP0                                                     (1 << 2)
#define BP1                                                     (1 << 3)
#define BP2                                                     (1 << 4)
#define RESERVED                                                (1 << 5)
#define AAI                                                     (1 << 6)
#define BPL                                                     (1 << 7)
#define opcodeWriteEnable                                       (0x06)
#define opcodeWriteDisable                                      (0x04)
#define opcodeBusyOutputEnable                                  (0x70)
#define opcodeBusyOutputDisable                                 (0x80)
#define opcodeStatusRegisterWriteEnable                         (0x50)
#define opcodeStatusRegisterWrite                               (0x01)
#define opcodeReadElectronicSignature                           (0xab)
#define opcodeReadJedecId                                       (0x9f)
#define opcodeReadId                                            (0x90)

#define opcodeRead                                              (0x03)
#define opcodeFastRead                                          (0x0b)
#define opcodeByteProgram                                       (0x02)
#define opcodeAAIWordProgram                                    (0xad)
#define opcodePageProgram                                       (0x02)
#define opcodeBlockErase                                        (0xd8)
#define opcodeSectorErase                                       (0x20)
#define opcodeChipErase                                         (0xc7)

static uint8_t F25L_manufactory;

static void F25L_ShiftOutAddrWrite(uint8_t opcode, uint32_t addr)
{
    SPIShiftDataMSB(opcode);
    SPIShiftDataMSB(addr >> 16);
    SPIShiftDataMSB(addr >> 8);
    SPIShiftDataMSB(addr >> 0);
}

static uint8_t F25L_StatusRegisterRead(void)
{
    uint8_t ret;

    SPICSLow();
    SPIShiftDataMSB(opcodeStatusRegisterRead);
    ret = SPISwapDataMSB(0xFF);
    SPICSHigh();

    return ret;
}

static void F25L_WriteEnable(void)
{
    SPICSLow();
    SPIShiftDataMSB(opcodeWriteEnable);
    SPICSHigh();
}

static void F25L_WriteDisable(void)
{
    SPICSLow();
    SPIShiftDataMSB(opcodeWriteDisable);
    SPICSHigh();
}

#if 0
static void F25L_BusyOutputEnable(void)
{
    SPICSLow();
    SPISwapDataMSB(opcodeBusyOutputEnable);
    SPICSHigh();
}
#endif

#if 0
static void F25L_BusyOutputDisable(void)
{
    SPICSLow();
    SPISwapDataMSB(opcodeBusyOutputDisable);
    SPICSHigh();
}
#endif

static void F25L_StatusRegisterWriteEnable(void)
{
    SPICSLow();
    SPIShiftDataMSB(opcodeStatusRegisterWriteEnable);
    SPICSHigh();
}

static void F25L_StatusRegisterWrite(uint8_t status)
{
    F25L_StatusRegisterWriteEnable();

    SPICSLow();
    SPIShiftDataMSB(opcodeStatusRegisterWrite);
    SPIShiftDataMSB(status);
    SPICSHigh();
}

extern void F25L_Init(void)
{
    /* Deselect the FLASH: Chip Select high */
    SPICSHigh();
}

//extern void F25L_ReadJedecId(uint8_t *buf)
//{
//  SPISemGet();
//  SPICSLow();
//  SPISwapDataMSB(opcodeReadJedecId);
//  *buf++ = F25L_manufactory = SPISwapDataMSB(0xFF);   // Manufactory
//  *buf++ = SPISwapDataMSB(0xFF);  // ID(15~8)
//  *buf = SPISwapDataMSB(0xFF);    // ID(7~0)
//  SPICSHigh();
//  SPISemPut();
//}

//extern void F25L_ReadId(uint8_t *buf)
//{
//  SPISemGet();
//  SPICSLow();
//  SPIShiftDataMSB(opcodeReadId);
//  SPIShiftDataMSB(0);
//  SPIShiftDataMSB(0);
//  SPIShiftDataMSB(0);
//  *buf++ = F25L_manufactory = SPISwapDataMSB(0xFF);   // Manufactory
//  *buf = SPISwapDataMSB(0xFF);    // ID(7~0)
//  SPICSHigh();
//  SPISemPut();
//}

//extern uint8_t F25L_ReadElectronicSignature(void)
//{
//  uint8_t ret;

//  SPISemGet();
//  SPICSLow();
//  SPIShiftDataMSB(opcodeReadElectronicSignature);
//  ret = SPISwapDataMSB(0xFF);
//  SPICSHigh();
//  SPISemPut();

//  return ret;
//}

extern void F25L_Read(uint32_t addr, uint8_t *buf, uint32_t bytes)
{
    SPISemGet();
    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeRead, addr);
    for (; bytes; bytes--)
    {
        *buf++ = SPISwapDataMSB(0xFF);
    }
    SPICSHigh();
    SPISemPut();
}

extern uint8_t F25L_BuffCmpCanWrite(uint32_t addr, uint8_t *buf, uint32_t bytes)
{
    SPISemGet();
    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeRead, addr);
    for (; bytes; bytes--,buf++)
    {
        if ((*buf & SPISwapDataMSB(0xFF)) != *buf)
        {
            SPICSHigh();
            SPISemPut();
            return FALSE;
        }
    }
    SPICSHigh();
    SPISemPut();
    return TRUE;
}

//extern void F25L_FastRead(uint32_t addr, uint8_t *buf, uint32_t bytes)
//{
//  SPISemGet();
//  SPICSLow();
//  F25L_ShiftOutAddrWrite(opcodeFastRead, addr);
//  SPIShiftDataMSB(0xff);  // dummy
//  for (; bytes; bytes--)
//  {
//      *buf++ = SPISwapDataMSB(0xFF);
//  }
//  SPICSHigh();
//  SPISemPut();
//}

static uint8_t F25L_ByteProgram(uint32_t addr, uint8_t dat)
{
    uint8_t ret;

    SPISemGet();
    F25L_StatusRegisterWrite(0x00);
    F25L_WriteEnable();
    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeByteProgram, addr);
    SPIShiftDataMSB(dat);
    SPICSHigh();
    while (F25L_StatusRegisterRead() & BUSY);

    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeRead, addr);
    if(dat == SPISwapDataMSB(0xFF))
    {
        ret = TRUE;
    }
    else
    {
        ret  = FALSE;
    }
    SPICSHigh();

    SPISemPut();

    return ret;
}

static uint8_t F25L_AAIWordProgram(uint32_t addr, uint8_t *dat, uint32_t words)
{
    uint8_t *p;
    uint32_t i;
    uint8_t ret;

    if(words == 0) return TRUE;

    p = dat;
    i = words;
  //LPC_SSP1->DR='A';
    SPISemGet();
    F25L_StatusRegisterWrite(0x00);

    F25L_WriteEnable();
    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeAAIWordProgram, addr);
    SPIShiftDataMSB(*p++);
    SPIShiftDataMSB(*p++);
    SPICSHigh();
    while (F25L_StatusRegisterRead() & BUSY);
    i--;
    for (; i; i--)
    {
        SPICSLow();
        SPIShiftDataMSB(opcodeAAIWordProgram);
        SPIShiftDataMSB(*p++);
        SPIShiftDataMSB(*p++);
        SPICSHigh();
        while (F25L_StatusRegisterRead() & BUSY);
    }
    F25L_WriteDisable();
    while (F25L_StatusRegisterRead() & BUSY);

    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeRead, addr);
    for (i=(words << 1),ret=TRUE; i; i--)
    {
        if(*dat != SPISwapDataMSB(0xFF))
        {
            ret = FALSE;
            break;
        }
        dat++;
    }
    SPICSHigh();

    SPISemPut();
    return ret;
}

static uint8_t F16_PageProgram(uint32_t addr, uint8_t *dat, uint32_t len) //24 bit address
{
    uint8_t *p;
    uint32_t i;
    uint8_t ret;

    if(len == 0) return TRUE;
    p = dat;
    i = len;
    ret = TRUE;

    SPISemGet();
    F25L_StatusRegisterWrite(0x00);
    F25L_WriteEnable();
    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodePageProgram, addr);
    while(i--)
    {
        SPIShiftDataMSB(*p++);
    }
    SPICSHigh();
    while (F25L_StatusRegisterRead() & BUSY);

    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeRead, addr);
    for (; len; len--)
    {
        if (*dat++ != SPISwapDataMSB(0xFF))
        {
            ret = FALSE;
            break;
        }
    }
    SPICSHigh();

    SPISemPut();
    return ret;
}

extern uint8_t F25L_BuffProgram(uint32_t addr, uint8_t *dat, uint32_t bytes)
{
    uint8_t ret = TRUE;
    uint32_t max_len;

    switch(F25L_manufactory)
    {
    case 0x1c:  // EON
    case 0xc2:  // MXIC
    case 0xef:  // Winbond
    default:
        while(bytes)
        {
            max_len = 0x100 - (addr & 0xff);
            if(max_len > bytes)
            {
                max_len = bytes;
            }
            ret &= F16_PageProgram(addr, dat, max_len);
            addr += max_len;
            dat += max_len;
            bytes -= max_len;
        }
        break;
    case 0x8c:  // ESMT
        if (addr & 0x01)
        {
            ret &= F25L_ByteProgram(addr,*dat);
            addr++;
            dat++;
            bytes--;
            if (bytes == 0)
            {
                return ret;
            }
        }
        ret &= F25L_AAIWordProgram(addr,dat,bytes/2);
        if (bytes & 0x01)
        {
            ret &= F25L_ByteProgram(addr+(bytes/2)*2,*(dat + bytes-1));
        }
        break;
    }
    return ret;
}

//extern void F25L_BlockErase(uint16_t block)
//{
//  SPISemGet();
//  F25L_StatusRegisterWrite(0x00);

//  F25L_WriteEnable();

//  SPICSLow();
//  F25L_ShiftOutAddrWrite(opcodeBlockErase, (block << 16));
//  SPICSHigh();

//  while (F25L_StatusRegisterRead() & BUSY);
//  SPISemPut();
//}

extern void F25L_SectorErase(uint16_t sector)
{
    SPISemGet();
    F25L_StatusRegisterWrite(0x00);

    F25L_WriteEnable();

    SPICSLow();
    F25L_ShiftOutAddrWrite(opcodeSectorErase, (sector << 12));
    SPICSHigh();

    while (F25L_StatusRegisterRead() & BUSY);
    SPISemPut();
}

extern void F25L_ChipErase(void)
{
    SPISemGet();
    F25L_StatusRegisterWrite(0x00);

    F25L_WriteEnable();

    SPICSLow();
    SPISwapDataMSB(opcodeChipErase);
    SPICSHigh();

    while (F25L_StatusRegisterRead() & BUSY);
    SPISemPut();
}

