/*
 * SPDX-License-Identifier: LGPL-2.1-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 * 
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 * 
 * --------------------------------------------------------------------------------
 * 
 * 作者: theflysong
 * 
 * command.c
 * 
 * 命令
 * 
 */



#include <global.h>
#include <export/__video_driver_fn.h>

#include <debug/logging.h>

#include <syscall/syscall.h>
#include <syscall/ipc.h>
#include <syscall/rpc.h>

#include <memory/malloc.h>

PRIVATE void write_char(int column, int line, byte color, byte ch) {
    *(byte*)(video_info.framebuffer + ((line * video_info.width) + column) * 2 + 0) = ch;
    *(byte*)(video_info.framebuffer + ((line * video_info.width) + column) * 2 + 1) = color;
}

PRIVATE rpc_args_struct wrapper_write_char(int caller, rpc_func func_no, rpc_args_struct args) {
    int column = ARG_READ(args.data, int);
    int line = ARG_READ(args.data, int);
    byte color = ARG_READ(args.data, byte);
    byte ch = ARG_READ(args.data, byte);

    color &= 0xFF;
    ch &= 0xFF;

    write_char(column, line, color, ch);

    bool *result = malloc(sizeof(TEXT_WRITE_CHAR_RETURN_TYPE));
    *result = true;

    return (rpc_args_struct){.data = result, .size = sizeof(TEXT_WRITE_CHAR_RETURN_TYPE)};
}

PRIVATE rpc_args_struct wrapper_write_string(int caller, rpc_func func_no, rpc_args_struct args) {
    int column = ARG_READ(args.data, int);
    int line = ARG_READ(args.data, int);
    byte color = ARG_READ(args.data, byte);
    int num = ARG_READ(args.data, int);

    color &= 0xFF;

    for (int i = 0 ; i < num ; i ++) {
        byte ch = ARG_READ(args.data, byte);
        ch &= 0xFF;
        write_char(column + i, line, color, ch);
    }

    bool *result = malloc(sizeof(TEXT_WRITE_STRING_RETURN_TYPE));
    *result = true;

    return (rpc_args_struct){.data = result, .size = sizeof(TEXT_WRITE_STRING_RETURN_TYPE)};
}

PUBLIC void text_register_rpc_functions(void) {
    rpc_register(TEXT_WRITE_CHAR_FN, wrapper_write_char, sizeof(TEXT_WRITE_CHAR_RETURN_TYPE), TEXT_WRITE_CHAR_ARGS_SIZE);
    rpc_register(TEXT_WRITE_STRING_FN, wrapper_write_string, sizeof(TEXT_WRITE_STRING_RETURN_TYPE), TEXT_WRITE_STRING_ARGS_SIZE);
}