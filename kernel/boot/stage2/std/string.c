// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2021, 2021 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage2/std/string.c
 *
 * Standard lib string.h source here
 */



#include <string.h>
#include <ctype.h>

int strcmp(const char *str1, const char *str2) {
    int res = 0;
    while (!(res = (unsigned char) *(str1++) - (unsigned char) *(str2++)) && *str1);

    if (res < 0)
        return -1;
    else if (res > 0)
        return 1;
    return 0;
}

int strlen(const char *str) {
    int res = 0;
    while (*(str ++)) res ++;
    return res;
}

int atoi(const char* str) {
    int result = 0;
    unsigned char sign = 1;

    if (str == 0) {
        return 0;
    }

    while (isspace(*(str ++))) {}
    str --;

    if ((*str) == '-') sign = -1;
    if ((*str) == '-' || (*str) == '+') str ++;

    while (isdigit(*(str ++))) {
        result = result * 10 + (*str) - '0';
    }

    return sign * result;
}

static const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

char* itoa(char* buffer, int value, int radix) {
    char* store = buffer;
    if (radix < 2 || radix >= 36) {
        *buffer = 0;
        return store;
    }
    if (value == 0) {
        *(buffer ++) = '0';
        *buffer = 0;
        return store;
    }
    else if (value < 0) {
        *(buffer ++) = '-';
        value = -value;
    }

    char buf[100] = {};

    int cnt = 0;

    while (value > 0) {
        buf[cnt ++] = digits[value % radix];
        value /= radix;
    }

    for (cnt -= 1 ; cnt >= 0 ; cnt --) {
        *(buffer ++) = buf[cnt];
    }
    *buffer = 0;

    return store;
}

char* uitoa(char* buffer, unsigned int value, int radix) {
    char* store = buffer;
    if (radix < 2 || radix >= 36) {
        *buffer = 0;
        return store;
    }
    if (value == 0) {
        *(buffer ++) = '0';
        *buffer = 0;
        return store;
    }

    char buf[100] = {};

    int cnt = 0;

    while (value > 0) {
        buf[cnt ++] = digits[value % radix];
        value /= radix;
    }

    for (cnt -= 1 ; cnt >= 0 ; cnt --) {
        *(buffer ++) = buf[cnt];
    }
    *buffer = 0;
    return store;
}

static const char upper_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

char* itoa_upper(char* buffer, int value, int radix) {
    char* store = buffer;
    if (radix < 2 || radix >= 36) {
        *buffer = 0;
        return store;
    }
    if (value == 0) {
        *(buffer ++) = '0';
        *buffer = 0;
        return store;
    }
    else if (value < 0) {
        *(buffer ++) = '-';
        value = -value;
    }

    char buf[100] = {};

    int cnt = 0;

    while (value > 0) {
        buf[cnt ++] = upper_digits[value % radix];
        value /= radix;
    }

    for (cnt -= 1 ; cnt >= 0 ; cnt --) {
        *(buffer ++) = buf[cnt];
    }
    *buffer = 0;

    return store;
}

char* uitoa_upper(char* buffer, unsigned int value, int radix) {
    char* store = buffer;
    if (radix < 2 || radix >= 36) {
        *buffer = 0;
        return store;
    }
    if (value == 0) {
        *(buffer ++) = '0';
        *buffer = 0;
        return store;
    }
    else if (value < 0) {
        *(buffer ++) = '-';
        value = -value;
    }

    char buf[100] = {};

    int cnt = 0;

    while (value > 0) {
        buf[cnt ++] = upper_digits[value % radix];
        value /= radix;
    }

    for (cnt -= 1 ; cnt >= 0 ; cnt --) {
        *(buffer ++) = buf[cnt];
    }
    *buffer = 0;

    return store;
}

char* ftoa(char* buffer, float value) {
    char* store = buffer;
    if (value == 0) {
        *(buffer ++) = '0';
        *buffer = 0;
        return store;
    }
    else if (value < 0) {
        *(buffer ++) = '-';
        value = -value;
    }

    char buf[100] = {};

    int cnt = 0;

    if (((int)value) == 0) {
        *(buffer ++) = '0';
    }

    while (value > 1) {
        buf[cnt ++] = ((int)value) % 10 + '0';
        value /= 10;
    }
    for (cnt -= 1 ; cnt >= 0 ; cnt --) {
        *(buffer ++) = buf[cnt];
    }

    *(buffer ++) = '.';

    while (value > 0) {
        value *= 10;
        *(buffer ++) = (int)value + '0';
        value -= (int)value;
    }

    *buffer = 0;
    return store;
}