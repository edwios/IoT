/***********************************************************
                 Keyscan.h
************************************************************/

extern unsigned char keyframe;
extern unsigned char key1frame;
extern unsigned char key2frame;
extern unsigned char key3frame;
extern unsigned char key4frame;
extern bit key_free;
extern bit key_down;

extern void Keyscan(void);
extern void key_init(void);