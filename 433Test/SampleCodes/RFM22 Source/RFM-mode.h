/***********************************************************
			RFM_mode.h
***********************************************************/
extern  bit RFM_TXframe;
extern  bit RFM_RXframe;
extern  bit RFM_Testframe;

extern  bit modeframe;
extern  bit dispframe;

extern unsigned char RFM_modeframe;

extern void RFM_modeclear(void);
extern void RFM_mode(void);