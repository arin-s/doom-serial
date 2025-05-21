#include <stdio.h>
#include <stdlib.h>

#if defined(DOOM_EXAMPLE_USE_SINGLE_HEADER) // Use the PureDOOM.h single header
#define DOOM_IMPLEMENTATION
#include "../../PureDOOM.h"
#else
#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#endif

#include "linux_uart.h"
#include "../../thirdparty/JPEGENC/src/JPEGENC.h"

void getJPEG(uint8_t *resultBuffer, int *resultSize);

const int JPEG_BUFFER_SIZE = 25000; //25kb

int main(int argc, char** argv)
{
    // Check arguments
    if(argc != 2) {
        printf("Incorrect argument count");
        return 1;
    }
    // Open port
    int error = openSerial(argv[1]);
    if(error) {
        printf("Failed to open port");
        return 1;
    }

    // Change default bindings to modern
    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);

    // Initialize doom
    doom_init(argc, argv, DOOM_FLAG_MENU_DARKEN_BG);

    //-----------------------------------------------------------------------
    
    // Main loop
    int done = 0;
    while (!done)
    {
        //Handle input
        //while (1)
        if (done) break;
        // Update game loop
        doom_update();
        // The JPEG buffer
        static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
        static int resultSize;
        getJPEG(resultBuffer, &resultSize);
        // Encode with JPEGENC
        writeSerial(resultBuffer, resultSize);
    }
    
    // Shutdown
    return 0;
}

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