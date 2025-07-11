#include "doomkeys.h"

#include "doomgeneric.h"

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

#include "JPEGENC.h"
#include "i_video.h"

#define KEYQUEUE_SIZE 16

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

void getJPEG(uint8_t *resultBuffer, int *resultSize);
static const int JPEG_BUFFER_SIZE = 25000; //25kb

static void addKeyToQueue(int pressed, unsigned int keyCode)
{
    /*
	unsigned char key = convertToDoomKey(keyCode);

	unsigned short keyData = (pressed << 8) | key;

	s_KeyQueue[s_KeyQueueWriteIndex] = keyData;
	s_KeyQueueWriteIndex++;
	s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
    */
}



void DG_Init()
{
	memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));
  	//s_Image = XCreateImage(s_Display, DefaultVisual(s_Display, s_Screen), depth, ZPixmap, 0, (char *)DG_ScreenBuffer, DOOMGENERIC_RESX, DOOMGENERIC_RESX, 32, 0);
}


void DG_DrawFrame()
{
    static uint8_t resultBuffer[JPEG_BUFFER_SIZE];
    static int resultSize;
    getJPEG(resultBuffer, &resultSize);

}

void DG_SleepMs(uint32_t ms)
{
    osDelay(ms);
}

uint32_t DG_GetTicksMs()
{
    struct timeval  tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);

    return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
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

void doom_main()
{
    Serial1.begin(3000000);
    doomgeneric_Create(0, (char**){NULL});
    while(1)
    {
      doomgeneric_Tick(); 
    }

    return;
}




void getJPEG(uint8_t *resultBuffer, int *resultSize)
{
    uint8_t* inputFrame = (uint8_t*)DG_ScreenBuffer;
    // Static copy of JPEG encoder class
    JPEGENC jpg;
    // Struct that stores JPEGENC state
    JPEGENCODE state;
    // Stores return code
	int rc;
	// Specify input pixel format (24-bit RGB)
	uint8_t ucPixelType = JPEGE_PIXEL_ARGB8888;
    // Bits per pixel (24-bit)
	int iBytePP = 4;
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

/*#include "stdio.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "string.h"
#include "heap_api.h"

extern char _binary_______apps_doom_doom1_wad_start[];
extern char _binary_______apps_doom_doom1_wad_end[];

void writeSerial(unsigned char* buf, int size) {
    
}

static void* doom_malloc_buds(int size)
{
    TRACE(0, "ALLOCATING %d", size);
    void* ptr = med_malloc((size_t)size);
    size_t total = 0, used = 0, max_used = 0;
	med_memory_info(&total, &used, &max_used);
    TRACE(0, "TOTAL: %d USED: %d PEAK_USAGE: %d", total, used, max_used);
    return ptr;
}

static void doom_free_buds(void* ptr)
{
    med_free(ptr);
}

static void doom_print_buds(const char* str)
{
    TRACE(1, "DOOM TRACE: %s", str);
    //printf("%s", str);
}

void* doom_open_buds(const char* filename, const char* mode)
{
    if(strcmp(filename, "FLASHWAD") != 0)
        return;
    size_t size = _binary_______apps_doom_doom1_wad_end - _binary_______apps_doom_doom1_wad_start;
    return fmemopen(_binary_______apps_doom_doom1_wad_start, size, mode);
}
void doom_close_buds(void* handle)
{
    fclose(handle);
}
int doom_read_buds(void* handle, void *buf, int count)
{
    return (int)fread(buf, 1, count, handle);
}
int doom_write_buds(void* handle, const void *buf, int count)
{
    return (int)fwrite(buf, 1, count, handle);
}
int doom_seek_buds(void* handle, int offset, doom_seek_t origin)
{
    return fseek(handle, offset, origin);
}
int doom_tell_buds(void* handle)
{
    return (int)ftell(handle);
}
int doom_eof_buds(void* handle)
{
    return feof(handle);
}

uint32_t second = 0;
uint32_t usecond = 0;
uint32_t prev_tick = 0;
void doom_gettime_buds(int* sec, int* usec)
{
    uint32_t cur_tick = hal_sys_timer_get();
    usecond += TICKS_TO_US(hal_timer_get_passed_ticks(cur_tick, prev_tick));
    second += usecond / 1000000;
    usecond = usecond % 1000000;
    *sec = second;
    *usec = usecond;
    prev_tick = cur_tick;
}

void doom_set_buds_impl() {
    doom_set_print(doom_print_buds);
    doom_set_file_io(doom_open_buds, doom_close_buds, doom_read_buds, doom_write_buds, doom_seek_buds, doom_tell_buds, doom_eof_buds);
    doom_set_gettime(doom_gettime_buds);
    doom_set_malloc(doom_malloc_buds, doom_free_buds);
}
    */