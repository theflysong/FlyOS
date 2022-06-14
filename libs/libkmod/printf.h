/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * printf.h
 *
 * printf
 *
 */



#pragma once

#include <tayhuang/defs.h>

int get_print_color(void);
void set_print_color(int color);
void change_pos(int x, int y);
void change_cursor_pos(int x, int y);
int get_pos_x(void);
int get_pos_y(void);
int get_scroll_line(void);
void set_scroll_line(int line);
void scroll_screen(int lines);
bool flush_to_screen(void);

void clrscr(void);
void putchar(char ch);
void puts(const char *str);
int printf(const char *format, ...);
int sprintf(char *buffer, const char *format, ...);

void disable_cursor_move(void);
void enable_cursor_move(void);