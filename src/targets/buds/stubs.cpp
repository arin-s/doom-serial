// stdlib
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>

// JPEGENC
#include "JPEGENC.h"

// doom
#include "i_video.h"
#include "doomkeys.h"
#include "common_serial.h"

// os
#include "cmsis_os.h"
#include "hal_timer.h"
#include "hal_trace.h"
#include "heap_api.h"

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
extern "C" {
void* doom_malloc_log(int size, const char* file, const int line)
{
    TRACE(0, "ALLOCATING %d from %s:%d\n", size, file, line);
    void* ptr = med_malloc((size_t)size);
    size_t total = 0, used = 0, max_used = 0;
	med_memory_info(&total, &used, &max_used);
    TRACE(0, "TOTAL: %d USED: %d PEAK_USAGE: %d\n", total, used, max_used);
    return ptr;
}

void doom_free_log(void* ptr, const char* file, const int line)
{
    if (ptr == NULL) {
        printf("WARNING: Trying to free NULL pointer\n");
        return;
    }
    med_free(ptr);
}
// Logging stuff
uint8_t log_buf[LOG_CAPACITY];
int log_offset;
void doom_log(char* buf, ...) {
    va_list ap;
    va_start(ap, buf);
    int len = vsnprintf(NULL, 0, buf, ap);
    if (len + log_offset > LOG_CAPACITY)
    {
        va_end(ap);
        return;
    }
    vsprintf((char*)log_buf + log_offset, buf, ap);
    va_end(ap);
    log_offset += len;
}
}
/*
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