#ifndef _IDT_H
#define _IDT_H

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "keyboard.h"
#include "rtc.h"
#include "systemCalls.h"
//#include "scheduling.h"

#define intr_VEC 0x2F
#define keyboard_VEC 0x21
#define rtc_VEC 0x28
#define syscall_VEC 0x80
#define PIT_VEC 0x20


/* set up the IDT table */
extern void make_idt();


/*The following are the different interrupts that can be called.
 * Each function prints out what interrupt occurred: */

extern void
divide_by_zero();

extern void
debug_exception();

extern void
NMI_intr();

extern void
breakpoint_intr();

extern void
overflow_intr();

extern void
BR_intr();

extern void
invalid_opcode();

extern void
device_NA();

extern void
double_fault();

extern void
segement_overrun();

extern void
invalid_TSS();

extern void
segment_not_present();

extern void
stack_segment_fault();

extern void
general_protection();

extern void
page_fault();

extern void
math_fault();

extern void
alignment_check();

extern void
machine_check();

extern void
floatingpoint_exception();

#endif
