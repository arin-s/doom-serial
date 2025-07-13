#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <malloc.h>

#include "doomdef.h"

#include "common_serial.h"
#include "wad_symbols.h"

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

    // Initialize doom
    doomgeneric_Create(argc, argv);

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
        doomgeneric_Tick();
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

void DG_SleepMs(uint32_t ms)
{
    usleep (ms * 1000);
}

uint32_t DG_GetTicksMs()
{
    struct timeval  tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);

    return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}


// set sigint flag to cleanup resources before exiting
void sigintHandler(int sig) {
    sigint_flag = true;
}


/*
static int alloctotal = 0;
static void* doom_malloc_buds(int size, char* file, int line)
{
    printf("\nMALLOCATING:%d ", size);
    void* ptr = malloc((size_t)size);
    alloctotal += size;
    printf("USED:%d at %s %d", alloctotal, file, line);
    return ptr;
}
static void doom_free_buds(void* ptr)
{
    alloctotal -= malloc_usable_size(ptr);
    free(ptr);
}
*/
