// stdlib includes
#include <stdio.h>
#include <stdlib.h>

// doom includes
#include "doomdef.h"
#include "common_serial.h"
#include "stubs.h"
#include "doom_config.h"
#include <algorithm>
#include <math.h>

// jpegenc include
#include "JPEGENC.h"

// firmware includes
#include "hal_trace.h"
#include "hal_iomux.h"
#include "heap_api.h"
#include "hal_timer.h"
#include "cmsis_os.h"

#define HEAP_SIZE 1000*50
static char heap[HEAP_SIZE];
unsigned int rx_callback(unsigned char *buf, unsigned int len);

void doom_main()
{
    hal_trace_rx_register("doom", rx_callback);
    // heap init
    med_heap_init(heap, HEAP_SIZE);
    // Set function implementations
    doom_set_buds_impl();
    
    // Change default bindings to modern
    doom_set_default_int("key_up", DOOM_KEY_W);
    doom_set_default_int("key_down", DOOM_KEY_S);
    doom_set_default_int("key_strafeleft", DOOM_KEY_A);
    doom_set_default_int("key_straferight", DOOM_KEY_D);
    doom_set_default_int("key_left", DOOM_KEY_LEFT_ARROW);
    doom_set_default_int("key_right", DOOM_KEY_RIGHT_ARROW);
    doom_set_default_int("key_fire", DOOM_KEY_CTRL);

    // Initialize doom
    doom_init(0, (char**){NULL}, DOOM_FLAG_MENU_DARKEN_BG);

    //-----------------------------------------------------------------------
    // Main loop
    int start_sec, start_usec, end_sec, end_usec, delta_sec, delta_usec;
    while (true)
    {
        // read from serial port
        //while()

        // Update game loop
        doom_gettime(&start_sec, &start_usec);
        doom_update();
        doom_gettime(&end_sec, &end_usec);
        delta_sec = end_sec - start_sec;
        delta_usec = end_usec - start_usec;
        if(delta_usec < 0)
        {
            delta_usec += 1000000;
            delta_sec--;
        }
        //TRACE(1, "DOOM_UPDATE() %d.%06d", delta_sec, delta_usec);
        //osDelay(10);

        // The JPEG buffer
        static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
        static int resultSize;
        doom_gettime(&start_sec, &start_usec);
        getJPEG(resultBuffer, &resultSize);
        doom_gettime(&end_sec, &end_usec);
        delta_sec = end_sec - start_sec;
        delta_usec = end_usec - start_usec;
        if(delta_usec < 0)
        {
            delta_usec += 1000000;
            delta_sec--;
        }
        //TRACE(1, "getJPEG() %d.%06d", delta_sec, delta_usec);
        //TRACE(1, "getJPEG() SIZE: %d", resultSize);
        hal_trace_output(resultBuffer, resultSize);
    }
    return;
}

char keyState[255];
// buf is NOT null-terminated
unsigned int rx_callback(unsigned char *buf, unsigned int len) {
    doom_key_t ascii;
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
        ascii = static_cast<doom_key_t>(~(1 << 7) & buf[i]);
        if(newState != oldState)
        {
            //TRACE(1, "INPUT CHANGE: %c FROM %d to %d\n", (char)ascii, oldState, newState);
            switch(newState)
            {
                case 0:
                    doom_key_up(ascii);
                    break;
                case 1:
                    doom_key_down(ascii);
                    break;
            }
            keyState[i] = buf[i];
        }
    }
    return 0;
}