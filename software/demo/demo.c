/**
 *	demo.c
 *
 *  Graphics demo for MAX10 Deca board
 *  Ting Cao, 09/2015
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "includes.h"

#include "demo.h"
#include "io.h"
#include "vip_registers.h"

VFR_PARAMETERS VFR_Parameters[] = {
    /* VFR0 - not implemented, Quartus can't synthesize the design with 3 VFRs */
    {
        0,
        1024, 768,
        0,
        /* words' divider is master_port_width (in bits) / 32, in deca_vip.qsys, port width is 256, therefore divider is 8 */
        256/32,
        0,
        0,
    },
    /* VFR1 */
    {
        1,
        800, 480,
        0x1300000,
        256/32,
        0,
        ALT_VIP_VFR_1_BASE,
    },
    /* VFR2 */
    {
        2,
        640, 480,
        0x1000000,
        256/32,
        0,
        ALT_VIP_VFR_2_BASE,
    },
};

/**
 *  Check DDR3 status
 */
int CheckDDR3()
{
    int status = -1;
    if ( 0x0d == ( status = IORD(DDR3_STATUS_BASE, 0x0 )) )
    {
        status = 0;
    }
    else
    {
        printf( "DDR3 Status = 0x%x\n", status );
    }
    return status;
}

/**
 *  @input:
 *      vfr_register - VFR register base address
 *      width - VFR width in pixel
 *      height - VFR height in pixel
 *      frame_buffer - frame buffer address
 *      divider_for_words - divider used to calculate words (VFR master port width / bpp)
 *      cpr_halve - Color Plane Sequencer's Halve control packet width flag
 */
void ConfigVFR( unsigned int vfr_register, int width, int height, unsigned int frame_buffer, unsigned int divider_for_words, int cpr_halve )
{
    /* turn VFR off */
    IOWR( vfr_register, FRAMEREADER_REGISTER_CONTROL, 0 );
    
    /* set up front buffer */
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_BASE, frame_buffer );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_WORDS, (width * height) / divider_for_words );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_SCCP, width * height );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_WIDTH, cpr_halve ? (width * 2) : width);
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_HEIGHT, height );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME0_INTERLACED, 0 );
    
    /* set up back buffer */
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_BASE, frame_buffer + width * height * 4 );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_WORDS, (width * height) / divider_for_words );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_SCCP, width * height );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_WIDTH, cpr_halve ? (width * 2) : width );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_HEIGHT, height );
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME1_INTERLACED, 0 );
    
    /* turn VFR on */
    IOWR( vfr_register, FRAMEREADER_REGISTER_CONTROL, 1 );
    /* set the front buffer as the default */
    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME_SELECT, 0 );
}
/**
 */
void ConfigMixer()
{
    VFR_PARAMETERS* vfr_parameter = 0;
    /*  din0 - 1024x768 layer, not used */

    /*  din1 - 800x480 layer */
    /*  top left corner */
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER1_X, 0 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER1_Y, 0 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER1_ENABLE, 1 );

    /*  din2 - 640x480 layer */
    /*  middle of the screen */
    vfr_parameter = &VFR_Parameters[2];
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER2_X, (SCREEN_WIDTH-vfr_parameter->width)/2 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER2_Y, (SCREEN_HEIGHT-vfr_parameter->height)/2 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER2_ENABLE, 1 );

    /*  din3 - tpg */
    /*  bottom right corner */
    /*  Test Pattern Generator is used to verify if VIP system is working */
#if 0
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER3_X, 640 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER3_Y, 559 );
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_LAYER3_ENABLE, 1 );
#endif
    
    /* enable the mixer */
    IOWR( VIP_MIXER_BASE, MIXER_REGISTER_CONTROL, 1 );
}

/**
 *  Initialize VIP
 */
int InitVIP()
{
    VFR_PARAMETERS* vfr_parameter = 0;
    unsigned int* vip_register = 0;
    /*
     *  TPG
     */
    vip_register = TPG_BASE;

    /* Enable TPG as 320x240 screen */
    IOWR( TPG_BASE, TPG_REGISTER_CONTROL, 1 );
    IOWR( TPG_BASE, TPG_REGISTER_WIDTH, 320 );
    IOWR( TPG_BASE, TPG_REGISTER_HEIGHT, 240 );

    /* Setup VFR1 */
    vfr_parameter = &VFR_Parameters[1];
    ConfigVFR( ALT_VIP_VFR_1_BASE,
              vfr_parameter->width, vfr_parameter->height,
              MEM_IF_DDR3_EMIF_BASE + vfr_parameter->address,
              vfr_parameter->words_divider,
              vfr_parameter->cpr_halve );

    /* Setup VFR2 */
    vfr_parameter = &VFR_Parameters[2];
    ConfigVFR( ALT_VIP_VFR_2_BASE,
              vfr_parameter->width, vfr_parameter->height,
              MEM_IF_DDR3_EMIF_BASE + vfr_parameter->address,
              vfr_parameter->words_divider,
              vfr_parameter->cpr_halve );
    
    ConfigMixer();

    return 0;
}

/**
 *  Get VFR_PARAMETERS for the specified index
 *
 *  Assumption:
 *      index of only 1 or 2 is valid
 */
VFR_PARAMETERS* GetVFRParameters( unsigned int index )
{
    VFR_PARAMETERS *parameters = 0;
    if ( ( 1 <= index ) && ( 2 >= index ) )
    {
        parameters = &VFR_Parameters[index];
    }
    return parameters;
}

/**
 *  Get frame buffer address
 */
unsigned int GetFrameBuffer( unsigned int vfr_register, unsigned int buffer_index )
{
    return (unsigned int)IORD( vfr_register,  (0==buffer_index)? FRAMEREADER_REGISTER_FRAME0_BASE : FRAMEREADER_REGISTER_FRAME1_BASE );
}

/**
 *  Swap the frame buffers, such that the current front frame buffer shows the content,
 *  while the back frame buffer is used to construct next scene
 *
 *  @return - index to the current front frame buffer
 */
unsigned int SwapFrameBuffer( unsigned int vfr_register )
{
    unsigned int old = IORD( vfr_register, FRAMEREADER_REGISTER_FRAME_SELECT );
    unsigned int new = ( 0 == old ) ? 1 : 0;

    IOWR( vfr_register, FRAMEREADER_REGISTER_FRAME_SELECT, new );

    return new;
}

/**
 *  Get the current front frame buffer index
 */
unsigned int GetCurrentFrameBufferIndex( unsigned int vfr_register )
{
    return IORD( vfr_register, FRAMEREADER_REGISTER_FRAME_SELECT );
}

/**
 *  Draw a bar at the specified position (xpos/ypos) of width/heigh pixels in dimension.
 *
 *  Assumption:
 *      ypos + width should be no more than screen_height
 */
void DrawBar( unsigned int * fb,
             unsigned int screen_width, unsigned int screen_height,
             int xpos, int ypos,
             unsigned int width, unsigned int height,
             unsigned int foreground )
{
    unsigned int *ptr = 0;
    int i, j;
    /* adjust to the starting scanline */
    fb += screen_width * ypos;
    /* adjust to the starting pixel of the first scanline */
    fb += xpos;
    for( i=0; i<height; i++ )
    {
        ptr = fb;
        for( j=0; j<width; j++ )
        {
            *ptr++ = foreground;
        }
        
        /* switch to next scanline */
        fb += screen_width;
    }
}

/* blue */
#define BACKGROUND_COLOR  (0x000000ff)
/* green */
#define BAR1_COLOR (0x0000ff00)
/* red */
#define BAR2_COLOR   (0x00ff0000)

/* note 640 and 480 are multiples of 5 */
#define MOVING_DELTA (1)
#define BAR_SIZE (10)
/**
 *  Draw directly to the frame buffer
 */
int DrawVFR( unsigned int vfr )
{
    VFR_PARAMETERS* vfr_parameter = &VFR_Parameters[vfr];
    unsigned int frame_buffers[2] = { 0,  0 }, current = 0, *frame_buffer = 0;
    int verticalbar_x = (vfr_parameter->width-BAR_SIZE)/2, verticalbar_y = 0, verticalbar_dir = 1;
    int horizontalbar_x = 0, horizontalbar_y = (vfr_parameter->height-BAR_SIZE)/2, horizontalbar_dir = 1;
    
    frame_buffers[0] = GetFrameBuffer( vfr_parameter->vfr_register, 0 );
    frame_buffers[1] = GetFrameBuffer( vfr_parameter->vfr_register, 1 );
    
    current = GetCurrentFrameBufferIndex( vfr_parameter->vfr_register );
    if ( ( sizeof(frame_buffers)/sizeof(unsigned int ) ) <= current )
    {
        printf( "invalid current frame buffer index: %d\n", current );
        return -1;
    }
    
    frame_buffer = (unsigned int*)frame_buffers[current];
    /* fill the screen in blue */
    DrawBar( frame_buffer,
            vfr_parameter->width, vfr_parameter->height,
            0, 0,
            vfr_parameter->width, vfr_parameter->height,
            BACKGROUND_COLOR );
    
    /* draw a vertical bar at the left edge in green */
    DrawBar( frame_buffer,
            vfr_parameter->width, vfr_parameter->height,
            verticalbar_x, verticalbar_y,
            BAR_SIZE, vfr_parameter->height,
            BAR1_COLOR );

    /* draw a horizontal bar at the top edge in red */
    DrawBar( frame_buffer,
            vfr_parameter->width, vfr_parameter->height,
            horizontalbar_x, horizontalbar_y,
            vfr_parameter->width, BAR_SIZE,
            BAR2_COLOR );
#if 1
    for(;;)
    {
        /* 3 draws take about 166 ticks, or ms */
        OSTimeDlyHMSM(0, 0, 0, 500);
        
        frame_buffer = (unsigned int*)frame_buffers[(0==current)?1:0];
        
        /* erase the background by filling the screen in blue */
        DrawBar( frame_buffer,
                vfr_parameter->width, vfr_parameter->height,
                0, 0,
                vfr_parameter->width, vfr_parameter->height,
                BACKGROUND_COLOR );
        
        /* draw vertical bar moving left or right */
        if ( 0 < verticalbar_dir )
        {
            if ( vfr_parameter->width > ( verticalbar_x + BAR_SIZE + MOVING_DELTA ) )
            {
                verticalbar_x += MOVING_DELTA;
            }
            else
            {
                verticalbar_dir = -1;
            }
        }
        else
        {
            if ( 0 < ( verticalbar_x - MOVING_DELTA ) )
            {
                verticalbar_x -= MOVING_DELTA;
            }
            else
            {
                verticalbar_dir = 1;
            }
        }

        /* draw horizontal bar moving up or down */
        if ( 0 < horizontalbar_dir )
        {
            if ( vfr_parameter->height > ( horizontalbar_y + BAR_SIZE + MOVING_DELTA ) )
            {
                horizontalbar_y += MOVING_DELTA;
            }
            else
            {
                horizontalbar_dir = -1;
            }
        }
        else
        {
            if ( 0 < ( horizontalbar_y - MOVING_DELTA ) )
            {
                horizontalbar_y -= MOVING_DELTA;
            }
            else
            {
                horizontalbar_dir = 1;
            }
        }

        /* draw a vertical bar at the left edge in green */
        DrawBar( frame_buffer,
                vfr_parameter->width, vfr_parameter->height,
                verticalbar_x, verticalbar_y,
                BAR_SIZE, vfr_parameter->height,
                BAR1_COLOR );

        /* draw a horizontal bar at the top edge in red */
        DrawBar( frame_buffer,
                vfr_parameter->width, vfr_parameter->height,
                horizontalbar_x, horizontalbar_y,
                vfr_parameter->width, BAR_SIZE,
                BAR2_COLOR );
        

        current = SwapFrameBuffer( vfr_parameter->vfr_register );
    }
#endif
    return 0;
}

int Draw()
{
    return DrawVFR( 1 );
}

/**
 *  Check if the mixer enables the VFR1
 */
int IsVFR1Enabled()
{
    return ( 0 == IORD( VIP_MIXER_BASE, MIXER_REGISTER_LAYER1_ENABLE ) ) ? 0 : 1;
}

#define VFR_MOVING_DELTA (2)
/* 1280x800 */
void MoveVFR( unsigned int index )
{
    VFR_PARAMETERS* vfr_parameter = &VFR_Parameters[index];
    static unsigned int xpos = 0;
    static unsigned int ypos = 0;
    static int xdir = 1;
    static int ydir = 1;

    for(;;)
    {
        if ( 0 < xdir )
        {
            if ( SCREEN_WIDTH > ( xpos + vfr_parameter->width + VFR_MOVING_DELTA ) )
            {
                xpos += VFR_MOVING_DELTA;
            }
            else
            {
                xdir = -1;
            }
        }
        else
        {
            if ( 0 < ( xpos - VFR_MOVING_DELTA) )
            {
                xpos -= VFR_MOVING_DELTA;
            }
            else
            {
                xdir = 1;
            }
        }
        
        if ( 0 < ydir )
        {
            if ( SCREEN_HEIGHT > ( ypos + vfr_parameter->height + VFR_MOVING_DELTA ) )
            {
                ypos += VFR_MOVING_DELTA;
            }
            else
            {
                ydir = -1;
            }
        }
        else
        {
            if ( 0 < ( ypos - VFR_MOVING_DELTA) )
            {
                ypos -= VFR_MOVING_DELTA;
            }
            else
            {
                ydir = 1;
            }
        }
        
        IOWR( VIP_MIXER_BASE, (1 == index) ? MIXER_REGISTER_LAYER1_X : MIXER_REGISTER_LAYER2_X, xpos );
        IOWR( VIP_MIXER_BASE, (1 == index) ? MIXER_REGISTER_LAYER1_Y : MIXER_REGISTER_LAYER2_Y, ypos );
        
        /* yield to the system */
        OSTimeDlyHMSM(0, 0, 0, 66 );
    }
}

unsigned int hmi_updating_ticks = 0;
void MonitorHMI()
{
}
