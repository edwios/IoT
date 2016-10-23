// ========================================================
/// @file       DK_Flash.c
/// @brief      PIC16F1519 flash Read/Write
/// @version    V1.0
/// @date       2013/04/26
/// @company    HOPE MICROELECTRONICS Co., Ltd.
/// @website    http://www.hoperf.com
/// @author     Geman Deng
/// @mobile     +86-013244720618
/// @tel        0755-82973805 Ext:846
// ========================================================
#include "DK_Flash.h"


/**********************************************************
**Name:     FlashRead
**Function: Read flash 
**Input:    FlashAdr, Address (32bit)
            *array, Data (byte)
**Output:   none
**********************************************************/
void FlashRead(u32 FlashAdr, u8 *array)
{ 
	u8 i;
	
	CFGS  = 0;				                                       //Access Flash, but not UserID  or Configuration
	PMADR = FlashAdr;		                                     //Send address
	
	for(i=0; i<32; i++)	                                     //32bit
	{
		RD    = 1;			                                       //Enable Read
		asm("NOP");			                                       //wait for it
		asm("NOP");			                                       //must be 2*NOP	
		array[i] = (u8)(PMDAT&0x00FF);	                       //Read
		PMADR++;
	}
	RD    = 0;				                                       //Read over
}

/**********************************************************
**Name:     FlashUnlock
**Function: Unlock Flash£¬use before write or erase
**Input:    none
**Output:   none
**********************************************************/
void FlashUnlock(void)
{
	PMCON2 = 0x55;
	PMCON2 = 0xAA;
	WR     = 1;
	asm("NOP");
	asm("NOP");
}

/**********************************************************
**Name:     FlashErase
**Function: Erase Flash
**Input:    FlashAdr, Address
**Output:   none
**********************************************************/
void FlashErase(u32 FlashAdr)
{
	GIE  = 0;					                                      //disable all interrupt
	CFGS = 0;					                                      //Access Flash
	PMADR = FlashAdr;	                                      //set Flash Address
	FREE = 1;					                                      //for erase flash
	WREN = 1;					                                      //enable write flash
	FlashUnlock();
	
	asm("NOP");				                                      //wait for 2ms
	
	WREN = 0;					                                      //disable write or erase flash
	GIE  = 1;					
}

/**********************************************************
**Name:     FlashWrite
**Function: Write data to flash
**Input:    FlashAdr, Address
            *array, Data
**Output:   none
**********************************************************/
void FlashWrite(u32 FlashAdr, u8 *array)
{ 
	u8 i;
	
	GIE  = 0;					                                       //disable all interrupt
	
	PMADR = FlashAdr;	                                       //set Flash Address
	
	CFGS = 0;					                                       //Access Flash 
	FREE = 0;
	LWLO = 1;					                                       //only write to latches
	WREN = 1;					                                       //enable write flash
	for(i=0;i<32;)
	{
		PMDATL = array[i];
		PMDATH = 0;
		i++;
		if(i<32)
		{
			FlashUnlock();
			PMADR++;
		}
		else
		{
			LWLO = 0;			                                       //then write to flash
			FlashUnlock();
			asm("NOP");		                                       //wait for 2ms		
			break;		
		}
	}	
	WREN = 0;					                                       //disable write or erase flash
	GIE  = 1;
}
