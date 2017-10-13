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
#define MASK_B 0x40


/*Register C is read after an IRQ8 so that an interrupt can happen again*/
/*also used for testing */
extern void rtc_handler();

/*initializes the RTC */
extern void rtc_init();

#endif
