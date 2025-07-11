#include "stdlib.h"
#include "doomdef.h"
#include "JPEGENC.h"
#include "common_serial.h"
#include "v_video.h"
#include "i_video.h"

extern unsigned char screen_palette[256 * 3];

uint8_t* getMCU(int x, int y, uint8_t* mcu);

void getJPEG(uint8_t *resultBuffer, int *resultSize)
{
    // Static copy of JPEG encoder class
    JPEGENC jpg;
    // Struct that stores JPEGENC state
    JPEGENCODE state;
    // Stores return code
	int rc;
	// Specify input pixel format (24-bit RGB)
	uint8_t ucPixelType = JPEGE_PIXEL_RGB888;
    // Bits per pixel (24-bit)
	int iBytePP = 3;
    // Image stride (number of bytes in a row of pixels)
    int iPitch = iBytePP * SCREENWIDTH;
	rc = jpg.open(resultBuffer, JPEG_BUFFER_SIZE);
	if (rc != JPEGE_SUCCESS)
	{
        //free(resultBuffer);
        //free(pBitmap);
        //return 1;
    }
    rc = jpg.encodeBegin(&state, SCREENWIDTH, SCREENHEIGHT, ucPixelType, JPEGE_SUBSAMPLE_420, JPEGE_Q_HIGH);
    if (rc != JPEGE_SUCCESS)
    {
        //free(resultBuffer);
        //free(pBitmap);
        //return 1;
    }
    
    static uint8_t mcu[16 * 16 * 3];
    int mcu_count = ((SCREENWIDTH + 15) / 16) * ((SCREENHEIGHT + 15) / 16);
    for(int i = 0; i < mcu_count; i++)
    {
        jpg.addMCU(&state, getMCU(state.x, state.y, mcu), 16 * 3);
    }
    //rc = jpg.addFrame(&state, inputFrame, iPitch);
    if(rc != JPEGE_SUCCESS)
    {
        printf("something went wrong");
        exit(1);
    }
    *resultSize = jpg.close();
    return;
}

uint8_t* getMCU(int x, int y, uint8_t* mcu)
{
    /*
    int palette_index, fb_coord, mcu_coord;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            if(y + i < SCREENHEIGHT)
                fb_coord = (y + i) * SCREENWIDTH + x + j;
            else
                fb_coord = (SCREENHEIGHT - 1) * SCREENWIDTH + x + j;
            mcu_coord = (i * 16 + j) * 3;
            palette_index = screens[0][fb_coord] * 3;
            mcu[mcu_coord + 2] = screen_palette[palette_index + 0];
            mcu[mcu_coord + 1] = screen_palette[palette_index + 1];
            mcu[mcu_coord + 0] = screen_palette[palette_index + 2];
        }
    }
    */
    return mcu;
}