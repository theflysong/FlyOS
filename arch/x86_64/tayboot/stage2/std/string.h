// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2021, 2021 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/std/string.h
 *
 * Standard lib string.h header here
 */



#pragma once

int strcmp(const char* str1, const char* str2);
int strlen(const char* str);
char* strcpy(char* dst, const char* src);
int atoi(const char* str);
int atoi_8(const char* str);
int atoi_16(const char* str);
unsigned int atoui(const char* str);
unsigned int atoui_8(const char* str);
unsigned int atoui_16(const char* str);
char* itoa(char* buffer, int value, int radix);
char* uitoa(char* buffer, unsigned int value, int radix);
char* itoa_upper(char* buffer, int value, int radix);
char* uitoa_upper(char* buffer, unsigned int value, int radix);
char* ftoa(char* buffer, float value); //don't work...