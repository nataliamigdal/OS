#ifndef SCHEDULING_H
#define SCHEDULING_H

#include "systemCalls.h"
#include "pages.h"


#define MAX_TASKS 3
#define TASK_DONE 1
#define TASK_IP 0
#define TIMER_IRQ 0
#define PIT_DIVIDER 0xA321  //freq of 35ms(28hs)
#define LO_DIV 0x21
#define HI_DIV 0xA3
#define PIT_SETTINGS 0x30//0b00110000
#define PIT_CMD 0x43
#define PIT_DATA 0x40
#define time_before_switch 30

//functions for scheduling
//function called on interrupt
void switch_task(uint8_t done_flag);
//function to add a task to the scheduler
void enqueue_task(pcb_t * task);
//function to remove task from scheduler
pcb_t* dequeue_task();

//PIT functions
//handler for the PIT, as stated. Used for scheduling
extern void pit_handler();
//initiliazes the PIT
extern void pit_init();



#endif
