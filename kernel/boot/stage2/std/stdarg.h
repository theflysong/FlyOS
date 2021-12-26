// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2021, 2021 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/std/stdarg.h
 *
 * Standard lib stdarg.h header here
 */



#pragma once

typedef char *va_list;
typedef va_list *pva_list;

#define INTSIZEOF(ty) ((sizeof(ty) + sizeof(int) - 1) & (~(sizeof(int) - 1)))

#define va_start(lst, start) (lst = (((va_list)&start) + INTSIZEOF(start)))
#define va_arg(lst, ty) (*(ty*)((lst += INTSIZEOF(ty)) - INTSIZEOF(ty)))
#define va_end(lst) (lst = ((va_list)0))