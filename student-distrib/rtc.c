#include "rtc.h"
#include "lib.h"
#include "i8259.h"

/*
 * rtc_handler
 *   DESCRIPTION: Register C is read after an IRQ8 so that an interrupt can happen
 *				  again. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Can either print a statement that indicates an RTC interrupt occured
 *				   or use test_interrupts().
 */ 

 /* ref: http://wiki.osdev.org/RTC */  
void rtc_handler() {
	//printf("RTC interrupt");
	//test_interrupts();
	outb(REGC, RTC_PORT);	// select register C which contains a bitmask
						//telling us which interrupt happened
	inb(RTC_DATA);		// just throw away contents
	send_eoi(RTC_IRQ); //send end of interrupt

}


/*
 * rtc_init
 *   DESCRIPTION: Initialized the RTC.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Turns on the periodic interrupt, IRQ 8. (and IRQ 2 for the slave)
 */   

/* ref: http://wiki.osdev.org/RTC */

void rtc_init() {
	//cli();
	disable_irq(RTC_IRQ);	//disable interrupts
	
	//default 1024 Hz rate
	outb(REGB_NMI, RTC_PORT);				// select register B, and disable NMI
	char prev=inb(RTC_DATA);			// read the current value of reg B 
	outb(REGB_NMI, RTC_PORT);				// set the index again
	outb(prev | MASK_B, RTC_DATA);		// turns on bit 6 of reg B

	enable_irq(RTC_IRQ_SLAVE);	//enable interrupts on slave
	enable_irq(RTC_IRQ);  //enable interrupts on master
	//sti();*/
}
