#include "rtc.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"

volatile int read_flag=0;
int rtc_on=0;


/*
 * rtc_handler
 *   DESCRIPTION: Register C is read after an IRQ8 so that an interrupt can happen
 *				  again.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Can either print a statement that indicates an RTC interrupt occured
 *				   or use test_interrupts() or print 1 to the screen.
 */

 /* ref: http://wiki.osdev.org/RTC */
void rtc_handler() {
	//printf("RTC interrupt\n");
	//set read rtc flag
	if (!read_flag)
		read_flag =1;
	//test_interrupts();
	cli();
	outb(REGC, RTC_PORT);	// select register C which contains a bitmask
						//telling us which interrupt happened
	inb(RTC_DATA);		// just throw away contents
	send_eoi(RTC_IRQ); //send end of interrupt
	sti();
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

	//set frequency to 2hz
	outb (REGA_NMI, RTC_PORT);
	prev = inb(RTC_DATA);
	//set to reg A & set new rate
	outb (REGA_NMI, RTC_PORT);
	outb ( (prev | 0x0F) , RTC_DATA);

	enable_irq(RTC_IRQ_SLAVE);	//enable interrupts on slave
	enable_irq(RTC_IRQ);  //enable interrupts on master
	//sti();*/
	//set on flag for open syscall
	rtc_on =1;
}


/*
 * rtc_open
 *   DESCRIPTION: iniitalises the rtc if not initialised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on succes
 *   SIDE EFFECTS: sets the interrupt rate of the rtc to 2hz
 */

int rtc_open(const uint8_t * filename){
	//initialise if rtc not initialised already
	if (!rtc_on){
		rtc_init();
		rtc_on =1;
	}
	//set up int to 2hz
	//disable interrupts
	// cli();
	// //select reg A & get old rate
	// outb (REGA_NMI, RTC_PORT);
	// char prev = inb(RTC_DATA);
	// //set to reg A & set new rate
	// outb (REGA_NMI, RTC_PORT);
	// outb ( (prev & 0xF0) |freq0 , RTC_DATA);
	// //enable ints
	// sti();
	return 0;
}

/*
 * rtc_close
 *   DESCRIPTION: closes the rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: none
 */

int rtc_close(int32_t fd){
	return 0 ;
}
//return 0 only after an interrupt has occured
/*
 * rtc_read
 *   DESCRIPTION: Waits for an interrupt then returns
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: none
 */

int rtc_read(int32_t fd, void* buf, int32_t nbytes){
	read_flag =0;
	while (!read_flag);
	return 0;
}

//accept only a 4 byte integer (interrupt rate in Hz)
//set the rate of periodic interrupts
//return -1 on falure
//max freq of 1024
//check if parameter is a power of 2
/*
 * rtc_write
 *   DESCRIPTION: Changes registerA on the RTC
 *   INPUTS: ifreq : the requested new frequency of the rtc
 *			 numBytes: the number of bytes of ifreq
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *				   -1 on falure
 *   SIDE EFFECTS: Changes the interrupt rate of the rtc
 */

int rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	unsigned char rate=0;
	//check the nmber of bytes of frequency
	//printf(" writing to rtc : %s\n",buf );
	if (nbytes !=4)
		return -1;
	//get value from the buffer
	uint32_t* ptr = (uint32_t*) buf;
	//find the rate needed for new freq
	switch (*ptr){	//cross-check
		case freq0: rate = 0x0F;
		 			break;
		case freq1:rate = 0x0E;
		 			break;
		case freq2:rate = 0x0D;
		 			break;
		case freq3:rate = 0x0C;
		 			break;
		case freq4: rate = 0x0B;
					break;
		case freq5:rate = 0x0A;
					break;
		case freq6:rate = 0x09;
					break;
		case freq7:rate = 0x08;
					break;
		case freq8:rate = 0x07;
					break;
		case freq9:rate = 0x06;
					break;
		//if none of the above, error
		default : return -1;
	}
	//change frequency of rtc
	//disable interrupts
	cli();
	//select reg A & get old rate
	outb (REGA_NMI, RTC_PORT);
	char prev = inb(RTC_DATA);
	//set to reg A & set new rate
	outb (REGA_NMI, RTC_PORT);
	outb ( (prev & 0xF0) |rate , RTC_DATA);
	//enable ints
	sti();
	return 0;
}
