#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"
#include "pages.h"
#include "asm2.h"


/*
 * asm_func_init
 *   DESCRIPTION: In-line assembly function that enables paging. 
 *   INPUTS: uint32_t* page_directory - instead of passing in the pageDirectory itself,
										our code required us to pass in the pointer of the
										address of pageDirectory[0]
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: puts address of page directory into cr3 register and then set the 
 *				   paging bit of the cr0 register (the 32th bit)
 */   
void asm_func_init(uint32_t* page_directory) {
	asm volatile(
			"movl %0, %%eax;"	/* loads address of page directoy */
			"movl %%eax, %%cr3;" /*store address into cr3 */

			"movl %%cr4, %%eax;"
			"orl $0x00000010, %%eax;"
			"movl %%eax, %%cr4;"	/* enables 4MiB pages*/

			"movl %%cr0, %%eax;"
			"orl $0x80000000, %%eax;"
			"movl %%eax, %%cr0;"	/*set paging bit */
			:		/*no outputs */
			:"r"(page_directory) /*page directory = input */
			:"%eax"		/*clobbered register */
	);
};

