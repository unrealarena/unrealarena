/*
===========================================================================
Copyright (C) 1999-2005 Id Software, Inc.
Copyright (C) 2007 HermitWorks Entertainment Corporation
Copyright (C) 2006-2009 Robert Beckebans <trebor_7@users.sourceforge.net>

This file is part of Daemon source code.

Daemon source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

Daemon source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Daemon source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#include "tr_local.h"
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

// HACK: Try to crash less when asked to decode invalid inputs.
class crnd_decompression_exception : public std::exception {};
#define CRND_ASSERT(_exp) (!!(_exp) ? (void)0 : throw crnd_decompression_exception())

#include "crunch/inc/crn_decomp.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

bool LoadInMemoryCRN(void* buff, size_t buffLen, byte **data, int *width, int *height,
                     int *numLayers, int *numMips, int *bits)
{
    if (crnd::crnd_validate_file(buff, buffLen, nullptr)) { // Checks the header, not the whole file.
        // Found height and width in [1, 4096], num mip levels in [1, 13], faces in {1, 6}
    } else {
        return false;
    }
    crnd::crn_texture_info ti;
    if (!crnd::crnd_get_texture_info(buff, buffLen, &ti)) {
        return false;
    }

    switch (ti.m_format) {
    case cCRNFmtDXT1:
        *bits |= IF_BC1;
        break;
    case cCRNFmtDXT3:
        *bits |= IF_BC2;
        break;
    case cCRNFmtDXT5:
        *bits |= IF_BC3;
        break;
    case cCRNFmtDXT5A:
        *bits |= IF_BC4;
        break;
    case cCRNFmtDXN_XY:
        *bits |= IF_BC5;
        break;
    default:
        return false;
    }

    *width = ti.m_width;
    *height = ti.m_height;
    *numMips = ti.m_levels;
    *numLayers = ti.m_faces == 6 ? 6 : 0;

    uint32_t totalSize = 0;
    uint32_t sizes[cCRNMaxLevels];
    for (unsigned i = 0; i < ti.m_levels; i++) {
        crnd::crn_level_info li;
        if (!crnd::crnd_get_level_info(buff, buffLen, i, &li)) {
            return false;
        }
        sizes[i] = li.m_blocks_x * li.m_blocks_y * li.m_bytes_per_block;
        totalSize += sizes[i] * ti.m_faces;
    }

    crnd::crnd_unpack_context ctx = crnd::crnd_unpack_begin(buff, buffLen);
    if (!ctx) {
        return false;
    }
    byte* nextImage = (byte *)ri.Z_Malloc(totalSize);
    bool success = true;
    for (unsigned i = 0; i < ti.m_levels; i++) {
        for (unsigned j = 0; j < ti.m_faces; j++) {
            data[i * ti.m_faces + j] = nextImage;
            nextImage += sizes[i];
        }
        try {
            if (!crnd::crnd_unpack_level(ctx, (void **)&data[i * ti.m_faces], sizes[i], 0, i)) {
                success = false;
                break;
            }
        } catch (const crnd_decompression_exception&) {
            // Exception added as a hack to try and avoid crashing on files using the old format.
            // In general though, it seems the crunch library does not try to validate the files and may crash while decoding.
            success = false;
            break;
        }
    }
    crnd::crnd_unpack_end(ctx);
    return success;
}

void LoadCRN(const char* name, byte **data, int *width, int *height,
             int *numLayers, int *numMips, int *bits, byte)
{
    void* buff;
    size_t buffLen = ri.FS_ReadFile(name, &buff);
    *numLayers = 0;
    if (!buff) {
        return;
    }
    if (!LoadInMemoryCRN(buff, buffLen, data, width, height, numLayers, numMips, bits)) {
        if (*data) {
            ri.Free(*data);
            *data = nullptr; // This signals failure.
        }
        Log::Warn("Invalid CRN image: %s", name);
    }
    ri.FS_FreeFile(buff);
}
