// stdlib
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <set>

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
static int resultSize;
// called from noapp_main
void doom_main()
{
    int start_ms, end_ms, delta_ms;
    // For rx input commands
    hal_trace_rx_register("doom", processInput);
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
        getJPEG(resultBuffer, &resultSize);
        cobsEncode(resultBuffer, resultSize);
        hal_trace_output(resultBuffer, resultSize);
    }
    return;
}