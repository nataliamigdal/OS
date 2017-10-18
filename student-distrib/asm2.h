#ifndef _ASM2_H
#define _ASM2_H

#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "multiboot.h"
#include "i8259.h"

 #include "idt.h"
// #include "x86_desc.h"
// #include "lib.h"
// #include "rtc.h"
// #include "keyboard.h"
 #include "pages.h"



/*In-line assembly function that enables paging using cr3, cr4, and cr0 */
extern void asm_func_init(uint32_t* page_directory);

#endif
