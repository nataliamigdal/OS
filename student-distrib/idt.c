#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "keyboard.h"


/*
 * make_idt
 *   DESCRIPTION: Sets up the IDT table.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Sets up IDT entries individually for trap, interrupt, and system calls. 
 */   
void
make_idt()
{

int i = 0;

//for all vectors
for(i = 0; i < NUM_VEC; i++)
{
	SET_IDT_ENTRY(idt[i], NULL); //not really necessary, but good practice to set unused stuff to NULL
	idt[i].present = 1;
	idt[i].reserved4 = 0;
	idt[i].reserved3 = 0; //zero because not interrupt
	idt[i].reserved2 = 1;
	idt[i].reserved1 = 1;
	idt[i].reserved0 = 0;
	idt[i].size = 1; //want 32, not 16 bits
	idt[i].dpl = 0; //kernel mode
	idt[i].seg_selector = KERNEL_CS; //code executes in kernel
}

//for interrupt vectors
for(i = 0; i < intr_VEC; i++)
{
	//SET_IDT_ENTRY(idt[i], NULL);
	idt[i].reserved3 = 1; //indicates interrupt
}

//set  up entries in idt table
// only 0-19 are set for interrupts/exceptions
//all of these are sent to asm wrappers that 
//make sure not to modify the stack when an
//interrupt occurs
SET_IDT_ENTRY(idt[0], divide_by_zero_asm);
SET_IDT_ENTRY(idt[1], debug_exception_asm);
SET_IDT_ENTRY(idt[2], NMI_intr_asm);
SET_IDT_ENTRY(idt[3], breakpoint_intr_asm);
SET_IDT_ENTRY(idt[4], overflow_intr_asm);
SET_IDT_ENTRY(idt[5], BR_intr_asm);
SET_IDT_ENTRY(idt[6], invalid_opcode_asm);
SET_IDT_ENTRY(idt[7], device_NA_asm);
SET_IDT_ENTRY(idt[8], double_fault_asm);
SET_IDT_ENTRY(idt[9], segement_overrun_asm);
SET_IDT_ENTRY(idt[10], invalid_TSS_asm);
SET_IDT_ENTRY(idt[11], segment_not_present_asm);
SET_IDT_ENTRY(idt[12], stack_segment_fault_asm);
SET_IDT_ENTRY(idt[13], general_protection_asm);
SET_IDT_ENTRY(idt[14], page_fault_asm);
//SET_IDT_ENTRY(idt[15], intelreserved); <-- intel reserved. Do not use.
SET_IDT_ENTRY(idt[16], math_fault_asm);
SET_IDT_ENTRY(idt[17], alignment_check_asm);
SET_IDT_ENTRY(idt[18], machine_check_asm);
SET_IDT_ENTRY(idt[19], floatingpoint_exception_asm);

/*rtc is at 0x28 on the IDT table and
 * keyboard is at 0x21 on the table */
SET_IDT_ENTRY(idt[rtc_VEC], rtc_handler_asm);
SET_IDT_ENTRY(idt[keyboard_VEC], keyboard_handler_asm);

idt[syscall_VEC].dpl = 3; // =3 for system cal so that it's accessible from user space
SET_IDT_ENTRY(idt[0x80], systemcall_handler_asm);

//load LDT
lidt(idt_desc_ptr);

return;

}

/*
 * divide_by_zero
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
divide_by_zero()
{
	printf("divide by zero");
	while(1);
}

/*
 * debug_exception
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
debug_exception()
{
	printf("debug exception");
	while(1);
}


/*
 * NMI_intr
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
NMI_intr()
{
	printf("NMI interrupt");
	while(1);
}


/*
 * breakpoint_intr
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
breakpoint_intr()
{
	printf("breakpoint");
	while(1);
}


/*
 * overflow_intr
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
overflow_intr()
{
	printf("overflow");
	while(1);
}


/*
 * BR_intr
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
BR_intr()
{
	printf("BOUND range exceeded");
	while(1);
}


/*
 * inavlid_opcode
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
invalid_opcode()
{
	printf("Invalid opcode (undefined opcode)");
	while(1);
}


/*
 * device_NA
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
device_NA()
{
	printf("device not available (no math coprocessor)");
	while(1);
}


/*
 * double_fault
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
double_fault()
{
	printf("double fault");
	while(1);
}


/*
 * segement_overrun
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
segement_overrun()
{
	printf("coprocessor segment overrun (reserved)");
	while(1);
}


/*
 * invalid_TSS()
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
invalid_TSS()
{
	printf("invalid TSS");
	while(1);
}


/*
 * segment_not_present
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
segment_not_present()
{
	printf("segement not present");
	while(1);
}


/*
 * stack_segment_fault
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
stack_segment_fault()
{
	printf("stack-segment fault");
	while(1);
}


/*
 * general_protection
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
general_protection()
{
	printf("general protection");
	while(1);
}


/*
 * page_fault
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
page_fault()
{
	printf("page fault");
	while(1);
}


/*
 * math_fault
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
math_fault()
{
	printf("x87 FPU Floating-Point Error (math fault)");
	while(1);
}


/*
 * alignment_check
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
alignment_check()
{
	printf("alignment check");
	while(1);
}


/*
 * machine_check
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
machine_check()
{
	printf("machine check");
	while(1);
}


/*
 * floatingpoint_exception
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
floatingpoint_exception()
{
	printf("SIMD floating-point exception");
	while(1);
}

/*
 * systemcall_handler
 *   DESCRIPTION: prints out the associating interrupt/exception
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: Goes into a while(1) loop to halt 
 */  
void
systemcall_handler()
{
	printf("System call");
	while(1);
}

