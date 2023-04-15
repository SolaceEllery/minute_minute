/*
 *  minute - a port of the "mini" IOS replacement for the Wii U.
 *
 *  Copyright (C) 2016          SALT
 *  Copyright (C) 2016          Daz Jones <daz@dazzozo.com>
 *
 *  This code is licensed to you under the terms of the GNU GPL, version 2;
 *  see file COPYING or http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef _PRSH_H
#define _PRSH_H

#include "types.h"

void prsh_reset(void);
void prsh_init(void);
int prsh_get_entry(const char* name, void** data, size_t* size);
int prsh_set_entry(const char* name, void* data, size_t size);
void prsh_recompute_checksum();
void prsh_decrypt();
void prsh_encrypt();

#endif