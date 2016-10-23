/**********************************************************
			RFM-mode frame
***********************************************************/
#include "RFM-mode.h"

bit  RFM_TXframe;
bit  RFM_RXframe;
bit  RFM_Testframe;

bit modeframe;
bit dispframe;

unsigned char RFM_modeframe;

void RFM_modeclear(void)
{
	RFM_modeframe = 0;
	dispframe = 0;
	
	RFM_RXframe = 0;
	RFM_TXframe = 0;
	RFM_Testframe = 0;
}
void RFM_mode(void)
{
	if(RFM_modeframe == 0)
	{
		dispframe = 1;
		modeframe = 0;
	}
	else
	{
		dispframe = 0;
		modeframe = 1;
	}
	switch(RFM_modeframe)
	{
		case 1:
				RFM_Testframe = 0;
				RFM_RXframe = 1;
				RFM_TXframe = 0;
				break;
		case 2:
				RFM_Testframe = 0;
				RFM_TXframe = 1;
				RFM_RXframe = 0;
				break;
		case 3:
				RFM_Testframe = 1;
				RFM_RXframe = 1;
				RFM_TXframe = 1;
				break;
		case 4:
				RFM_Testframe = 1;
				RFM_RXframe = 0;
				RFM_TXframe = 0;
				break;
		case 5:
				RFM_modeframe = 0;
				break;
	}
}