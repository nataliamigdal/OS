#include "scheduling.h"

//array to store pcb pointers
uint32_t task_queue[MAX_TASKS]={0};
//to keep trak of front index
uint8_t rear =0;
uint8_t front =0;
uint8_t size =0;
uint32_t count =0;
uint8_t tc = 0;

///////////// functions for scheduling ////////////
/*
 * switch_task
 *   DESCRIPTION: Called on interrupt to switch current task
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void switch_task(uint8_t done_flag){
    //variables
    uint32_t terminal_no;
    //uint32_t pcb1, pcb2, pcb3;
    //uint32_t esp, ebp;
    pcb_t * next_task, *root_task;
    //clear interrupts
    //cli();
    /////////////////////////// storing current task ///////////
    //get current task
    pcb_t * this_task = get_pcb();
    //store valuable info for current task
    // asm volatile(
    //     "movl %%esp, %0;"
    //     "movl %%ebp, %1;"
    //
    //     :"=r"(this_task->sched_esp), "=r"(this_task->sched_ebp)
    // );
    //get current base shell terminal for this process
    root_task = this_task;
    while (root_task->base != 1 ){
        root_task = root_task->par_pcb;
    }
    terminal_no = root_task->proc_index;
    //save current video memory
    save_video(terminal_no);
    //push the task on the queue
    enqueue_task(this_task);


    ///////////////////////// loading next task///// /////////////
    //dequeue next pcb
    next_task = dequeue_task();
    //set it as the current task
    set_pcb (next_task);
    //set up the tasks paging
    map_prog(next_task->phy_addr);

    //turn on video memory for this shell
    //get process index of base shell
    root_task = next_task;
    while (root_task->base != 1 ){
        root_task = root_task->par_pcb;
    }
    terminal_no = root_task->proc_index;
    if(terminal_no == get_term_no()) {
        //load it's memory into video memory
        //load_video(terminal_no);
        //connect it to video memory
        route_video(terminal_no);
    }
    else {
        //just route to the buffer
        route_buffer(terminal_no);
    }

    //chenge to the tasks kernel stack
    tss.esp0 = (uint32_t) next_task->kernel_btm;

    //store pointers for prev & Restore kernel stack pointers for next
    //store old one
    asm volatile(
        "movl %%esp, %0;"
        "movl %%ebp, %1;"

        :"=r"(this_task->sched_esp), "=r"(this_task->sched_ebp)
    );
    //store load prev ones
    asm volatile(
        "movl %0, %%ebp;"
        "movl %1, %%esp;"

        :
        :"r"(next_task->sched_ebp), "r"(next_task->sched_esp)
        :"%ebp", "%esp"
    );
    //set lo byte
    outb(LO_DIV, PIT_DATA);
    //hi byyte
    outb(HI_DIV, PIT_DATA);
    //send eoi
    send_eoi(TIMER_IRQ);


}



/*
 * enqueue_taske
 *   DESCRIPTION: Adds a task to the scheduler.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void enqueue_task(pcb_t * task){
    if(task==NULL)
        return;
    //check if it is full
    if (size == MAX_TASKS)
        return;
    //put task at rear then update rear
    task_queue[rear] = (uint32_t)task;
    rear++;
    size++;
    //if last index
    if (rear>=MAX_TASKS)
        rear =0;

}



/*
 * dequeue_task
 *   DESCRIPTION: Removes task from scheduler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: task - pcb pointer of task
 *   SIDE EFFECTS: none
 */
pcb_t* dequeue_task(){
    uint32_t task;
    //if emptry, return null
    if (size == 0)
        return NULL;
    //get the element at the front and increment front
    task = task_queue[front];
    task_queue[front] = 0;
    front++;
    size--;
    //if it is the last index
    if (front>=MAX_TASKS)
        front =0;

    return (pcb_t*)task;
}



////////////// PIT STUFF ///////////////

/*
 * pit_init
 *   DESCRIPTION: Initilizing the PIT
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_init(){
    //initialise the pit to required frequency
    cli();
    outb (PIT_SETTINGS, PIT_CMD);   //channel 0, lobyte/hibyte, int on terminal cnt
    //set low byte
    outb(LO_DIV, PIT_DATA);
    //hi byte
    outb(HI_DIV, PIT_DATA);
    //enable irq0
    enable_irq(TIMER_IRQ);
    sti();
}

/*
 * pit_handler
 *   DESCRIPTION: Handles pit interrupt and checks to see if we need to create a new shell,
 *                handles scheduling
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void pit_handler(){
     count++;
     if (count == time_before_switch){
         count =0;
         write_terminal(1, "a", 1);
     }
    //if we need to create a new shell
    if(tc < MAX_TERMINALS) {
        get_pcb()->base = 1;
        save_video(tc);
        tc++;
        if(tc < MAX_TERMINALS) {
            //store the esp & ebp upon execute
            asm volatile(
                "movl %%esp, %0;"
                "movl %%ebp, %1;"

                :"=r"(get_pcb()->sched_esp), "=r"(get_pcb()->sched_ebp)
            );
            enqueue_task(get_pcb());
            //set lo byte
            outb(LO_DIV, PIT_DATA);
            //hi byyte
            outb(HI_DIV, PIT_DATA);
            send_eoi(TIMER_IRQ);
            //save it's video mmeory
            //save_video(tc);
            //switch_xy(tc-1, tc);
            route_buffer(tc);
            execute((uint8_t*)"shell");
        }
        send_eoi(TIMER_IRQ);
    }
    // //if there is at least one task, run scheduler
    //switch_task(TASK_IP);

    //set reload value again
    //set lo byte
    outb(LO_DIV, PIT_DATA);
    //hi byyte
    outb(HI_DIV, PIT_DATA);

    //send eoi
    send_eoi(TIMER_IRQ);
    sti();
}
