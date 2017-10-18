#ifndef SYSTEMCALLS_H
#define SYSTEMCALLS_H

#include "rtc.h"
#include "idt.h"
#include "terminal.h"
#include "filesystem.h"
#include "types.h"
#include "pages.h"
#include "asm2.h"

//numbers for each system call copied from syscalls
#define SYS_HALT    1
#define SYS_EXECUTE 2
#define SYS_READ    3
#define SYS_WRITE   4
#define SYS_OPEN    5
#define SYS_CLOSE   6
#define SYS_GETARGS 7
#define SYS_VIDMAP  8
#define SYS_SET_HANDLER  9
#define SYS_SIGRETURN  10

//to represent status of fd
#define IN_USE 1
#define NOT_IN_USE 0

#define open_index 0
#define close_index 1
#define read_index 2
#define write_index 3

//file types
#define rtc_file 0
#define dir_file 1
#define reg_file 2
#define empty_file 3

#define START_OFFSET 0
//8MB
#define PROG_PHY_ADDR 0x800000
#define PAGE_TOP 0x08000000
#define PROG_LOCATION 0x08048000
#define MAX_ARG_SIZE 70  //somebody confirm this

//need bytes 24-27
#define ENTRY_PT_OFF 24
//points to the last memory location of the kernel in mem
#define KERNEL_BTM 0x7ffffc
//mask is 0b 1111 1111 1110 0000 0000 0000
#define PCB_MASK 0xffe000
#define NEW_KERNEL_MASK 0x0000000

//range for file descriptors that are not stdin or stdout
#define min_new_fd 2
#define max_new_fd 8

//valid file descriptor range
#define min_fd 0
#define max_fd 7

//stdin and stout file descriptors
#define stdin_fd 0
#define stdout_fd 1

//
#define TASK_MASK 0x01
#define NO_TASK 0x00
#define MAX_TASK_INDEX 5

//functions
//function that should never be called, helps with debugging
int32_t filler ();

//halts a process
int32_t halt(uint8_t status);

//executes and loads a process
int32_t execute (const uint8_t* command);

//reads data from a driver
int32_t read (int32_t fd, void* buf, int32_t nbytes);

//writes data to driver
int32_t write(int32_t fd, const void* buf, int32_t nbytes);

//opens file by filling in info about the file
int32_t open(const uint8_t * filename);

//closes the given file descriptor
int32_t close(int32_t fd);

//reads the arguments of a command line
int32_t getargs(uint8_t* buf, int32_t nbytes);

//maps text-mode video memory
int32_t vidmap(uint8_t** screen_start);

//changes default action when signal is received
int32_t set_handler(int32_t signum, void* handler_addresses);

//Copies the hardware context from user-level stack ->processor
int32_t sigreturn(void);

//set fds to not present
void set_fds();

//system call handler
extern void sysHandler();


//struct to store function pointers
typedef struct{
    int32_t (*open)(const uint8_t * filename);
    int32_t (*close)(int32_t fd);
    int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
} funct_str;



// File Descriptor struct
typedef struct{
    //filename & type
    int8_t filename[32];
    uint32_t file_type;
    //inode number
    uint32_t inode_num;
    //array of file ops
    funct_str f_ops;
    //File position to keep track of where the user is at
    uint32_t f_pos;
    //file in use
    uint32_t flag;
} fd_t;

// PCB struct
// Keeps track of the information of the current task
typedef struct pcb_t pcb_t;
struct pcb_t{
    //array of 8 file descriptors
    fd_t file_array[8];
    //physical address the user prog is loaded at
    uint32_t phy_addr;
	//kernel stack pointers
    uint32_t* kernel_btm;
    //parent's pcb pointer
    pcb_t* par_pcb;
    //current kernel stack pointers
    uint32_t  ebp;
    uint32_t esp;
    //stack pointers for scheduling
    uint32_t sched_ebp;
    uint32_t sched_esp;
    //process index
    uint8_t proc_index; //o-6 for 6 processes
    //arguments fot the process
    uint8_t arg[MAX_ARG_SIZE];
    //if it is a base shell
    uint8_t base;
    //active task bit
    uint8_t task_active;
};

//helper function that gets a pointer to the current pcb
pcb_t * get_pcb();
void set_pcb (pcb_t* new_task);

#include "scheduling.h"
#endif
