/**
 *  HMI driver
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>

#include "includes.h"

#include "system.h"
#include "demo.h"

extern unsigned long get_ms(void);

#ifdef PROFILING
    static unsigned long start_ticks = 0;
    static unsigned long start_prev_ticks = 0;
    static unsigned long stop_ticks = 0;
#endif

short *lcdInit(int *width, int *height, int *depth)
{
    unsigned int frame_buffer = 0;
    VFR_PARAMETERS* vfr_parameters = GetVFRParameters( 2 );
    if ( 0 != vfr_parameters )
    {
        frame_buffer = MEM_IF_DDR3_EMIF_BASE + vfr_parameters->address;
        *width = vfr_parameters->width;
        *height = vfr_parameters->height;
        *depth = 32;
    }
    
    return (short*)frame_buffer;
}

short *lcdInitLayer(int layer, int color )
{
    return 0;
}

char  *lcdGetLayerAlphaBuffer( int layer )
{
    return 0;
}

/**
 *  Called when HMI is ready to compose and render a frame
 */
int   lcdStartGraphics(void)
{
#ifdef PROFILING
    start_prev_ticks = start_ticks;
    start_ticks = OSTimeGet();
#endif
    return 1;
}

/**
 *  Called when HMI is done with a frame
 */
int   lcdStopGraphics(void)
{
#ifdef PROFILING
    unsigned int frame_count = 0;
    stop_ticks = OSTimeGet();

    if ( ( start_ticks != start_prev_ticks ) &&
        ( start_ticks != stop_ticks ) )
    {
        printf( "%d: update fps = %d(%d ms), render fps = %d (%d ms)\n",
               frame_count,
               1000/(start_ticks - start_prev_ticks),
               start_ticks - start_prev_ticks,
               1000/(stop_ticks - start_ticks),
               stop_ticks - start_ticks
               );
    }

    frame_count++;
#endif
    
    return 1;
}

void   OSgetScreen(int *width, int *height, int *bpp)
{
    
}

void  *OSalloc(size_t size)
{
    return malloc(size);
}

void   OSfree(void* ptr)
{
    free(ptr);
}

void  *OSrealloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

void  *OScalloc(int num, size_t size)
{
    return calloc(num, size);
}

void *OSInputEvent(void)
{
    return 0;
}
