#ifndef __DK_Flash_h__
#define __DK_Flash_h__

#include "DK_PIC16_App.h"

#define EEPROM_FirstAddr          0x3820


void FlashErase(u32 FlashAdr);
void FlashWrite(u32 FlashAdr, u8 *array);
void FlashRead(u32 FlashAdr, u8 *array);

#endif







