// SPDX-License-Identifier: GPL-2.0-only
/* -------------------------------*-TayhuangOS-*-----------------------------------
 *
 *   Copyright (C) 2022, 2022 TayhuangOS Development Team - All Rights Reserved
 *
 * --------------------------------------------------------------------------------
 *
 * Author: Flysong
 *
 * kernel/boot/stage3/init.c
 *
 * Initialize functions are implemented here
 */



#include "init.h"
#include "int_handlers.h"
#include <int_vectors.h>
#include <ports.h>

struct desc_struct GDT[GDT_SIZE];
struct gdt_ptr gdtr;
struct gate_struct IDT[IDT_SIZE];
struct desc_ptr idtr;

PUBLIC void init_gdt(void) {
    GDT[0] = (struct desc_struct)GDT_ENTRY(0, 0, 0xFFFFF);
    GDT[1] = (struct desc_struct)GDT_ENTRY(0xC09A, 0, 0xFFFFF);
    GDT[2] = (struct desc_struct)GDT_ENTRY(0xC093, 0, 0xFFFFF);
    GDT[3] = (struct desc_struct)GDT_ENTRY(0x0089, 4096, 103);
    gdtr.ptr = GDT;
    gdtr.len = sizeof (GDT) - 1;
    asmv ("lgdtl %0" : : "m"(gdtr));
}

PUBLIC void init_8259A(void) {
    outb (M_8042A_CONTROL, 0x11);
    outb (S_8042A_CONTROL, 0x11);

    outb (M_8042A_CONTROLMASK, INT_VECTOR_IRQ0);
    outb (S_8042A_CONTROLMASK, INT_VECTOR_IRQ8);

    outb (M_8042A_CONTROLMASK, 0x4);
    outb (S_8042A_CONTROLMASK, 0x2);

    outb (M_8042A_CONTROLMASK, 0x1);
    outb (S_8042A_CONTROLMASK, 0x1);
    
    outb (M_8042A_CONTROLMASK, 0xFF);
    outb (S_8042A_CONTROLMASK, 0xFF);
}

PUBLIC void init_idt_desc(byte vector, byte type, int_handler handler, byte privilege) {
    dword base = (dword)handler;
    IDT[vector].offset_low = base & 0xFFFF;
    IDT[vector].segment = rdcs();
    IDT[vector].bits.ist = 0;
    IDT[vector].bits.zero = 0;
    IDT[vector].bits.type = type;
    IDT[vector].bits.dpl = privilege;
    IDT[vector].bits.p = 1;
    IDT[vector].offset_middle = base >> 16;
#ifdef ARCH_x86_64
    IDT[vector].offset_high = 0;
    IDT[vector].reserved = 0;
#endif
}

PRIVATE void _int_idt_descs(void) {
    init_idt_desc(0, GATE_INTERRUPT, divide_by_zero_error, 0);
    init_idt_desc(1, GATE_INTERRUPT, single_step_debug, 0);
    init_idt_desc(2, GATE_INTERRUPT, non_maskable_interrup, 0);
    init_idt_desc(3, GATE_INTERRUPT, breakpoint, 0);
    init_idt_desc(4, GATE_INTERRUPT, overflow, 0);
    init_idt_desc(5, GATE_INTERRUPT, bound_range_exceeded, 0);
    init_idt_desc(6, GATE_INTERRUPT, invalid_opcode, 0);
    init_idt_desc(7, GATE_INTERRUPT, device_not_available, 0);
    init_idt_desc(8, GATE_INTERRUPT, double_fault, 0);
    init_idt_desc(9, GATE_INTERRUPT, coprocessor_segment_overrun, 0);
    init_idt_desc(10, GATE_INTERRUPT, invalid_tss, 0);
    init_idt_desc(11, GATE_INTERRUPT, segment_not_present, 0);
    init_idt_desc(12, GATE_INTERRUPT, stack_segment_fault, 0);
    init_idt_desc(13, GATE_INTERRUPT, general_protection_fault, 0);
    init_idt_desc(14, GATE_INTERRUPT, page_fault, 0);
    init_idt_desc(15, GATE_INTERRUPT, reserved1_excepetion, 0);
    init_idt_desc(16, GATE_INTERRUPT, x87_floating_point_exception, 0);
    init_idt_desc(17, GATE_INTERRUPT, alignment_check, 0);
    init_idt_desc(18, GATE_INTERRUPT, machine_check, 0);
    init_idt_desc(19, GATE_INTERRUPT, simd_floating_point_exception, 0);
    init_idt_desc(20, GATE_INTERRUPT, virtualization_exception, 0);
    init_idt_desc(21, GATE_INTERRUPT, control_protection_exception, 0);
    init_idt_desc(22, GATE_INTERRUPT, reserved2_excepetion, 0);
    init_idt_desc(23, GATE_INTERRUPT, reserved3_excepetion, 0);
    init_idt_desc(24, GATE_INTERRUPT, reserved4_excepetion, 0);
    init_idt_desc(25, GATE_INTERRUPT, reserved5_excepetion, 0);
    init_idt_desc(26, GATE_INTERRUPT, reserved6_excepetion, 0);
    init_idt_desc(27, GATE_INTERRUPT, reserved7_excepetion, 0);
    init_idt_desc(28, GATE_INTERRUPT, hypervisor_injection_exception, 0);
    init_idt_desc(29, GATE_INTERRUPT, vmm_communication_exception, 0);
    init_idt_desc(30, GATE_INTERRUPT, security_exception, 0);
    init_idt_desc(31, GATE_INTERRUPT, reserved8_excepetion, 0);
}

PUBLIC void init_idt(void) {
    idtr.address = IDT;
    idtr.size = sizeof(IDT) - 1;
    asmv ("lidtl %0" : : "m"(idtr));
    _int_idt_descs();
}