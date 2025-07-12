// stdlib
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>

// JPEGENC
#include "JPEGENC.h"

// doom
#include "i_video.h"
#include "doomkeys.h"
#include "common_serial.h"

// os
#include "cmsis_os.h"
#include "hal_timer.h"

// stubs
#include "doomgeneric.h"

void DG_SleepMs(uint32_t ms)
{
    osDelay(ms);
}

uint32_t second = 0;
uint32_t usecond = 0;
uint32_t prev_tick = 0;
uint32_t DG_GetTicksMs()
{
    uint32_t cur_tick = hal_sys_timer_get();
    usecond += TICKS_TO_US(hal_timer_get_passed_ticks(cur_tick, prev_tick));
    second += usecond / 1000000;
    usecond = usecond % 1000000;
    return (second * 1000) + (usecond / 1000); /* return milliseconds */
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

void doom_set_buds_impl() {
    doom_set_print(doom_print_buds);
    doom_set_file_io(doom_open_buds, doom_close_buds, doom_read_buds, doom_write_buds, doom_seek_buds, doom_tell_buds, doom_eof_buds);
    doom_set_gettime(doom_gettime_buds);
    doom_set_malloc(doom_malloc_buds, doom_free_buds);
}
    */