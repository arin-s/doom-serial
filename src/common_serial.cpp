#include "stdlib.h"
#include "doomdef.h"
#include "JPEGENC.h"
#include "common_serial.h"
#include "v_video.h"
#include "i_video.h"
#include <set>

// stubs
#include "doomgeneric.h"

uint8_t* getMCU(int x, int y, uint8_t* mcu);
void getJPEG(uint8_t *resultBuffer, int *resultSize)
{
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
        //doom_free(resultBuffer);
        //doom_free(pBitmap);
        //return 1;
    }
    rc = jpg.encodeBegin(&state, SCREENWIDTH, SCREENHEIGHT, ucPixelType, JPEGE_SUBSAMPLE_420, JPEGE_Q_HIGH);
    if (rc != JPEGE_SUCCESS)
    {
        //doom_free(resultBuffer);
        //doom_free(pBitmap);
        //return 1;
    }
    
    static uint8_t mcu[16 * 16 * 3];
    int mcu_count = ((SCREENWIDTH + 15) / 16) * ((SCREENHEIGHT + 15) / 16);
    for(int i = 0; i < mcu_count; i++)
    {
        jpg.addMCU(&state, getMCU(state.x, state.y, mcu), 16 * 3);
    }
    //rc = jpg.addFrame(&state, inputFrame, iPitch);
    if(rc != JPEGE_SUCCESS)
    {
        printf("something went wrong");
        exit(1);
    }
    *resultSize = jpg.close();
    return;
}

extern unsigned char screen_palette[256 * 3];
uint8_t* getMCU(int x, int y, uint8_t* mcu)
{
    struct color c;
    int palette_index, fb_coord, mcu_coord;
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            if(y + i < SCREENHEIGHT)
                fb_coord = (y + i) * SCREENWIDTH + x + j;
            else
                fb_coord = (SCREENHEIGHT - 1) * SCREENWIDTH + x + j;
            mcu_coord = (i * 16 + j) * 3;
            c = colors[*(I_VideoBuffer + fb_coord)];
            mcu[mcu_coord + 2] = (uint8_t)c.r;
            mcu[mcu_coord + 1] = (uint8_t)c.g;
            mcu[mcu_coord + 0] = (uint8_t)c.b;
        }
    }
    return mcu;
}

#define KEYQUEUE_SIZE 16
static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

void addKeyToQueue(int pressed, unsigned char key)
{
	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex)
	{
		//key queue is empty
		return 0;
	}
	else
	{
		unsigned short keyData = s_KeyQueue[s_KeyQueueReadIndex];
		s_KeyQueueReadIndex++;
		s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

		*pressed = keyData >> 8;
		*doomKey = keyData & 0xFF;

		return 1;
	}
}

void DG_SetWindowTitle(const char * title)
{
	
}

void DG_Init()
{
	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
}

std::set<uint8_t> pressed_keys;
// buf is NOT null-terminated
unsigned int processInput(unsigned char *buf, unsigned int len) {
    int state, ascii;
    for (int i = 0; i < len; i++)
    {
        state = (buf[i] >> 7) & 0b1;
        ascii = ~(1 << 7) & buf[i];
        // key is pressed and not in set
        if (state && pressed_keys.find(ascii) == pressed_keys.end())
            pressed_keys.insert(ascii);
        // key is not pressed and in set
        else if(!state && !(pressed_keys.find(ascii) == pressed_keys.end()))
            pressed_keys.erase(ascii);
        else
            continue;
        //printf("INPUT CHANGE: %c FROM %d to %d\n", ascii, ~state, state);
        addKeyToQueue(state, ascii);
    }
    return 0;
}