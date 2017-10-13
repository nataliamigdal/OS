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

    //turn on paging
    asm_func_init(&pageDirectory[0]);

}
