// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id:$
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//      Zone Memory Allocation, perhaps NeXT ObjectiveC inspired.
//        Remark: this was the only stuff that, according
//         to John Carmack, might have been useful for
//         Quake.
//
//---------------------------------------------------------------------

#ifndef __Z_ZONE__
#define __Z_ZONE__




//
// ZONE MEMORY
// PU - purge tags.
// Tags < 100 are not overwritten until freed.
#define PU_STATIC       1        // static entire execution time
#define PU_SOUND        2        // static while playing
#define PU_MUSIC        3        // static while playing
#define PU_DAVE         4        // anything else Dave wants static
#define PU_LEVEL        50       // static until level exited
#define PU_LEVSPEC      51       // a special thinker in a level
// Tags >= 100 are purgable whenever needed.
#define PU_PURGELEVEL   100
#define PU_CACHE        101


void  Z_Init(void);
void* Z_Malloc2(int size, int tag, void* ptr, const char* debugfile, int debugline);
void  Z_Free(void* ptr);
void  Z_FreeTags(int lowtag, int hightag);
void  Z_DumpHeap(int lowtag, int hightag);
void  Z_FileDumpHeap(void* f);
void  Z_CheckHeap(void);
void  Z_ChangeTag2(void* ptr, int tag);
int   Z_FreeMemory(void);


typedef struct memblock_s
{
    int size;       // including the header and possibly tiny fragments
    void** user;    // 0 if a free block
    int tag;        // purgelevel
    int id;         // should be ZONEID
    struct memblock_s* next;
    struct memblock_s* prev;
} memblock_t;

//
// This is used to get the local FILE:LINE info from CPP
// prior to really call the function in question.
//
#define Z_ChangeTag(p,t) \
{ \
    extern unsigned char doom_wad_data_start[]; \
    extern unsigned char doom_wad_data_end[]; \
    if (!(p >= doom_wad_data_start && p < doom_wad_data_end)) \
    if (( (memblock_t *)( (byte *)(p) - sizeof(memblock_t)))->id!=0x1d4a11) \
    { \
        /*I_Error("Error: Z_CT at "__FILE__":%i",__LINE__);*/ \
        char buf[260]; \
        doom_strcpy(buf, "Error: Z_CT at " __FILE__ ":"); \
        doom_concat(buf, doom_itoa(__LINE__, 10)); \
        I_Error(buf); \
    } \
    Z_ChangeTag2(p,t); \
};
#define Z_Malloc(s,t,u) Z_Malloc2(s,t,u,__FILE__,__LINE__)

#endif

//-----------------------------------------------------------------------------
//
// $Log:$
//
//-----------------------------------------------------------------------------
