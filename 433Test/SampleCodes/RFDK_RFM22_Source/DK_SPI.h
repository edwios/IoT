#ifndef __DK_SPI_h__
#define __DK_SPI_h__

#include "DK_PIC16_App.h"

void SPICmd8bit(u8 WrPara);
u8 SPIRead(u8 adr);
u8 SPIRead8bit(void);
void SPIWrite(word WrPara);
void SPIBurstRead(u8 adr, u8 *ptr, u8 length);
void BurstWrite(u8 adr, u8 *ptr, u8 length);

#endif







