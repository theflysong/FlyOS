/**
 * @file io.h
 * @author theflysong (song_of_the_fly@163.com)
 * @brief IO函数
 * @version alpha-1.0.0
 * @date 2023-06-23
 * 
 * @copyright Copyright (c) 2022 TayhuangOS Development Team
 * SPDX-License-Identifier: LGPL-2.1-only
 * 
 */

#pragma once

#include <tay/types.h>

/**
 * @brief 读字节
 * 
 * @param port 端口
 * @return 读到的字节 
 */
inline static byte inb(word port) {
    byte data;
    asm volatile ("inb %1, %0" : "=a"(data) : "dN"(port));
    return data;
}

/**
 * @brief 写字节
 * 
 * @param port 端口
 * @param data 数据
 */
inline static void outb(word port, byte data) {
    asm volatile ("outb %0, %1" : : "a"(data), "dN"(port));
}

/**
 * @brief 读字
 * 
 * @param port 端口
 * @return 读到的字 
 */
inline static word inw(word port) {
    word data;
    asm volatile ("inw %1, %0" : "=a"(data) : "dN"(port));
    return data;
}

/**
 * @brief 写字节
 * 
 * @param port 端口
 * @param data 数据
 */
inline static void outw(word port, word data) {
    asm volatile ("outw %0, %1" : : "a"(data), "dN"(port));
}

/**
 * @brief 读双字
 * 
 * @param port 端口
 * @return 读到的双字 
 */
inline static dword ind(word port) {
    dword data;
    asm volatile ("inl %1, %0" : "=a"(data) : "dN"(port));
    return data;
}

/**
 * @brief 写双字
 * 
 * @param port 端口 
 * @param data 数据
 */
inline static void outd(word port, dword data) {
    asm volatile ("outl %0, %1" : : "a"(data), "dN"(port));
}

/**
 * @brief 读取cs
 * 
 * @return cs 
 */
inline static sreg_t rdcs(void) {
    sreg_t reg;
    asm volatile ("movw %%cs, %0" : "=rm"(reg));
    return reg;
}


/**
 * @brief 读取ds
 * 
 * @return ds 
 */
inline static b16 rdds(void) {
    sreg_t reg;
    asm volatile ("movw %%ds, %0" : "=rm"(reg));
    return reg;
}

/**
 * @brief 写ds
 * 
 * @param reg ds
 */
inline static void stds(sreg_t reg) {
    asm volatile ("movw %0, %%ds" : : "rm"(reg));
}

/**
 * @brief 读取es
 * 
 * @return es 
 */
inline static sreg_t rdes(void) {
    sreg_t reg;
    asm volatile ("movw %%es, %0" : "=rm"(reg));
    return reg;
}

/**
 * @brief 写es
 * 
 * @param reg es
 */
inline static void stes(sreg_t reg) {
    asm volatile ("movw %0, %%es" : : "rm"(reg));
}

/**
 * @brief 读取fs
 * 
 * @return es 
 */
inline static sreg_t rdfs(void) {
    sreg_t reg;
    asm volatile ("movw %%fs, %0" : "=rm"(reg));
    return reg;
}

/**
 * @brief 写fs
 * 
 * @param reg fs
 */
inline static void stfs(sreg_t reg) {
    asm volatile ("movw %0, %%fs" : : "rm"(reg));
}

/**
 * @brief 读取gs
 * 
 * @return gs 
 */
inline static sreg_t rdgs(void) {
    sreg_t reg;
    asm volatile ("movw %%gs, %0" : "=rm"(reg));
    return reg;
}

/**
 * @brief 写gs
 * 
 * @param reg gs
 */
inline static void stgs(sreg_t reg) {
    asm volatile ("movw %0, %%gs" : : "rm"(reg));
}

/**
 * @brief 读取ss
 * 
 * @return ss 
 */
inline static sreg_t rdss(void) {
    sreg_t reg;
    asm volatile ("movw %%ss, %0" : "=rm"(reg));
    return reg;
}

/**
 * @brief 写ss
 * 
 * @param reg ss
 */
inline static void stss(sreg_t reg) {
    asm volatile ("movw %0, %%ss" : : "rm"(reg));
}