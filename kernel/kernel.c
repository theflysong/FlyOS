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
 * kernel.c
 *
 * 内核主程序
 *
 */



#include <tayhuang/boot_args.h>
#include <tayhuang/defs.h>
#include <tayhuang/paging.h>
#include <tayhuang/descs.h>
#include <tayhuang/io.h>
#include <tayhuang/services.h>
#include <tayhuang/control_registers.h>

#include <string.h>
#include <assert.h>

#include <memory/kheap.h>
#include <memory/pmm.h>
#include <memory/paging.h>

#include <intterup/init_int.h>
#include <intterup/irq_handler.h>
#include <intterup/clock/clock.h>

#include <task/task_manager.h>
#include <task/task_scheduler.h>

#include <syscall/syscall.h>
#include <syscall/syscalls.h>

#include <printk.h>
#include <logging.h>
#include <global.h>
#include <kmod_loader.h>

PUBLIC void *kernel_pml4 = NULL;
PUBLIC struct boot_args args;
PUBLIC int cur_pid = 0;

PRIVATE struct desc_struct GDT[16];
PRIVATE struct gdt_ptr gdtr;
PRIVATE struct tss TSS;

PRIVATE inline void set_tss(void *addr, qword base, dword limit, byte type) { //设置TSS
    struct tss_struct *desc = (struct tss_struct*)addr;
    memset(desc, 0, sizeof(struct tss_struct));

    desc->limit0 = (word)limit;
    desc->limit1 = (limit >> 16) & 0xF;
    desc->base0 = (word)base;
    desc->base1 = (base >> 16) & 0xFF;
    desc->base2 = (base >> 24) & 0xFF;
    desc->base3 = (dword)(base >> 32);
    desc->type = type;
    desc->p = 1;
}

PRIVATE void init_gdt(void) { //初始化GDT
    GDT[0] = (struct desc_struct)GDT_ENTRY(0, 0, 0);
    GDT[cs0_idx] = (struct desc_struct)GDT_ENTRY(0xA09A, 0, 0xFFFFF);
    GDT[cs1_idx] = (struct desc_struct)GDT_ENTRY(0xA0BA, 0, 0xFFFFF);
    GDT[cs3_idx] = (struct desc_struct)GDT_ENTRY(0xA0FA, 0, 0xFFFFF);
    GDT[ds0_idx] = (struct desc_struct)GDT_ENTRY(0xA092, 0, 0xFFFFF);
    GDT[ds1_idx] = (struct desc_struct)GDT_ENTRY(0xA0B2, 0, 0xFFFFF);
    GDT[ds3_idx] = (struct desc_struct)GDT_ENTRY(0xA0F2, 0, 0xFFFFF);

    set_tss(&GDT[tr_idx], &TSS, sizeof(TSS), DESC_TSS); //设置TSS

    gdtr.ptr = GDT;
    gdtr.len = sizeof (GDT);
    asmv ("lgdt %0" : : "m"(gdtr)); //加载GDT
    stds(ds0_idx << 3); //初始化段寄存器
    stes(ds0_idx << 3);
    stfs(ds0_idx << 3);
    stgs(ds0_idx << 3);
    stss(ds0_idx << 3);
    int _tr_idx = tr_idx << 3; //初始化TR寄存器
    asmv ("ltr %0" : : "m"(_tr_idx));
}

static inline void init_sse(void) { //SSE
    cr0_t cr0 = get_cr0();
    cr0.em = 0;
    cr0.mp = 1;
    set_cr0(cr0);

    cr4_t cr4 = get_cr4();
    cr4.osfxsr = 1;
    cr4.osxmmexcpt = 1;
    set_cr4(cr4);
}

#define CLOCK_FREQUENCY (50.0f) //时钟周期

#define SETUP_MOD_BASE (64 * 1024)
#define SETUP_MOD_SIZE (320 * 1024)
#define KHEAP_PRE_BASE (SETUP_MOD_BASE + SETUP_MOD_SIZE)
#define KHEAP_PRE_SIZE (640 * 1024 - KHEAP_PRE_BASE)

//0 ~ 32MB系统保留
#define RESERVED_LIMIT (0x2000000)

void print_mod_info(program_info *mod_info) {
    linfo ("KMod Loader", "--------------------------------------------");
    linfo ("KMod Loader", "Start:        %p", mod_info->start);
    linfo ("KMod Loader", "End:          %p", mod_info->end);
    linfo ("KMod Loader", "Entry:        %p", mod_info->entry);
    linfo ("KMod Loader", "PGD:          %p", mod_info->pgd);
    linfo ("KMod Loader", "Stack Bottom: %p", mod_info->stack_bottom);
    linfo ("KMod Loader", "Stack Top:    %p", mod_info->stack_top);
    linfo ("KMod Loader", "Heap Bottom:  %p", mod_info->heap_bottom);
    linfo ("KMod Loader", "Heap Top:     %p", mod_info->heap_top);
    linfo ("KMod Loader", "--------------------------------------------");
}

PRIVATE VOLATILE bool i1_ready = false;
PRIVATE VOLATILE bool i2_ready = false;

//first task-init
PUBLIC void init(void) {
    //do something
    start_schedule();
    
    linfo ("Init", "hi");

    //-----------SETUP MOD-----------------
    // program_info setup_mod_info = load_kmod_from_memory(SETUP_MOD_BASE);
    // print_mod_info(&setup_mod_info);
    
    // initialize_kmod_task(
    //     create_task(DS_KERNEL, setup_mod_info.stack_top, setup_mod_info.stack_bottom, setup_mod_info.entry, CS_KERNEL, RFLAGS_KERNEL,
    //                 setup_mod_info.pgd, setup_mod_info.start, setup_mod_info.end, setup_mod_info.start, setup_mod_info.end, setup_mod_info.heap_bottom, setup_mod_info.heap_top,setup_mod_info.start, setup_mod_info.end,
    //                 SETUP_SERVICE, 1, 0, current_task));
    
    void *mail = kmalloc(8192);
    setmail_buffer(mail, 8192);

    wait_ipc();

    char buffer[256] = {};
    assert(recv_msg(buffer) != -1);
    linfo ("Init", "%s", buffer);

    assert(sendmsg ("Hello, Init2", 13, 2));
    wait_ipc();

    while (true);
}

PUBLIC void init2(void) {
    linfo ("Init2", "hi");

    void *mail = kmalloc(8192);
    setmail_buffer(mail, 8192);

    assert(sendmsg ("Hello Init!", 12, 1));
    wait_ipc();

    char buffer[256] = {};
    assert(recv_msg(buffer));
    linfo ("Init2", "%s", buffer);

    while (true);
}

PUBLIC void mapping_kernel(void *pgd) {
    set_mapping(pgd, NULL, NULL, RESERVED_LIMIT / MEMUNIT_SZ, true, false);
}

//初始化
PUBLIC void initialize(void) {
    //初始化GDT
    init_gdt();
    
    //初始化输出
    init_serial();

    init_video(args.framebuffer, args.screen_width, args.screen_height);

    //初始化中断
    init_idt();
    init_pic();

    init_sse();
    init_pit(CLOCK_FREQUENCY);

    en_int();

    //复制setup mod
    memcpy (SETUP_MOD_BASE, args.setup_mod_addr, SETUP_MOD_SIZE);

    //初始化内存
    init_kheap(KHEAP_PRE_BASE, KHEAP_PRE_SIZE);

    qword memsz = (((qword)args.memory_size_high) << 32) + args.memory_size;

    init_pmm(memsz, 0x2000000);

    kernel_pml4 = create_pgd();

    set_mapping(kernel_pml4, NULL, NULL, memsz / MEMUNIT_SZ, true, true);
    mapping_kernel(kernel_pml4);
    
    if (((qword)args.framebuffer) >= memsz) {
        set_mapping(kernel_pml4, args.framebuffer, args.framebuffer, (args.screen_width * args.screen_height * 3 * 2) / MEMUNIT_SZ, true, true);
    }

    __set_cr3(kernel_pml4);

    create_empty_task(); //创建空进程
    cur_pid = 1;

    //初始化TSS
    TSS.ist1 = IST0_STACKTOP;
    TSS.rsp0 = RING0_STACKTOP;

    //初始化中断
    register_irq_handler(0, clock_int_handler);
    enable_irq(0);
}

PUBLIC void entry(struct boot_args *_args) {
    //Boot Arg
    if (_args->magic != BOOT_ARGS_MAGIC) {
        while (true);
    }
    memcpy(&args, _args, sizeof(struct boot_args));

    initialize(); //初始化

    linfo ("Kernel", "Hello, I'm TayhuangOS Kernel!");

    current_task = __create_task(DS_KERNEL, RING0_STACKTOP, RING0_STACKBOTTOM, init, CS_KERNEL, RFLAGS_KERNEL,
                 kernel_pml4, 0, 0, 0, 0, 0, 0, 0, 0,
                 1, 1, 0, NULL
    );

    add_task(current_task);
    current_task->state = RUNNING;
    current_task->count = 3;
    
    create_task(DS_KERNEL, RING0_STACKTOP2, RING0_STACKBOTTOM2, init2, CS_KERNEL, RFLAGS_KERNEL,
                 kernel_pml4, 0, 0, 0, 0, 0, 0, 0, 0,
                 2, 1, 0, current_task
    );

    asmv ("movq %0, %%rsp" : : "g"(RING0_STACKTOP));
    asmv ("jmp init");

    while (true);
}