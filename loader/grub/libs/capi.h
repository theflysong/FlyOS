/**
 * @file capi.h
 * @author theflysong (song_of_the_fly@163.com)
 * @brief CAPI
 * @version alpha-1.0.0
 * @date 2022-12-31
 * 
 * @copyright Copyright (c) 2022 TayhuangOS Development Team
 * SPDX-License-Identifier: LGPL-2.1-only
 * 
 */

#pragma once

#include <tay/types.h>
#include <stddef.h>

/**
 * @brief 堆大小
 */
#define HEAP_SIZE (512 * 1024)

/**
 * @brief 分配内存
 * 
 * @param size 请求的内存大小
 * @return void* 内存指针
 */
void *malloc(size_t size);

/**
 * @brief 释放内存
 * 
 * @param ptr 需要释放的内存指针
 */
void free(void *ptr);

/**
 * @brief 输出字符
 * 
 * @param ch 需要输出的字符
 */
void putchar(char ch);

/**
 * @brief 输出字符串
 * 
 * @param str 需要输出的字符串
 */
void puts(const char *str);