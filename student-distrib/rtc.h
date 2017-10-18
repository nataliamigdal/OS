#ifndef _RTC_H
#define _RTC_H

#include "x86_desc.h"
#include "lib.h"

#define REGC 0x0C
#define RTC_PORT 0x70
#define RTC_DATA 0x71
#define RTC_IRQ 8
#define RTC_IRQ_SLAVE 2
#define REGB_NMI 0x8B
#define REGA_NMI 0x8A
#define MASK_B 0x40
#define screen_width 80
//allowed frequencies
#define freq0 2
#define freq1 4
#define freq2 8
#define freq3 16
#define freq4 32
#define freq5 64
#define freq6 128
#define freq7 256
#define freq8 512
#define freq9 1024


/*Register C is read after an IRQ8 so that an interrupt can happen again*/
/*also used for testing */
extern void rtc_handler();

/*initializes the RTC */
extern void rtc_init();
//open, close, read & write functions fot the rtc
extern int rtc_open(const uint8_t * filename);
extern int rtc_close(int32_t fd);
extern int rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
