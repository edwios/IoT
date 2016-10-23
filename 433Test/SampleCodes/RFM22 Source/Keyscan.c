/**********************************************
			Keyscan()
***********************************************/
#include "DK-rfm2xb.h"
#include "Keyscan.h"
#include "RFM-mode.h"

unsigned char keyframe;
unsigned char key1frame;
unsigned char key2frame;
unsigned char key3frame;
unsigned char key4frame;
bit key_free;
bit key_down;

void key_init(void)
{
	keyframe = 0;
	key1frame = 0;
	key2frame = 0;
	key3frame = 0;
	key4frame = 0;
}	
void Keyscan(void)
{
	unsigned char keyvalue;
	if((PORTB&0x0F)!=0x0F)
	{
		for(systime=0;systime<1;);
		if((PORTB&0x0F)!=0x0F)
		{
			key_down = 1;
			key_free = 0;
			keyframe = 1;
			keyvalue = PORTB&0x0F;
			switch(keyvalue)
			{
				case 0x0E:      key1frame = 1;
								RFM_modeframe++;
								if(RFM_modeframe>4)
									RFM_modeframe = 0;
								break;
				case 0x0D:		key2frame = 1;
								break;
				case 0x0B:		key3frame = 1;
								break;
				case 0x07:		key4frame = 1;
								break;
				default:
								break;
			}
		}
	}
	else
	{
		keyframe = 0;
		key_down = 0;
		key_free = 1;
	}
}
