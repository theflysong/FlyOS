/*
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *    Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: theflysong
 *
 * main.c
 *
 * setup主函数
 *
 */



#include <debug/logging.h>

#include <assert.h>

#include <syscall/syscall.h>
#include <syscall/ipc.h>

#include <memory/malloc.h>

#include <tayhuang/services.h>

#include <disk.h>

PUBLIC void kmod_main(void) {
    set_logging_name("Setup");
    linfo ("233");

    reg_irq(14);
    reg_irq(15);

    void *boot = malloc(512);
    partition_info partition;
    get_partition(DISK_SEL_IDE0_MASTER, 0, &partition);
    
    linfo ("%d:%s", partition.start_lba, partition.state == PS_BOOTABLE ? "true" : "false");

    while (true);
}