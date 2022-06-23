/* 
 * SPDX-License-Identifier: GPL-3.0-only
 * -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * 作者: Flysong
 *
 * fat32.c
 *
 * FAT32文件系统
 *
 */



#include "fat32.h"
#include <lheap.h>
#include <disk.h>
#include <printf.h>
#include <ctype.h>
#include <string.h>
#include <tayhuang/partition.h>

struct fat32_info_struct {
    word _jmp_short; //跳转指令 +0x00
    byte _nop; //空指令 +0x02
    char oem_name[8]; //oem名 +0x03
    word bytes_per_sector; //每扇区字节数 +0x0B
    byte sectors_per_clus; //每簇扇区数 +0x0D
    word reversed_sectors; //保留扇区 +0x0E
    byte num_fats; //fat数 +0x10
    dword reserved0; // +0x11
    byte media; //介质 +0x15
    word reserved1; // +0x16
    word sectors_per_track; //每磁道扇区数 +0x18
    word heads; //磁头数 +0x1A
    dword hidden_sectors; //隐藏扇区数 +0x1C
    dword total_sectors; //总扇区数 +0x20
    dword fat_sectors; //fat扇区数 +0x24
    word reserved2; // +0x28
    word version; //版本号 +0x2A
    dword root_directory_start_clus; //根目录区起始簇号 +0x2C
    word fsinfo_sectors; //FS信息扇区数 +0x30
    word backup_boot_sectors; //引导备份扇区数 +0x32
    dword reserved3; // +0x34
    dword reserved4; // +0x38
    dword reserved5; // +0x3C
    byte drive_no; //驱动器号 +0x40
    byte reserved6; //驱动器号 +0x41
    byte boot_sig; //boot标志 +0x42
    dword volumn_id; //卷号 +0x43
    char volumn_label[11]; //卷标 +0x47
    char file_system[8]; //文件系统 +0x52
} __attribute__((packed));

typedef struct {
    // 
    int selector; //Selector of Context
    dword magic; //0x93186A8E (MD5 of "File Allocation Table 32"的前32位)
    partition_member partition;
    struct fat32_info_struct infomations;
    //Extension
    dword fat1_start; //fat1起始
    dword data_start; //数据区起始
    void *root_directory; //根目录数据
    void *fat_buffer; //FAT缓冲
    int buffer_start; //FAT缓冲开始扇区编号 (编号 + FAT1_START = 所在扇区)
} FAT32_CONTEXT;

#define FAT32_CONTEXT_MAGIC (0x93186A8E)
#define FAT32_FILE_ENTRY_SIZE (0x20)
#define FAT32_FILE_ENTRIES_PER_SECTOR (0x200 / FAT32_FILE_ENTRY_SIZE)
#define FAT32_ENTRY_SIZE (4)
#define FAT32_ENTRIES_PER_SECTOR (0x200 / FAT32_ENTRY_SIZE)

#define FAT_BUFFER_SECTORS (4)
#define INFO _context->infomations

PRIVATE dword get_fat32_entry(fs_context context, dword last) {
    FAT32_CONTEXT *_context = (FAT32_CONTEXT*)context;
    int num = (last / FAT32_ENTRIES_PER_SECTOR);
    int _sector = num + _context->fat1_start;
    int _offset = last % FAT32_ENTRIES_PER_SECTOR;

    if (num >= _context->buffer_start + FAT_BUFFER_SECTORS) { //不在缓冲区里
        read_sector(_sector, FAT_BUFFER_SECTORS, _context->selector, _context->fat_buffer); //读入
        _context->buffer_start = num; //设置开始编号
    }

    int num_off = num - _context->buffer_start;
    int offset = num_off * INFO.bytes_per_sector + //扇区间偏移量
                 _offset * sizeof(dword); //扇区内偏移量

    dword entry = *((dword*)(_context->fat_buffer + offset));
    return entry;
}

PRIVATE void __load_file(fs_context context, dword clus, void *dst, bool show_progress) {
    FAT32_CONTEXT *_context = (FAT32_CONTEXT*)context;
    //起始簇号为2
    int sectors_per_clus = INFO.sectors_per_clus;
    while (clus < 0x0FFFFFF0) {
        if (show_progress) {
            printf ("%#X->", clus);
        }
        int start_sector = _context->data_start + (clus - 2) * sectors_per_clus;
        for (int i = 0 ; i < sectors_per_clus ; i ++) {
            read_sector(start_sector + i, 1, _context->selector, dst);
            dst += 512;
        }
        clus = get_fat32_entry(context, clus);
    }
    if (show_progress) {
        printf("%#X\n", clus);
    }
}

PUBLIC fs_context load_fat32_fs(int disk_selector, int partition_id) {
    void *boot = lmalloc(512);
    FAT32_CONTEXT *context = lmalloc(sizeof(FAT32_CONTEXT));
    get_partition(disk_selector, partition_id, &context->partition);
    read_sector(context->partition.start_lba, 1, disk_selector, boot);

    //MARK
    context->selector = disk_selector;
    context->magic = FAT32_CONTEXT_MAGIC;
    
    memcpy (&context->infomations, boot, sizeof(context->infomations));

    //Extension
    context->fat1_start = context->infomations.reversed_sectors + context->partition.start_lba;
    context->data_start = context->fat1_start + context->infomations.num_fats * context->infomations.fat_sectors;
    
    context->root_directory = lmalloc(16 * context->infomations.bytes_per_sector); //16个扇区
    context->fat_buffer = lmalloc(FAT_BUFFER_SECTORS * context->infomations.bytes_per_sector); //4个扇区

    read_sector(context->fat1_start, FAT_BUFFER_SECTORS, disk_selector, context->fat_buffer);
    context->buffer_start = 0;

    //FIXME: Bug here
    __load_file(context, context->infomations.root_directory_start_clus, context->root_directory, false);

    lfree(boot);
    return context;
}

PUBLIC void terminate_fat32_fs(fs_context context) {
    lfree (((FAT32_CONTEXT*)context)->root_directory);
    lfree (((FAT32_CONTEXT*)context)->fat_buffer);
    lfree (context);
}

PRIVATE dword get_file_start_clus(fs_context context, const char *name) {
    if (strlen(name) != 11)
        return -1;
    FAT32_CONTEXT *_context = (FAT32_CONTEXT*)context;
    char rdname[12] = "";
    rdname[11] = 0;
    for (int i = 0 ; i < 16 * FAT32_ENTRIES_PER_SECTOR ; i ++) {
        memcpy(rdname, _context->root_directory + i * FAT32_ENTRY_SIZE + 0, 11);
        if (! strcmp(name, rdname)) {
            dword low_word = *(word*)(_context->root_directory + i * FAT32_ENTRY_SIZE + 0x1A); //低字
            dword high_word = *(word*)(_context->root_directory + i * FAT32_ENTRY_SIZE + 0x14); //高字
            return (high_word << 16) + low_word;
        }
    }
    return -1;
}

PRIVATE char *convert(const char *name, char *buffer) {
    char *backup = buffer;
    int cnt = 0;
    while (*name != '.') {
        *buffer = toupper(*name);
        buffer ++;
        name ++;
        cnt ++;
    }
    if (cnt > 8) {
        return NULL;
    }
    while (cnt < 8) {
        *buffer = ' ';
        buffer ++;
        cnt ++;
    }
    name ++;
    for (int i = 0 ; i < 3 ; i ++) {
        *buffer = toupper(*name);
        buffer ++;
        name ++;
    }
    *buffer = '\0';
    return backup;
}

PUBLIC bool load_fat32_file(fs_context context, const char *name, void *dst, bool show_progress) {
    FAT32_CONTEXT *_context = (FAT32_CONTEXT*)context;
    char _name[12];
    if (convert(name, _name) == NULL) {
        return false;
    }
    dword clus = get_file_start_clus(context, _name);
    if (clus == -1) {
        return false;
    }
    __load_file(_context, clus, dst, show_progress);
    return true;
}

PUBLIC void display_fat32_fs_info(void *context) {
    FAT32_CONTEXT *_context = (FAT32_CONTEXT*)context;

    char oem_name[9], volumn_label[12], file_system[9];
    memcpy (oem_name, INFO.oem_name, 8);
    memcpy (volumn_label, INFO.volumn_label, 11);
    memcpy (file_system, INFO.file_system, 8);
    oem_name[8] = volumn_label[11] = file_system[8] = '\0';

    printf ("OEM Name: %s;  Bytes per Sector : %d;  Sectors per Clus: %d\n",
        oem_name, INFO.bytes_per_sector, INFO.sectors_per_clus);
    printf ("Reversed Sectors: %#X;  FAT Num: %d; Total Sectors: %#X;  Media: %#02X\n",
        INFO.reversed_sectors, INFO.num_fats, INFO.total_sectors, INFO.media);
    printf ("FAT Sectors: %#X;  Sectors per Track: %d;  Heads: %d\n",
        INFO.fat_sectors, INFO.sectors_per_track, INFO.heads);
    printf ("Hidden Sectors: %#X;  Version: %d;  Root Directory Start Clus: %d\n",
        INFO.hidden_sectors, INFO.version, INFO.root_directory_start_clus);
    printf ("FSInfo Sectors: %#X;  Backup Boot Sectors: %#X;  Drive No: %#02X\n",
        INFO.fsinfo_sectors, INFO.backup_boot_sectors, INFO.drive_no);
    printf ("Boot Sig: %#02X;  Vol ID: %#08X;  Vol Label: %s\n",
        INFO.boot_sig, INFO.volumn_id, volumn_label);
    printf ("File System: %s; FAT1 Start: %#08X\n", file_system, _context->fat1_start);
    printf ("Data Start: %#08X ; Start LBA: %#08X\n", _context->data_start, _context->partition.start_lba);
}

#undef INFO