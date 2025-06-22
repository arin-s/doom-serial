#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <atomic>
#include <chrono>

#if defined(DOOM_EXAMPLE_USE_SINGLE_HEADER) // Use the PureDOOM.h single header
#define DOOM_IMPLEMENTATION
#include "../../PureDOOM.h"
#else
#include "DOOM/DOOM.h"
#include "DOOM/doomdef.h"
#endif

#include "common/common_serial.h"

#include "linux_uart.h"
#include "../../thirdparty/JPEGENC/src/JPEGENC.h"

void sigintHandler(int sig);

std::atomic<bool> sigint_flag = false; // for SIGINT handler

int main(int argc, char** argv)
{   
    // Check arguments
    if(argc != 2) {
        printf("Incorrect argument count");
        return 1;
    }
    // Open port
    int serial_port = openSerial(argv[1]);
    if(serial_port < 0) {
        printf("Failed to open port");
        return 1;
    }

    // Register SIGINT handler
    signal(SIGINT, sigintHandler);

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
    while (!sigint_flag)
    {
        // read from serial port
        //while()
        

        //while (1) {}
        if (done) break;

        // Update game loop
        auto startTime = std::chrono::steady_clock::now();
        doom_update();
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        printf("DOOM %f ms\n", duration.count());

        // The JPEG buffer
        static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
        static int resultSize;
        startTime = std::chrono::steady_clock::now();
        getJPEG(resultBuffer, &resultSize);
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        printf("JPEG %f ms\n", duration.count());
        //printf("START: %02X END: %02X\n", resultBuffer[0], resultBuffer[resultSize-1]);
        //printf("%d\n", resultSize);
        // Encode with JPEGENC
        writeSerial(resultBuffer, resultSize);
    }
    closeSerial();
    return 0;
}

// set sigint flag to cleanup resources before exiting
void sigintHandler(int sig) {
    sigint_flag = true;
}