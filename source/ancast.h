/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef _ANCAST_H
#define _ANCAST_H

#include "types.h"

u32 ancast_iop_load(const char* path);
u32 ancast_ppc_load(const char* path);

u32 ancast_iop_load_from_raw_sector(int sector_idx);
u32 ancast_iop_load_from_memory(void* ancast_mem);
u32 ancast_patch_load(const char* fn_ios, const char* fn_patch);

// Used for patches on IOS boot, and the passalong magic otherwise.
#define ALL_PURPOSE_TMP_BUF (0x00800000)

#define PASSALONG_MAGIC_BOOT1 ("MINTBT01")

#define ANCAST_MAGIC (0xEFA282D9l)
#define ANCAST_TARGET_IOP (0x02)
#define ANCAST_TARGET_PPC (0x01)

#endif