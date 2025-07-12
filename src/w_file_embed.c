//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	WAD I/O functions.
//

#include <stdio.h>
#include "string.h"
#include "common_symbols.h"
#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

typedef struct
{
    wad_file_t wad;
} embed_wad_file_t;

extern wad_file_class_t embed_wad_file;

static wad_file_t *W_Embed_OpenFile(char *path)
{
    printf("EMBED OPENING");
    embed_wad_file_t *result;
    // Create a new embed_wad_file_t to hold the file handle.
    result = Z_Malloc(sizeof(embed_wad_file_t), PU_STATIC, 0);
    result->wad.file_class = &embed_wad_file;
    result->wad.mapped = doom_wad_data_start;
    result->wad.length = doom_wad_data_end - doom_wad_data_start;
    return &result->wad;
}

static void W_Embed_CloseFile(wad_file_t *wad)
{
    embed_wad_file_t *embed_wad;
    embed_wad = (embed_wad_file_t *) wad;
    Z_Free(embed_wad);
}

// Read data from the specified position in the file into the 
// provided buffer.  Returns the number of bytes read.

size_t W_Embed_Read(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    // Read into the buffer.
    memcpy(buffer, doom_wad_data_start + offset, buffer_len);
    return buffer_len;
}


wad_file_class_t embed_wad_file = 
{
    W_Embed_OpenFile,
    W_Embed_CloseFile,
    W_Embed_Read,
};


