#include "DK-rfm2xb.h"

void Beep_init(void)
{
	TRISC2 = 1;
	PR2 = 0x63;           // 2.5M
	CCP1CON = 0x0F;
	CCPR1L = 0x32;
	TMR2IF = 0;
	T2CON = 0x06;
	while(!TMR2IF);
}
void BeepOn(void)
{
	TRISC2 = 0;
}
void BeepOff(void)
{
	TRISC2 = 1;
}
void led_tx()
{
	TLED4 =1;
	for(systime=0;systime<100;);
	TLED5 = 1;
	TLED3 = 1;
	for(systime=0;systime<100;);
	TLED1 = 1;
	TLED2 = 1;
	TLED6 = 1;
	for(systime=0;systime<100;);
	TLED4 =0;
	for(systime=0;systime<100;);
	TLED5 = 0;
	TLED3 = 0;
	for(systime=0;systime<100;);
	TLED1 = 0;
	TLED2 = 0;
	TLED6 = 0;
	for(systime=0;systime<100;);
}
void led_rx()
{
	LATD &= 0x01;
	RLED1 = 1;
	for(systime=0;systime<200;);
}
void led_alltx()
{
	TLED1 = 1;
	TLED2 = 1;
	TLED3 = 1;
	TLED4 = 1;
	TLED5 = 1;
	TLED6 = 1;
	RLED1 = 0;
}
void led_off()
{
	TLED1 = 0;
	TLED2 = 0;
	TLED3 = 0;
	TLED4 = 0;
	TLED5 = 0;
	TLED6 = 0;
	RLED1 = 0;
}