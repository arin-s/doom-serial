// stdlib includes
#include <stdio.h>
#include <stdlib.h>
//#include <chrono>

#include "DOOM.h"
#include "doomdef.h"

//#include <atomic>
#include "buds.h"

// jpegenc include
#include "JPEGENC.h"
#include "serial.h"

#include "common_serial.h"

void doom_main()
{
    // Set function implementations
    doom_set_buds_impl();

    // Change default bindings to modern
    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_use", DOOM_KEY_E);

    // Initialize doom
    doom_init(0, (char**){NULL}, DOOM_FLAG_MENU_DARKEN_BG);

    //-----------------------------------------------------------------------
    // Main loop
    while (true)
    {
        // read from serial port
        //while()

        // Update game loop
        //auto startTime = std::chrono::steady_clock::now();
        doom_update();
        //auto endTime = std::chrono::steady_clock::now();
        //auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        //printf("DOOM %f ms\n", duration.count());

        // The JPEG buffer
        static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
        static int resultSize;
        //startTime = std::chrono::steady_clock::now();
        getJPEG(resultBuffer, &resultSize);
        //endTime = std::chrono::steady_clock::now();
        //duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        //printf("JPEG %f ms\n", duration.count());
        //printf("START: %02X END: %02X\n", resultBuffer[0], resultBuffer[resultSize-1]);
        //printf("%d\n", resultSize);
        // Encode with JPEGENC
        writeSerial(resultBuffer, resultSize);
    }
    return;
}