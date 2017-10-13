.text

/* all global so that idt.c can use them */
.globl rtc_handler_asm, keyboard_handler_asm
.globl divide_by_zero_asm, debug_exception_asm, NMI_intr_asm, breakpoint_intr_asm
.globl overflow_intr_asm, BR_intr_asm, invalid_opcode_asm
.globl device_NA_asm, double_fault_asm, segement_overrun_asm, invalid_TSS_asm
.globl segment_not_present_asm, stack_segment_fault_asm ,general_protection_asm, page_fault_asm, math_fault_asm
.globl alignment_check_asm, machine_check_asm, floatingpoint_exception_asm, systemcall_handler_asm


/*
 * rtc_handler_asm
 *   DESCRIPTION: Calls interrupt handlers without modifying the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
rtc_handler_asm:
	pushal
	call rtc_handler
	popal
	iret


/*
 * keyboard_handler_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
keyboard_handler_asm:
	pushal
	call keyboard_handler
	popal
	iret


/*
 * divide_by_zero_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
divide_by_zero_asm:
	pushal
	call divide_by_zero
	popal
	iret
	

/*
 * debug_exception_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  	
debug_exception_asm:
	pushal
	call debug_exception
	popal
	iret
	

/*
 * NMI_intr_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
NMI_intr_asm:
	pushal
	call NMI_intr
	popal
	iret


/*
 * breakpoint_intr_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
breakpoint_intr_asm:
	pushal
	call breakpoint_intr
	popal
	iret


/*
 * overflow_intr_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
overflow_intr_asm:
	pushal
	call overflow_intr
	popal
	iret


/*
 * BR_intr_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  	
BR_intr_asm:
	pushal
	call BR_intr
	popal
	iret
	

/*
 * invalid_opcode_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
invalid_opcode_asm:
	pushal
	call invalid_opcode
	popal
	iret
	

/*
 * device_NA_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
device_NA_asm:
	pushal
	call device_NA
	popal
	iret
	

/*
 * double_fault_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
double_fault_asm:
	pushal
	call double_fault
	popal
	iret


/*
 * segement_overrun_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
segement_overrun_asm:
	pushal
	call segement_overrun
	popal
	iret

/*
 * invalid_TSS_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
invalid_TSS_asm:
	pushal
	call invalid_TSS
	popal
	iret


/*
 * segment_not_present_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
segment_not_present_asm:
	pushal
	call segment_not_present
	popal
	iret
	
/*
 * stack_segment_fault_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
stack_segment_fault_asm:
	pushal
	call stack_segment_fault
	popal
	iret


/*
 * general_protection_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
general_protection_asm:
	pushal
	call general_protection
	popal
	iret
	

/*
 * page_fault_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  	
page_fault_asm:
	pushal
	call page_fault
	popal
	iret
	

/*
 * math_fault_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
math_fault_asm:
	pushal
	call math_fault
	popal
	iret
	

/*
 * alignment_check_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  	
alignment_check_asm:
	pushal
	call alignment_check
	popal
	iret


/*
 * machine_check_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
machine_check_asm:
	pushal
	call machine_check
	popal
	iret


/*
 * floatingpoint_exception_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
floatingpoint_exception_asm:
	pushal
	call floatingpoint_exception
	popal
	iret


/*
 * systemcall_handler_asm
 *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none 
 *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
 */  
systemcall_handler_asm:
	pushal
	call systemcall_handler
	popal
	iret
