// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>

// doom
#include "doomdef.h"
#include "stubs.h"
#include "buds.h"
#include "common_serial.h"
#include "doomgeneric.h"

// jpegenc
#include "JPEGENC.h"

// os
#include "hal_trace.h"
#include "hal_iomux.h"
#include "heap_api.h"
#include "hal_timer.h"
#include "cmsis_os.h"

#define HEAP_SIZE 1000*50
static char heap[HEAP_SIZE];
unsigned int rx_callback(unsigned char *buf, unsigned int len);
int start_ms, end_ms, delta_ms;
// called from noapp_main
void doom_main()
{
    // For rx input commands
    hal_trace_rx_register("doom", rx_callback);
    // Heap init
    med_heap_init(heap, HEAP_SIZE);
    // Initialize doom
    doomgeneric_Create(0, (char**){NULL});
    // Main loop
    while (true)
    {
        // Update game loop
        start_ms = DG_GetTicksMs();
        doomgeneric_Tick(); 
        end_ms = DG_GetTicksMs();
        //TRACE(1, "DOOM_UPDATE() %d", end_ms - start_ms);
        //osDelay(10);
    }
    return;
}

static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
static int resultSize;
void DG_DrawFrame()
{
    start_ms = DG_GetTicksMs();
    getJPEG(resultBuffer, &resultSize);
    end_ms = DG_GetTicksMs();
    //TRACE(1, "getJPEG() %d.%06d", delta_sec, delta_usec);
    //TRACE(1, "getJPEG() SIZE: %d", resultSize);

    hal_trace_output(resultBuffer, resultSize);
}

char keyState[255];
// buf is NOT null-terminated
unsigned int rx_callback(unsigned char *buf, unsigned int len) {
    char ascii;
    int newState, oldState;
    /*TRACE(1, "RECEIVED INPUT: ");
    for (int i = 0; i < len; i++)
    {
        int state = (buf[i] >> 7) & 0b1;
        char c = ~(1 << 7) & buf[i];
        TRACE(1, "%c%d", c, state);
    }
    TRACE(1,"\n");*/
    for (int i = 0; i < len; i++)
    {
        oldState = (keyState[i] >> 7) & 0b1;
        newState = (buf[i] >> 7) & 0b1;
        ascii = ~(1 << 7) & buf[i];
        if(newState != oldState)
        {
            //TRACE(1, "INPUT CHANGE: %c FROM %d to %d\n", ascii, oldState, newState);
            addKeyToQueue(newState, ascii);
            keyState[i] = buf[i];
        }
    }
    return 0;
}