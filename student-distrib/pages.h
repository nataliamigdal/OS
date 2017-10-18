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
#define _8kb 0x2000
#define FOURMB	0x400000 //four MB in PAGE_SIZE
#define KILO 1024
#define FOURKILO 4096
#define PROG_VIRT_ADDR 0x08000000
#define USER_VID_MEM 0x08400000 //user video mem virtual address PDE:33, PTE: 0
#define PT_MASK 0x003FF

//attributes
#define SL_WRITE_PRESENT 0x3  //supervisor level, read/write, present
#define SL_WRITE_NOTPRESENT 0x2  //supervisor level, read/write, not present
#define SIZE_SL_WRITE_PRESENT 0x83 //size (4MB), supervisor level, read/write, present
#define PRESENT 0x1 	//only present is set
#define FOURMBUSER_PRESENT 0x87
#define FOURKBUSER_PRESENT 0x7
#define FOURKBUSER_NOT_PRESENT 0x6
//to turn of 4mb pages
#define USER 0xffffff7f
#define MAX_TERMINALS 3


//terminal video buffer
//set page entries for temp vide mem for terminals
#define TERM1 0xB9000
#define TERM2 0xBA000
#define TERM3 0xBB000
/* initializes and sets up paging */
extern void setup_paging();
//maps a prog into physical addr
extern void  map_prog(uint32_t phy_addr);
//maps an address to user memory
extern uint32_t map_vid();
//turns on video memory for a terminal
void turn_on_video(uint32_t terminal);
//Turns off video memory for a terminal.
void turn_off_video(uint32_t terminal);
//Copies video memory without changing the mapping
void save_video (uint32_t terminal);
//Copies video buffer to video memory
void load_video (uint32_t terminal);
//Routes to physical video memory
void route_video(uint32_t terminal);
//connects the buffer to video memory
void route_buffer (uint32_t terminal);
#endif
