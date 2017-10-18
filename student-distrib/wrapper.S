.text

/* all global so that idt.c can use them */
.globl rtc_handler_asm, keyboard_handler_asm
.globl pit_handler_asm
.globl divide_by_zero_asm, debug_exception_asm, NMI_intr_asm, breakpoint_intr_asm
.globl overflow_intr_asm, BR_intr_asm, invalid_opcode_asm
.globl device_NA_asm, double_fault_asm, segement_overrun_asm, invalid_TSS_asm
.globl segment_not_present_asm, stack_segment_fault_asm ,general_protection_asm, page_fault_asm, math_fault_asm
.globl alignment_check_asm, machine_check_asm, floatingpoint_exception_asm


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
	cli
	call rtc_handler
	sti
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
	cli
	call keyboard_handler
	sti
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
	cli
	call divide_by_zero
	sti
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
	cli
	call debug_exception
	sti
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
	cli
	call NMI_intr
	sti
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
	cli
	call breakpoint_intr
	sti
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
	cli
	call overflow_intr
	sti
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
	cli
	call BR_intr
	sti
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
	cli
	call invalid_opcode
	sti
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
	cli
	call device_NA
	sti
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
	cli
	call double_fault
	sti
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
	cli
	call segement_overrun
	sti
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
	cli
	call invalid_TSS
	sti
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
	cli
	call segment_not_present
	sti
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
	cli
	call stack_segment_fault
	sti
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
	cli
	call general_protection
	sti
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
	cli
	call page_fault
	sti
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
	cli
	call math_fault
	sti
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
	cli
	call alignment_check
	sti
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
	cli
	call machine_check
	sti
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
	cli
	call floatingpoint_exception
	sti
	popal
	iret

    /*
     * pit_handler_asm
     *   DESCRIPTION: Calls interrupt handlers without modifyingn the stack.
     *   INPUTS: none
     *   OUTPUTS: none
     *   RETURN VALUE: none
     *   SIDE EFFECTS: calls the handler, which prints out the interrupt on the screen
     */
    pit_handler_asm:
    	pushal
    	cli
    	call pit_handler
    	sti
    	popal
    	iret
