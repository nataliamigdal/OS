#include "systemCalls.h"


//array of file operations tables for each kind of file
funct_str rtc_funct= {rtc_open, rtc_close,rtc_read,rtc_write};
funct_str stdin_funct= {open_terminal, close_terminal ,read_terminal,filler};
funct_str stdout_funct= {open_terminal, close_terminal,filler,write_terminal};
funct_str fs_funct= {file_open, file_close,file_read,file_write};
funct_str dir_funct={dir_open, dir_close,dir_read,dir_write};

///Magic number for executable
uint8_t magic_nos[4] = {0x7F, 0x45, 0x4C, 0x46};
//variable to store the bottom of the kernel stack for current process
 pcb_t * cur_proc_pcb = NULL;
 //buffer for arguments
 uint8_t arg[MAX_ARG_SIZE] ={0};
//bitmask that store the loaded task
static uint8_t loaded_tasks = 0;
//masks for the teminal
uint8_t exec_term_mask[3] = {0,0,0};


///////////////////// IMPLEMENTATION OF THE CALLS /////////////////

 /*
 * halt
 *   DESCRIPTION: Terminates a process, returning the specified value to its parent process
 *   INPUTS: status of the terminated process
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure
 *   SIDE EFFECTS: none
 */

int32_t halt(uint8_t status){
    int i;
    //get current process
	pcb_t * child = cur_proc_pcb;

    //close any open fds
    for(i = 0; i < max_fd; i++)
    {
        if(cur_proc_pcb->file_array[i].flag == IN_USE)
        {
            close(i);
        }
    }

    //reinitialize the fd struct
    set_fds();

    //Remove index from bitmask
    loaded_tasks = loaded_tasks & ~(TASK_MASK<<cur_proc_pcb->proc_index);
    //set process to not active
    cur_proc_pcb->task_active =0;


    //if this is the first shell or it is a base shell
	//if(cur_proc_pcb->proc_index == 0) {
    //if (loaded_tasks ==0 || (cur_proc_pcb->base==1)){
    if (cur_proc_pcb->base ==1){
		// make cur_proc_pcb NULL
		//cur_proc_pcb = NULL;
        //set the terminal no to 0
        uint32_t term = get_term_no();
        exec_term_mask[term] = 0;
		// call excecute shell
        //41 = length of string
        write_terminal(1, "Tried exiting first shell, restarting...\n", 41);
		execute((uint8_t*)"shell ");
	}
	// Restore paging - phy_addr
    map_prog(cur_proc_pcb->par_pcb->phy_addr);

    // Take in from new cur_proc_pcb kernel_btm, and load into tss
    tss.esp0 =(uint32_t) cur_proc_pcb->par_pcb->kernel_btm;
    tss.ss0 = KERNEL_DS;
    //change to par pcb
    cur_proc_pcb = cur_proc_pcb->par_pcb;
	// Jump to execute return

  asm volatile(
    "xorl %%eax, %%eax;"
    "movb %%bl, %%al;"
    "movl %0, %%ebp;"
    "movl %1, %%esp;"

    "jmp halting ;"


    :
    : "r"(child->ebp), "r"(child->esp)
    : "%eax", "%ebp", "%esp"
   );

    return -1;
}


/*
 * execute
 *   DESCRIPTION: Loads and executes a new program, handing off the processor to the new program
 *                until it terminates
 *   INPUTS: command - space-separated sequence of words
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure -- command cannot be executed, program does not exist, filename
 *                                  specified not an executable,
 *                 256 -- if program dies by an exception
 *                 0-255 -- program executes halt system call
 *   SIDE EFFECTS: none
 */

int32_t execute (const uint8_t* command){
    //declare variables
    uint8_t file_name[filename_size];
    uint32_t i = 0;
    uint32_t charac = 0;
    //uint32_t argno=0;
    int32_t retval =-1;
    dentry_t exec_dentry;
    uint8_t magic_buf[4];
    uint32_t entry_pt =0;
    pcb_t * proc_pcb;
    uint32_t new_btm;
    //initialise filename var
    for (i=0; i<filename_size; i++){
        file_name[i]= NULL;
    }
    ////*********Would need to initialise arguments too
    for(i = 0; i < MAX_ARG_SIZE; i++)
    {
    	arg[i] = NULL;
    }
    //error checking
    if (command==NULL)
        return -1;
    i=0;
    //get all arguments from the command
    while (command[i]!='\0'){
        //get the file name
        for (i =0; (command[i]!=' '); i++){
            //check if no args
            if (command[i]=='\0')
                break;
            //if file name is too long
            if (i >=filename_size)
                return -1;
            //store in the array
            file_name[i] = command[i];

        }
        //remove leading spaces
        while(command[i]==' ')
            i++;
        while(command[i]!='\0'){
            //store the argument
            for (charac =0; command[i]!='\0'; charac++,i++){
                arg[charac] = command[i];
            }
            arg[charac] = '\0';
        }
    }
    //check if the exec is in the file system
    if (read_dentry_by_name(file_name, &exec_dentry ) == -1){
        printf("not found");
        return -1;
    }
    //get the exec magic numbers from the command(first 4 bytes
    if (read_data(exec_dentry.inode_num, START_OFFSET, magic_buf, 4) ==-1){
        return -1;
    }

    //check if right nos//compare 4 bytes
    if (strncmp((int8_t*)magic_buf, (int8_t*)magic_nos, 4) !=0){
        printf("not executable");
        return -1;
    }

    //printf("mask  is : %x\n",loaded_tasks);
    //find free space in the loaded tasks
    for (i=0; i<6; i++){
        //if you find a free spot
        if ((loaded_tasks&(TASK_MASK<<i)) ==NO_TASK){
            //mark as occupied
            loaded_tasks = loaded_tasks | (TASK_MASK<<i);
            //if first task
            if (cur_proc_pcb == NULL){
                proc_pcb = (pcb_t*)(KERNEL_BTM& PCB_MASK);
                cur_proc_pcb = proc_pcb;
                //populate pcb
                proc_pcb->phy_addr = PROG_PHY_ADDR;
                proc_pcb->kernel_btm = (uint32_t*)KERNEL_BTM;
                proc_pcb->par_pcb = NULL;
                proc_pcb->proc_index =i;
                proc_pcb->base = 1;
                //set the terminal mask
                exec_term_mask[0] = 1;
                //set task as active
                proc_pcb->task_active = 1;
            }
            else{
                //get new stck bottom by going 4bytes  above old pcb
                //use  i to get stack pointer
                new_btm = KERNEL_BTM - i*(_8kb);
                //new_btm = (uint32_t)(cur_proc_pcb-0x4);
                //get new pcb location
                proc_pcb = (pcb_t*)(new_btm&PCB_MASK);
                //store old pcb location and update kernel bottom
                proc_pcb->par_pcb = cur_proc_pcb;
                proc_pcb->kernel_btm = (uint32_t*)new_btm;
                proc_pcb->proc_index = i;
                proc_pcb->phy_addr = PROG_PHY_ADDR + (i * FOURMB);
                proc_pcb->base = 0;
                proc_pcb->task_active =1;
                //if current task is a base
                //if the current terminals index is the current terminal
                //get the terminal number
                uint32_t term = get_term_no();
                if(exec_term_mask[term] == 0){
                    exec_term_mask[term] = 1;
                    proc_pcb->base = 1;
                    //push shell into the queue
                    //enqueue_task(cur_proc_pcb);
                }
                //set parent task to not active
                //cur_proc_pcb->task_active =0;
                //update the cur_proc_pcb variable
                cur_proc_pcb = proc_pcb;

            }
            break;
        }

    }
    //couldn't find a free spot
    if (i>MAX_TASK_INDEX){
        //couldn't start a new process
        //25 = length of string
        write_terminal(1, "Can't run any more tasks\n", 25);
        return -1;
    }



    // //set up the pcb for this process
    // if (cur_proc_pcb ==NULL){  //if this is the first shell/process
    //     //get pcb locatoin by masking (8kb)
    //     proc_pcb = (pcb_t*)(KERNEL_BTM& PCB_MASK);
    //     cur_proc_pcb = proc_pcb;
    //     //populate pcb
    //     proc_pcb->phy_addr = PROG_PHY_ADDR;
    //     proc_pcb->kernel_btm = (uint32_t*)KERNEL_BTM;
    //     proc_pcb->par_pcb = NULL;
    //     proc_pcb->proc_index =0;
    // }
    // else{
    //     //only run 6 processes
    //     if (cur_proc_pcb->proc_index < 5){
    //         //get new stck bottom by going 4bytes  above old pcb
    //         new_btm = (uint32_t)(cur_proc_pcb-0x4);
    //         //get new pcb location
    //         proc_pcb = (pcb_t*)(new_btm&PCB_MASK);
    //         //store old pcb location and update kernel bottom
    //         proc_pcb->par_pcb = cur_proc_pcb;
    //         proc_pcb->kernel_btm = (uint32_t*)new_btm;
    //         proc_pcb->proc_index = cur_proc_pcb->proc_index +1;
    //         proc_pcb->phy_addr = cur_proc_pcb->phy_addr + FOURMB;
    //         //update the cur_proc_pcb variable
    //         cur_proc_pcb = proc_pcb;
    //     }
    //     else{
    //         //couldn't start a new process
    //         write_terminal(1, "Can't open any more shells\n", 27);
    //         return -1;
    //     }
    //
    // }

    //////////////////// LOADER FUNCTIONALITY ///////////////
    //set up new page dir
    map_prog(proc_pcb->phy_addr);
    //copy the executable to memory
    if (read_data(exec_dentry.inode_num, START_OFFSET, (uint8_t*)PROG_LOCATION, FOURMB)==-1){
        return -1;
    }

    //get the entry point from the copied file
    if (read_data(exec_dentry.inode_num, ENTRY_PT_OFF, (uint8_t*)&entry_pt, 4)==-1){
        return -1;
    }

    //set up stdin & out functs for this pcb
     open ((uint8_t*)"stdin");
     open((uint8_t*)"stdout");

     //copy over arguments
    // strcpy((int8_t*)arg, (int8_t*)cur_proc_pcb->arg);
     strcpy((int8_t*)cur_proc_pcb->arg, (int8_t*)arg);
    /////////////////////  context switching ///////////
    //store the kernel ss & stack pointer in the TSSS
    tss.ss0 = KERNEL_DS;
    tss.esp0 = (uint32_t)proc_pcb->kernel_btm;

    //get the page botom for user program
    uint32_t pg_btm = (PAGE_TOP + FOURMB) -0x4;


    //store esp and ebp
    asm volatile(
        "movl %%ebp, %0;"
        "movl %%esp, %1;"

        : "=r"(cur_proc_pcb->ebp), "=r"(cur_proc_pcb->esp) // outputs

    );


    //add task to scheduler queue clear int
    cli();
    //enqueue_task(cur_proc_pcb);
    //ret_user_asm((uint32_t)ss_32,(uint32_t)pg_btm, (uint32_t)tss.eflags, (uint32_t)cs_32,(uint32_t)entry_pt);
    asm volatile(

        "cli;"
        "movw $0x2B, %%ax;" //2B = USER_DS
        "movw %%ax, %%ds;"  //segment selectors
        "movw %%ax, %%es;"
        "movw %%ax, %%fs;"
        "movw %%ax, %%gs;"


        "pushl $0x2B;" //push stack seg sel
        "pushl %1;"    //push esp

        "pushfl;"       //push the flag registers after enabling the IF flag
        "popl %%eax;"
        "orl  $0x200, %%eax;"
        "pushl %%eax;"

        "pushl $0x23;" // 23 = USER_CS
        "pushl %2;"    //push eip
        "iret;"

        //get retval from halt
        "halting: ;"
        "movl %%eax, %0;"

        : "=r"(retval) //outputs
        : "r"(pg_btm),"r"(entry_pt)  //inputs
        :"%eax"
    );

    return retval;
}


/*
 * open
 *   DESCRIPTION: Provides access to the file system. Call finds the directory entry corresponding to the
 *                named file, allocates an unused file descriptor, and sets data to handle given type of file.
 *   INPUTS: filename - name of file to open
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure -- filename does not exist or no descriptors are free
 *                  fd on success
 *   SIDE EFFECTS: none
 */
int32_t open(const uint8_t * filename){
    //get the pcb for this process
    pcb_t * this_pcb = cur_proc_pcb;
    dentry_t identry;
    uint8_t i;
    sti();
    //check if it is stdin or std out
    if (strncmp((int8_t*)filename, (int8_t*)"stdin", 5)==0){
        if (strlen((int8_t*)filename)!=5){
            return -1;
        }
        //if stdin is not set up, set it up
        if(this_pcb->file_array[stdin_fd].flag ==IN_USE){
            return -1;
        }
        //mark file as in use
        strcpy((int8_t*)this_pcb->file_array[stdin_fd].filename, (int8_t*)"stdin");
        this_pcb->file_array[stdin_fd].flag = IN_USE;
        this_pcb->file_array[stdin_fd].f_pos =0;
        //give it appropriate jump table
        this_pcb->file_array[stdin_fd].file_type= empty_file;
        this_pcb->file_array[stdin_fd].f_ops = stdin_funct;
        //call the open function
        this_pcb->file_array[stdin_fd].f_ops.open(filename);
        return 0;

    }
    //check if we should open stdout
    if (strncmp((int8_t*)filename, (int8_t*)"stdout", 6)==0){
        //if something else at the back
        if (strlen((int8_t*)filename)!=6){
            return -1;
        }
        //if not set up, set up
        if(this_pcb->file_array[stdout_fd].flag ==IN_USE){
            return -1;
        }
        strcpy((int8_t*)this_pcb->file_array[stdout_fd].filename, (int8_t*)"stdout");
        //mark file as in use
        this_pcb->file_array[stdout_fd].flag = IN_USE;
        this_pcb->file_array[stdout_fd].f_pos =0;
            this_pcb->file_array[stdout_fd].file_type = empty_file;
        //give it appropriate jump table
        this_pcb->file_array[stdout_fd].f_ops = stdout_funct;
        this_pcb->file_array[stdout_fd].f_ops.open(filename);
        return 0;
    }

    //for normal files
    // find the directory entry corresponding to the file name
     if (read_dentry_by_name(filename, &identry) != 0){
         return -1;
     }

    //allocate unused file descriptor
    for (i= min_new_fd ; i< max_new_fd; i++){
        //find the first fd not in use
        if (this_pcb->file_array[i].flag==NOT_IN_USE){
            //set it to in use
            this_pcb->file_array[i].flag = IN_USE;
            //copy over filetype & name
            strcpy((int8_t*)this_pcb->file_array[i].filename, (int8_t*)identry.filename);
            this_pcb->file_array[i].file_type = identry.file_type;
            this_pcb->file_array[i].inode_num = identry.inode_num;
            this_pcb->file_array[i].f_pos =0;
            //printf("opening file: %d",this_pcb->file_array[i].file_type);

            //set up file operators for the given type
            switch (identry.file_type){
                case (rtc_file):
                    //give it the right fileoprations
                    this_pcb->file_array[i].f_ops = rtc_funct;
                    //open rtc file
                    this_pcb->file_array[i].f_ops.open(filename);
                    return i;

                case (dir_file):
                    this_pcb->file_array[i].f_ops = dir_funct;
                    //oper dir file
                    this_pcb->file_array[i].f_ops.open(filename);
                    return i;

                case(reg_file):
                    this_pcb->file_array[i].f_ops = fs_funct;
                    //set up regular files
                    this_pcb->file_array[i].f_ops.open(filename);
                    return i;

                default:
                    //invalid file type
                    return -1;
            }

        }
    }
    //couldnt find any open fd
    return -1;
}

//call the close functions - should not be able to close stdin or out
/*
 * close
 *   DESCRIPTION: Closes the specified file descriptor
 *   INPUTS: fd - file decriptor to close
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- tried closing invalid fd
 *                  ret -- return value from close call (always returns 0)
 *   SIDE EFFECTS: none
 */
int32_t close(int32_t fd){
    //get the right pcb
    sti();
    pcb_t * this_pcb = cur_proc_pcb;
    int32_t ret =-1;
    //printf("close call\n");

    //tried to close an invalid fd
    if (fd < min_new_fd || fd > max_fd){
        return -1;
    }

    //tried to close unopened fd
    if(this_pcb->file_array[fd].flag == NOT_IN_USE)
    {
        return -1;
    }

    //call close function for that fd
    ret = this_pcb->file_array[fd].f_ops.close(fd);

    if (ret ==0){
        //clear the file descriptor
        memset(this_pcb->file_array[fd].filename, 0, filename_size);
        this_pcb->file_array[fd].inode_num =0;
        this_pcb->file_array[fd].file_type =empty_file;
        this_pcb->file_array[fd].flag= NOT_IN_USE;
    }

    return ret;
}

/*
 * read
 *   DESCRIPTION: Reads data from the keyboard, a file, device (RTC), or directory
 *   INPUTS: fd -- file descriptor
 *           buf -- holds the buffer to read into
 *           nbytes -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure
 *                  bytesRead -- number of bytes read (success)
 *   SIDE EFFECTS: none
 */

// FIX FOR CHECKPOINT 4
int32_t read (int32_t fd, void* buf, int32_t nbytes){

    //tried to read and invalid fd
    if (fd < min_fd || fd > max_fd || (buf ==NULL) || (cur_proc_pcb->file_array[fd].flag ==NOT_IN_USE)){
        return -1;
    }
    sti();
    //return bytes in read call
    return cur_proc_pcb->file_array[fd].f_ops.read(fd, buf, nbytes);
}


/*
 * write
 *   DESCRIPTION: Writes data to the terminal or to a device (RTC)
 *   INPUTS: fd -- file descriptor
 *           buf -- holds the buffer to write into
 *           nbytes -- number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: -1 on failure
 *                  or number of bytes written, on success
 *   SIDE EFFECTS: none
 */

int32_t write(int32_t fd, const void* buf, int32_t nbytes){

    //tried to write and invalid fd
    if (fd < min_fd || fd > max_fd || (buf ==NULL) || (cur_proc_pcb->file_array[fd].flag ==NOT_IN_USE)){
        return -1;
    }
    sti();
    //call write function for that fd
    return cur_proc_pcb->file_array[fd].f_ops.write(fd, buf, nbytes);

}


/*
 * getargs
 *   DESCRIPTION: Reads the program's command line arguments into a user-level buffer
 *   INPUTS: buf -- buffer to read into
 *           nbytes -- number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure
 *                  or number of bytes written, on success
 *   SIDE EFFECTS: none
 */
int32_t getargs(uint8_t* buf, int32_t nbytes){
    int i;
    uint32_t len =strlen((int8_t*)cur_proc_pcb->arg);


    //check if it is a valid buffer
    if (buf==NULL || (nbytes<= len)){
        //printf("bad arg %d, vs %d ", nbytes, strlen(cur_proc_pcb->arg) );
        return -1;
    }

    for(i = 0; i < nbytes; i++)
    {
        buf[i] = '\0';
    }

    if(cur_proc_pcb->arg[0] == NULL)
    {
    	return -1;
    }
    //copy over the argument
    strncpy((int8_t*) buf, (int8_t*) cur_proc_pcb->arg, len);
    return 0;
}


/*
 * vidmap
 *   DESCRIPTION: Maps the text-mode video memory into user space at a pre-set virtual address
 *   INPUTS: screen_start -- memory location to start writing to
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure
 *                  or the virtual address on success
 *   SIDE EFFECTS: none
 */
int32_t vidmap(uint8_t** screen_start){
    //check if the video mem would be able to fit
    // progvirtaddr + 4m < screen start < 4gb -4kib

    //put it at the screen start in the page table & set permissions

    //make the pointer that screen start is pointing to, point to a location in
    //video mory
    //*screen_start = v irtual address of pte pointing to video memory

    //error checkint
    if (screen_start == NULL)
        return -1;
    if ( ((uint32_t)screen_start<PAGE_TOP) || ((uint32_t)screen_start > PAGE_TOP+FOURMB) )
        return -1;


    //create a PDE that points to a  page table
    //this page table would map to vide memory
    //return the page table entry to user
    *screen_start = (uint8_t*)map_vid();
    //return 0 on success
    return (int32_t)*screen_start;
}

/*
 * set_handler
 *   DESCRIPTION: changes the default action taken when a signal is received
 *   INPUTS: signum -- specifies which signal's handler to change
 *           handler_addresses -- points to a user-level function to be run when that signal
 *                                is received
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure
 *                  0 -- handler was successful
 *   SIDE EFFECTS: none
 */
int32_t set_handler(int32_t signum, void* handler_addresses){

    return -1;
}

/*
 * sigreturn
 *   DESCRIPTION: Copies the hardware context that was on the user-level stack back onto the
 *                processor
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure
 *   SIDE EFFECTS: none
 */
int32_t sigreturn(void){
    return -1;
}




//filler function to make sure the terminal functs can never call the wrong thing
/*
 * filler
 *   DESCRIPTION: Filler function to make sure the terminal functions never call the wrong function
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: -1 -- failure, it called the wrong function
 *   SIDE EFFECTS: none
 */
int32_t filler (){
    //printf ("tried to call a wrong function\n");
    return -1;

}


/*
 * get_pcb
 *   DESCRIPTION: Helper function that gets the current pcb pointer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: cur_proc_pcb -- current pcb pointer
 *   SIDE EFFECTS: none
 */
pcb_t * get_pcb(){
    return cur_proc_pcb;
}


/*
 * set_fds
 *   DESCRIPTION: Sets all the file descriptors to not present
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void set_fds(){
    pcb_t * this_pcb = cur_proc_pcb;
    uint8_t i=0;
    for(i = min_fd; i <= max_fd; i++){
        //clear the file descriptor
        memset(this_pcb->file_array[i].filename, NULL, filename_size);
        this_pcb->file_array[i].inode_num =0;
        this_pcb->file_array[i].file_type =empty_file;
        this_pcb->file_array[i].flag= NOT_IN_USE;
    }
}

//function that sets the current pcb
void set_pcb (pcb_t* new_task){
    cur_proc_pcb = new_task;
}
