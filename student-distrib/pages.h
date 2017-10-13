#ifndef _PAGES_H
#define _PAGES_H

#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "multiboot.h"
#include "i8259.h"
#include "types.h"

#define VIDEOMEM 0xB8000  //video memory address////video memory address
#define PAGE_SIZE 0x1000 //from OSDev
#define FOURMB	0x400000 //four MB in PAGE_SIZE
#define KILO 1024
#define FOURKILO 4096

//attributes
#define SL_WRITE_PRESENT 0x3  //supervisor level, read/write, present
#define SL_WRITE_NOTPRESENT 0x2  //supervisor level, read/write, not present
#define SIZE_SL_WRITE_PRESENT 0x83 //size (4MB), supervisor level, read/write, present
#define PRESENT 0x1 	//only present is set

/* initializes and sets up paging */
extern void setup_paging();

#endif
