#include <stdio.h>
#include <stdlib.h>

#if defined(DOOM_EXAMPLE_USE_SINGLE_HEADER) // Use the PureDOOM.h single header
#define DOOM_IMPLEMENTATION
#include "../../PureDOOM.h"
#else
#include "DOOM/DOOM.h"
#endif

// Resolution DOOM renders at
#define WIDTH 320
#define HEIGHT 200

int main(int argc, char** argv)
{
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
        if (done) break;
        doom_update();
        // Get framebuffer
        const unsigned char* src = doom_get_framebuffer(3);
    }
    
    // Shutdown
    return 0;
}
