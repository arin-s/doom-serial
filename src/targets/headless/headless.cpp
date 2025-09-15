#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <atomic>
#include <chrono>
#include <malloc.h>

#include "doomdef.h"

#include "common_serial.h"
#include "common_symbols.h"
#include "doomgeneric.h"

#include "linux_uart.h"
#include "JPEGENC.h"
#include <sys/time.h>

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
    int done = 0, size;
    static int resultSize;
    uint8_t* read_buffer;
    while (!sigint_flag)
    {
        // read from port
        readSerial(read_buffer, size);
        // process key input
        processInput(read_buffer, size);
        delete[] read_buffer;
        
        // Update game loop
        auto startTime = std::chrono::steady_clock::now();
        doomgeneric_Tick();
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        printf("DOOM %f ms\n", duration.count());

        startTime = std::chrono::steady_clock::now();
        getJPEG(resultBuffer, &resultSize);
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        printf("JPEG %f ms\n", duration.count());
        cobsEncode(resultBuffer, resultSize);
        startTime = std::chrono::steady_clock::now();
        writeSerial(resultBuffer, resultSize);
        endTime = std::chrono::steady_clock::now();
        duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        printf("WRITESTREAM %f ms\n", duration.count());
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

#ifdef __cplusplus
extern "C" {
#endif
static int alloctotal = 0;
void* doom_malloc_log(int size, const char* file, const int line)
{
    printf("\nMALLOCATING:%d ", size);
    void* ptr = malloc((size_t)size);
    if (ptr == NULL) {
        printf("MALLOC FAILED! Size:%d at %s:%d\n", size, file, line);
        exit(1);
    }
    alloctotal += size;
    printf("USED:%d at %s %d", alloctotal, file, line);
    return ptr;
}
void doom_free_log(void* ptr, const char* file, const int line)
{
    if (ptr == NULL) {
        printf("WARNING: Trying to free NULL pointer\n");
        return;
    }
    alloctotal -= malloc_usable_size(ptr);
    free(ptr);
}
#ifdef __cplusplus
}
#endif