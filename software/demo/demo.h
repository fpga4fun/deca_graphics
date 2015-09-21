/**
 *	demo.h
 */
#ifndef _DEMO_H_
#define _DEMO_H_

#define SCREEN_WIDTH (1280)
#define SCREEN_HEIGHT (800)
typedef struct
{
    unsigned int vfr_index;
    unsigned int width;
    unsigned int height;
    unsigned int address;
    unsigned int words_divider;
    unsigned int cpr_halve;
    unsigned int vfr_register;
}
VFR_PARAMETERS;

int CheckDDR3();
int InitVIP();

VFR_PARAMETERS* GetVFRParameters( unsigned int index );

int Draw();

#endif /* _DEMO_H_ */