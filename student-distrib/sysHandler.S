# sysHandler.S - set up the stack & call right system call
.text
.globl sysHandler


# define ASM 1
/*
 * sysHandler
 *   DESCRIPTION: Handler to handle system call and call system call functions
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */

sysHandler:
# save all the registers on the stack in proper orderz
    pushfl
    pushw %fs
    pushw %es
    pushw %ds
    # pushing other registers
    pushl %ebp
    pushl %edi
    pushl %esi
    # arguments from right to left
    pushl %edx
    pushl %ecx
    pushl %ebx

#  check if valid
    cmpl $1, %eax
    jl invalid_op    # if less than 1

    cmpl $10, %eax
    jg invalid_op    # if greater than 10


# call right function from jump table
    sti
    call *func_table(,%eax,4)


# put return value in EAX
    /*movl 4(%ebp), %eax*/
    jmp pop_args

invalid_op:

    # put -1 in eax
    xorl %eax, %eax
    decl %eax

pop_args:
    # pop arguments
    popl %ebx
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    popw %ds
    popw %es
    popw %fs
    popfl
    iret


# jumptable for c function system calls
func_table:
    .long 0x0
    .long halt
    .long execute
    .long read
    .long write
    .long open
    .long close
    .long getargs
    .long vidmap
    .long set_handler
    .long sigreturn
