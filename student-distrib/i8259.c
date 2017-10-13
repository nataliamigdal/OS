/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/*
 * i8259_init
 *   DESCRIPTION: Initializes the 8259 PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Uses control words to give information to the PICs
 *				   ICW1 - makes PIC wait for 3 more commands
 *				   ICW2 - vector offset
 *				   ICW3 - tells how it's wired to master/slaves
 *				   ICW4 - gives information about EOI
 */   
void
i8259_init(void)
{

	outb(MASK, MASTER_8259_PORT + 1); //mask all of 8259A-1 (master)
	outb(MASK, SLAVE_8259_PORT + 1); //mask all of 8259A-2 (slave)

	outb(ICW1, MASTER_8259_PORT);		//ICW1: select 8259A-1 init
	outb(ICW2_MASTER, MASTER_8259_PORT + 1); //ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27
	outb(ICW3_MASTER, MASTER_8259_PORT + 1); //8259A-1 (master) has slave on IR2
	outb(ICW4, MASTER_8259_PORT + 1); //master expects normal EOI

	outb(ICW1, SLAVE_8259_PORT);	//ICW1: select 8259A-2 init
	outb(ICW2_SLAVE, SLAVE_8259_PORT + 1); //ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f
	outb(ICW3_SLAVE, SLAVE_8259_PORT + 1); //8259A-2 is a slave on master's IR2
	outb(ICW4, SLAVE_8259_PORT + 1); //slave expects normal EOI

	outb(MASK, MASTER_8259_PORT + 1); //restore master IRQ mask
	outb(MASK, SLAVE_8259_PORT + 1);  //restore slave IRQ mask

}


/*
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ.
 *   INPUTS: uint32_t irq_num - IRQ line number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */  

 /* ref: http://wiki.osdev.org/8259_PIC */
void
enable_irq(uint32_t irq_num)
{

	//if less than 8 --> master
	if(irq_num < 8) {
		// Set the master mask bit to 0 by and-ing with ~1 bitshifted to the right irq line
		master_mask = inb(MASTER_8259_PORT + 1) & ~(1 << irq_num);
		// Sending to master data
		outb(master_mask, MASTER_8259_PORT + 1);

	//else --> slave
	} else {
		// Set the slave mask bit to 0 by and-ing with ~1 bitshifted to the right slave irq line
		slave_mask = inb(SLAVE_8259_PORT + 1) & ~(1 << (irq_num - 8));
		// Sending to slave data
		outb(slave_mask, SLAVE_8259_PORT + 1);
	}

}


/*
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ.
 *   INPUTS: uint32_t irq_num - IRQ line number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */  

/* ref: http://wiki.osdev.org/8259_PIC */
void
disable_irq(uint32_t irq_num)
{
	//if less than 8 --> master
	if(irq_num < 8) {
		// Set the master mask bit to 1 by or-ing with 1 bitshifted to the right irq line
		master_mask = inb(MASTER_8259_PORT + 1) | (1 << irq_num);
		// Sending to master data
		outb(master_mask, MASTER_8259_PORT + 1);
	//else --> slave
	} else {
		// Set the slave mask bit to 1 by or-ing with 1 bitshifted to the right slave irq line
		slave_mask = inb(SLAVE_8259_PORT + 1) | (1 << (irq_num - 8));
		// Sending to slave data
		outb(slave_mask, SLAVE_8259_PORT + 1);
	}

}

/*
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *   INPUTS: uint32_t irq_num - IRQ line number
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sends to either both master and slave, or just to master
 */  
void
send_eoi(uint32_t irq_num)
{
	//IRQ came from slave PIC, send to both PIC chips
	if(irq_num >= 8) {
		outb(EOI | (irq_num - 8),SLAVE_8259_PORT);
		outb(EOI | ICW3_SLAVE,MASTER_8259_PORT);
	} else {
		//IRQ came from master, so only send to master
		outb(EOI | irq_num,MASTER_8259_PORT); 
	}
}

