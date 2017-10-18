#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "pages.h"
#include "asm2.h"

/*declare global objects and align with 4KiB*/
//create pageDirectory
uint32_t pageDirectory[KILO] __attribute__((aligned(FOURKILO)));

//create pageTabele
uint32_t pageTable[KILO] __attribute__((aligned(FOURKILO)));

//create pageTable for user vid memory
uint32_t user_vid_pt[KILO] __attribute__((aligned(FOURKILO)));


/*
 * setup_paging()
 *   DESCRIPTION: Initializes and enables paging.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Blanks each entry in the pagedirectory, creates pagetable,
 *				   puts the page table into the page directory, and enables
 *				   paging by calling an in-line assembly function
 */

/* ref: http://wiki.osdev.org/Setting_Up_Paging */
void
setup_paging(){


    int i;

    //set each entry to not present
    for(i = 0; i < KILO; i++){
        pageDirectory[i] = SL_WRITE_NOTPRESENT; //sets supervisor, write enabled, and not present
    }

    // (i) holds the physical address where we want to start mapping these pages to.
	// in this case, we want to map these pages to the very beginning of memory.

	//we will fill all 1024 entries in the table, mapping 4 megabytes
    for(i = 0; i < KILO; i++){

    	// As the address is page aligned, it will always leave 12 bits zeroed.
    	// Those bits are used by the attributes
        pageTable[i] = (i * PAGE_SIZE) | SL_WRITE_NOTPRESENT; // attributes: supervisor level, read/write, not present.
    }

    //at video memory, make page table present!
    //attributes: present
    pageTable[VIDEOMEM/PAGE_SIZE] = pageTable[VIDEOMEM/PAGE_SIZE] | PRESENT;

    //put page table in page directory
    //attributes: supervisor level, read/write, present
    pageDirectory[0] = ((unsigned int)pageTable) | SL_WRITE_PRESENT;

    //attributes: size (set to 1 for 4MB page), supervisor level, r/w, present
    //kernel memory starts at 4MB
    pageDirectory[1] = FOURMB | SIZE_SL_WRITE_PRESENT;


    //page table for video memory
    pageTable[TERM1/PAGE_SIZE] = TERM1| SL_WRITE_PRESENT;
    pageTable[TERM2/PAGE_SIZE] = TERM2| SL_WRITE_PRESENT;
    pageTable[TERM3/PAGE_SIZE] = TERM3| SL_WRITE_PRESENT;

    //turn on paging
    asm_func_init(&pageDirectory[0]);

}


/*
 * map_prog()
 *   DESCRIPTION: Maps a program to memory using the page directory
 *   INPUTS: phy_addr - physical address
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: flushes the tlbs
 */
void map_prog(uint32_t phy_addr){
    //make the page at that virtual address present
    //Shift right by 22 to get page dir entry for 4mb pages
    pageDirectory[PROG_VIRT_ADDR/FOURMB] = phy_addr|FOURMBUSER_PRESENT;

    //flush the tlbs
    asm volatile(
            "cli;"
            "movl %0, %%eax;"	/* loads address of page directoy */
            "movl %%eax, %%cr3;" /*store address into cr3 */
            "sti;"

            :		/*no outputs */
            :"r"(pageDirectory) /*page directory = input */
            :"%eax"		/*clobbered register */
    );

}

/*
 * map_vid()
 *   DESCRIPTION: maps video memory to a page table
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: USER_VID_MEM - virtual address
 *   SIDE EFFECTS: none
 */
uint32_t map_vid(){
    uint32_t i;
    //initialise page table to point to vide memory
    for(i = 0; i < KILO; i++){

    	// As the address is page aligned, it will always leave 12 bits zeroed.
    	// Those bits are used by the attributes
        user_vid_pt[i] = FOURKBUSER_NOT_PRESENT;
    }
    //shift to right by 12, then take only lower 10 bits
    user_vid_pt[(USER_VID_MEM/PAGE_SIZE) & PT_MASK] = VIDEOMEM|FOURKBUSER_PRESENT;

    //change page dir entries - sshif by 22
    pageDirectory[USER_VID_MEM/FOURMB] = ((uint32_t)user_vid_pt|FOURKBUSER_PRESENT)&USER;


    return USER_VID_MEM;
}




//////////////// Functions for terminal switching  ////////////////////////
/*
 * turn_on_video()
 *   DESCRIPTION: Function that turns on video memory for a terminal
 *   INPUTS: terminal - number that holds which terminanl we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Gets the terminals page and remaps it to video memory
 */

void turn_on_video(uint32_t terminal){
    //get addres for the terminal
    uint32_t address;
    if (terminal>2)
        return;
    //get right virtual address
    switch (terminal){
        case (0) :  address = TERM1; break;
        case (1) : address = TERM2; break;
        case (2) : address = TERM3; break;
    }
    //remaps to video memory
    pageTable[VIDEOMEM/PAGE_SIZE] = VIDEOMEM | SL_WRITE_PRESENT;
    //also remap user level video
    user_vid_pt[(USER_VID_MEM/PAGE_SIZE) & PT_MASK] = VIDEOMEM|FOURKBUSER_PRESENT;
    //clear tlbs
    asm volatile(
            "cli;"
            "movl %0, %%eax;"	/* loads address of page directoy */
            "movl %%eax, %%cr3;" /*store address into cr3 */
            "sti;"

            :		/*no outputs */
            :"r"(pageDirectory) /*page directory = input */
            :"%eax"		/*clobbered register */
    );

    //copies page in term to vide memory
    memcpy((void*) VIDEOMEM, (void*)address, (uint32_t)PAGE_SIZE);
}


/*
 * turn_off_video()
 *   DESCRIPTION: Turns off video memory for a terminal. 
 *   INPUTS: terminal - number that holds which terminanl we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: copies video memory into terminal address to save it
 */

void turn_off_video(uint32_t terminal){
    //copies video memory ont right page
    uint32_t address;
    //terminal index can't go passed 2 (0,1,2)
    if (terminal>2)
        return;
    //get right address
    switch (terminal){
        case (0) :  address = TERM1; break;
        case (1) : address = TERM2; break;
        case (2) : address = TERM3; break;
    }
    //copy video memory to address
    memcpy( (void*)address,(void*) VIDEOMEM, (uint32_t)PAGE_SIZE);
    //remaps the page away from video memory
    pageTable[VIDEOMEM/PAGE_SIZE] = address | SL_WRITE_PRESENT;
    //remap user level video map
    user_vid_pt[(USER_VID_MEM/PAGE_SIZE) & PT_MASK] = address|FOURKBUSER_PRESENT;
    //clear tlbs
    asm volatile(
            "cli;"
            "movl %0, %%eax;"	/* loads address of page directoy */
            "movl %%eax, %%cr3;" /*store address into cr3 */
            "sti;"

            :		/*no outputs */
            :"r"(pageDirectory) /*page directory = input */
            :"%eax"		/*clobbered register */
    );

}


/*
 * save_video()
 *   DESCRIPTION: Copies video memory without changing the mapping. 
 *   INPUTS: terminal - number that holds which terminal we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void save_video (uint32_t terminal){
    //copies video memory ont right page
    uint32_t address;
    if (terminal>MAX_TERMINALS)
        return;
    //get right address
    switch (terminal){
        case (0) :  address = TERM1; break;
        case (1) : address = TERM2; break;
        case (2) : address = TERM3; break;
    }
    //copy video memory to address
    memcpy( (void*)address,(void*) VIDEOMEM, (uint32_t)PAGE_SIZE);
}

/*
 * load_video()
 *   DESCRIPTION: Copies video buffer to video memory 
 *   INPUTS: terminal - number that holds which terminal we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void load_video (uint32_t terminal){
    //copies video memory ont right page
    uint32_t address;
    if (terminal>MAX_TERMINALS)
        return;
    //get right address
    switch (terminal){
        case (0) :  address = TERM1; break;
        case (1) : address = TERM2; break;
        case (2) : address = TERM3; break;
    }
    clear();
    //copy vbuffer to video memory
    memcpy( (void*)VIDEOMEM,(void*) address, (uint32_t)PAGE_SIZE);
}

/*
 * route_video()
 *   DESCRIPTION: Routes to physical video memory.
 *   INPUTS: terminal - number that holds which terminal we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void route_video(uint32_t terminal) {
    //pt physical video addr
    pageTable[VIDEOMEM/PAGE_SIZE] = VIDEOMEM | SL_WRITE_PRESENT;
    //also remap user level video
    user_vid_pt[(USER_VID_MEM/PAGE_SIZE) & PT_MASK] = VIDEOMEM|FOURKBUSER_PRESENT;
    //clear tlbs
    asm volatile(
            "cli;"
            "movl %0, %%eax;"	/* loads address of page directoy */
            "movl %%eax, %%cr3;" /*store address into cr3 */
            "sti;"

            :		/*no outputs */
            :"r"(pageDirectory) /*page directory = input */
            :"%eax"		/*clobbered register */
    );
}

/*
 * route_buffer()
 *   DESCRIPTION: connects the buffer to video memory
 *   INPUTS: terminal - number that holds which terminal we're in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void route_buffer (uint32_t terminal){
    uint32_t address;
    if (terminal>MAX_TERMINALS)
        return;
    //get right address
    switch (terminal){
        case (0) :  address = TERM1; break;
        case (1) : address = TERM2; break;
        case (2) : address = TERM3; break;
    }
    //remaps the page away from video memory
    pageTable[VIDEOMEM/PAGE_SIZE] = address | SL_WRITE_PRESENT;
    //remap user level video map
    user_vid_pt[(USER_VID_MEM/PAGE_SIZE) & PT_MASK] = address|FOURKBUSER_PRESENT;
    //clear tlbs
    asm volatile(
            "cli;"
            "movl %0, %%eax;"	/* loads address of page directoy */
            "movl %%eax, %%cr3;" /*store address into cr3 */
            "sti;"

            :		/*no outputs */
            :"r"(pageDirectory) /*page directory = input */
            :"%eax"		/*clobbered register */
    );
}
