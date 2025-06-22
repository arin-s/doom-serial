#include "stdlib.h"
#include "DOOM.h"
#include "doomdef.h"
#include "JPEGENC.h"
#include "common_serial.h"

void getJPEG(uint8_t *resultBuffer, int *resultSize)
{
    // Get framebuffer
    uint8_t* inputFrame = const_cast<uint8_t*>(doom_get_framebuffer(3));
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
    rc = jpg.addFrame(&state, inputFrame, iPitch);
    *resultSize = jpg.close();
    return;
}