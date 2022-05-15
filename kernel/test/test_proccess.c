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
 * test_proccess.c
 * 
 * 测试进程
 * 
 */



#include "test_proccess.h"
#include <tayhuang/boot_args.h>
#include <tayhuang/defs.h>
#include <tayhuang/paging.h>
#include <tayhuang/descs.h>
#include <tayhuang/io.h>
#include <string.h>

#include "../kheap.h"

#include "../memory/paging.h"
#include "../memory/pmm.h"

#include "../display/video.h"
#include "../display/printk.h"

#include "../intterup/init_int.h"
#include "../intterup/irq_handler.h"

#include "../init/init_clock.h"
#include "../init/init_misc.h"

#include "../process/task_manager.h"

#include "../keyboard/keyboard.h"

#include "../syscall/syscall.h"
#include "../syscall/system_api.h"

PRIVATE int get_ticks(void) {
    qword pack[20] = {0x00};
    int ticks = 0;
    send_msg(pack, 0x10000, sizeof(pack));
    receive_msg(&ticks, 0x10000);
    return ticks;
}

PRIVATE void ipc_puts(const char *str) {
    qword pack[] = {0x00, (qword)str};
    send_msg(pack, 0x10001, sizeof(pack));
}

PRIVATE void ipc_putchar(char ch) {
    qword pack[] = {0x01, ch};
    send_msg(pack, 0x10001, sizeof(pack));
}

PRIVATE void ipc_printint(int num) {
    qword pack[] = {0x02, num};
    send_msg(pack, 0x10001, sizeof(pack));
}

PRIVATE void delay(int wait_ticks) { //延迟函数
    int last_ticks = get_ticks();
    while (get_ticks() - last_ticks < wait_ticks);
}

PRIVATE char *getline(char *buffer) { //获取一行的输入
    char *result = buffer;
    char ch = getchar();
    while (ch != '\n') {
        *buffer = ch;
        ch = getchar();
        buffer ++;
    }
    *buffer = '\0';
    return result;
}

PRIVATE bool start_with(const char *str, const char *prefix) { //判断前缀
    int len = strlen(prefix);
    for (int i = 0 ; i < len ; i ++) {
        if (str[i] != prefix[i])
            return false;
    }
    return true;
}

void fake_shell(void) { //假shell
    char buffer[64];
    while (true) {
        int color = get_print_color();
        set_print_color(0x0A);

        putchar ('>'); //提示符

        set_print_color(color);

        flush_to_screen();

        getline (buffer); //获取命令
        if (start_with(buffer, "echo ")) { //echo命令
            int color = get_print_color();
            set_print_color(0x0A);

            int len = strlen(buffer);
            for (int i = 5 ; i < len ; i ++) {
                putchar (buffer[i]);
            }

            set_print_color(color);
            putchar ('\n');
        }
        else { //错误输入
            printk ("Wrong Input!\n");
        }
    }
}

void keyboard_handler(void) {
    while (true) {
        deal_key(); //处理按键
    }
}

void tick_display(void) {
    while (true) {
        int posx = get_pos_x(), posy = get_pos_y();
        change_pos(0, 0); //设置打印位置
        int color = get_print_color();
        set_print_color(0x0D);

        printk ("Current Startup Time(s): %d\n", get_ticks() / 50); //打印

        set_print_color(color);
        change_pos(posx, posy);

        delay(25 * 2); //延迟一会儿再打印
    }
}

volatile int pid1 = 0, pid2 = 0;

void __test_proc1(void) {
    pid1 = current_task->pid;
    printk ("hello1!\n");
    while (pid2 == 0);
    printk ("hello1!\n");
    //delay(100);
    send_msg("Hello, IPC!", pid2, 12);
    while(true);
}

void __test_proc2(void) {
    pid2 = current_task->pid;
    printk ("hello2!\n");
    while (pid1 == 0);
    printk ("hello2!\n");
    //delay(100);
    char msg[20] = {};
//    receive_msg(msg, pid1); //第一种方法
    while (receive_any_msg(msg) == -1); //第二种方法
    printk ("some process says: \"%s\"\n", msg);
    while(true);
}