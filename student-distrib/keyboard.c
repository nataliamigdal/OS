#include "keyboard.h"
#include "terminal.h"
#include "lib.h"
#include "systemCalls.h"


uint8_t shift_status = 0;
uint8_t capslock_status = 0;
uint8_t ctrl_status	= 0;
uint8_t alt_status = 0;
uint8_t text_buffer[3][TEXT_BUFFER_SIZE];
uint8_t	text_buffer_pos[3] = {0,0,0};
uint8_t initx = 0;
uint8_t enter_indicator = 1;
uint8_t term_no = 0;
uint8_t term_mask[3] = {1,0,0};
uint32_t task_array[3];
uint32_t tss_arr[3];
uint8_t svd_xyz[3][2];

// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
// * specifies keycodes to be done later
static unsigned char scancode_to_key[4][MAX_INDEX] =

{
	{
		    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,	// 00 - of
			'q', 'w', 'e', 'r', 't', 'y', 'u', 'i','o', 'p', '[', ']', '\n', 0, 'a', 's',	// 10 - 1f
			'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',	// 20 - 2f
			'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0	, 1, 14, 15								// 30 - 3a ends on caps lock
	},

		// When Shift is Pressed
	{
			0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,		// 00 - 0f
	 		'Q', 'W', 'E', 'R', 'T', 'Y', 'U','I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',		// 10 - 1f
	 		'D', 'F', 'G', 'H',  'J','K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V', 		// 20 - 2f
	 		'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0	, 1, 14, 15								// 30 - 3a ends on caps lock
	},
		// Caps lock, differs from shift. Letters are capitlized but don't get special characters
	{
			0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', 0, 0,
	 		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0, 'A', 'S',
	 		'D','F', 'G', 'H','J', 'K', 'L' , ';', '\'', '`', 0, '\\', 'Z', 'X', 'C', 'V',
	 		'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' ', 0, 1, 14, 15
	},
		//Caps lock and shift,  letters become lower case and get special characters
	{
			0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0, 0,		// 00 - 0f
	 		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', 0, 'a', 's',		// 10 - 1f
	 		'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', 0, '\\', 'z', 'x', 'c', 'v', 		// 20 - 2f
	 		'b', 'n', 'm', '<', '>', '?', 0, '*', 0, ' ', 0, 1 , 14, 15							// 30 - 3a ends on caps lock
	},
};

/*
 * initialize_keyboard

 *   DESCRIPTION: Initializes keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: keyboard is initialized
 */

void initialize_keyboard()
{

	set_start_y_pos();

	//enable keyboard interrupts
	enable_irq( KEYBOARD_IRQ );
}

/*
 * keyboard_handler

 *   DESCRIPTION: Handles keyboard, takes scan code inputs and calls get_key_from_scancode to print key value to screen, also handles backspace
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: calls get_key_from_scancode and print_buffer if valid scancode
 */

void keyboard_handler()
{

	uint8_t scancode_input;
	cli();

	// get scan code
	scancode_input = inb (KEYBOARD_PORT);
	// check if inside current values a-z, 0-9, will change later

	// Set shift flags
	if (scancode_input == LEFT_SHIFT_PRESSED || scancode_input == RIGHT_SHIFT_PRESSED)
		shift_status = 1;
	else if (scancode_input == LEFT_SHIFT_RELEASED || scancode_input == RIGHT_SHIFT_RELEASED)
		shift_status = 0;

	// set capslock flag
	else if (scancode_input == CAPS_LOCK_PRESSED)
		capslock_status = 1 - capslock_status;

	// set ctrl flag
	else if (scancode_input == CTRL_PRESSED)
		ctrl_status = 1;
	else if (scancode_input == CTRL_RELEASED)
		ctrl_status = 0;

	// backspace pressed
	else if(scancode_input == BACKSPACE_PRESSED)
	{
		if(text_buffer_pos[term_no] > 0)
		{
			/*
			if(text_buffer_pos == SCREEN_WIDTH) {
				set_x_pos(SCREEN_WIDTH);
				set_y_pos(get_y_pos() - 1);
			}*/
			text_buffer[term_no][text_buffer_pos[term_no]] = NULL;
			text_buffer_pos[term_no]--;
			remove_character();
			set_x_pos(get_x_pos() - 1);
			cursor_reset();

		}
		//print_buffer();
	}

	else if ( scancode_input == ENTER_PRESSED ) {

		enter_indicator = 0;
		text_buffer[term_no][text_buffer_pos[term_no]] = '\n'; //insert new line
		text_buffer_pos[term_no] = 0; //buffer position = 0 in x

		putc('\n');
		set_stypos(get_y_pos() + 1);
		set_y_pos();
		cursor_reset();
	}

	else if ( scancode_input == ALT_PRESSED ) {
		// We are switching terminals
		alt_status = 1;

	} else if ( scancode_input == ALT_RELEASED ) {
		alt_status = 0;
	}

	else
	{
		if (get_key_from_scancode(scancode_input)) {
			initx = get_y_pos();
			putc(text_buffer[term_no][text_buffer_pos[term_no]-1]);
			cursor_reset();
		}
		//print_buffer();
	}


	send_eoi(KEYBOARD_IRQ);
	sti();
}


/*
 * get_key_from_scancode

 *   DESCRIPTION: Takes scancode input and adds it to the buffer to print to screen, handles capslock, shift and ctrl+l
 *   INPUTS: scancode_input
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: calls print_buffer
 */
int get_key_from_scancode(uint8_t scancode_input)
{

	uint8_t key_setting, key, count;
	//int i;
	//uint8_t* temp[128];

	// Neither shift or Capslock
	if (shift_status == 0 && capslock_status == 0)
		key_setting = 0;
	// Shift
	else if (shift_status == 1 && capslock_status == 0)
		key_setting = 1;
	// Capslock
	else if (shift_status == 0 && capslock_status == 1)
		key_setting = 2;
	// Shift and Capslock
	else
		key_setting = 3;

	// if scancode out of index leavee
	if (scancode_input >= MAX_INDEX)
		return 0;

	// if ctrl and l are pressed
	if ( ctrl_status == 1 && scancode_input == ONE_PRESSED) /* HEY CHECK THIS PART*/
	{
		//if you want to save last line before clearing
		//doesn't seem like we need to do this but I'm
		//going to put this here just in case

		/*for(i = 0; i < TEXT_BUFFER_SIZE; i++)
		{
			temp[i] = text_buffer[i];
		}*/
		clear();
		for(count = 0; count < TEXT_BUFFER_SIZE; count++)
			text_buffer[term_no][count] = NULL;
		text_buffer_pos[term_no] = 0;


		/*for(i = 0; i < TEXT_BUFFER_SIZE; i++)
		{
			if(temp[i] == '\0')
			{
				break;
			}
			putc(temp[i]);
		}*/

		cursor_reset();
		return 0;
	}

	// get key
	key = scancode_to_key[key_setting][scancode_input];
	cli();
	//code to set the current terminal
	if (key != 0 && alt_status == 1) {  //change this back
		// Switch between different number terminals
		if(key == F1CODE) {
			////////////// saving old terminal ////////
			//save the pcb of the current terminal number
			task_array[term_no] =(uint32_t)get_pcb();
			pcb_t * last_pcb = get_pcb();
			//save old video memory
			save_video(term_no);
			svd_xyz[term_no][0] = get_x_pos() - 1;
			svd_xyz[term_no][1] = get_y_pos();

			//save ebp & esp of current process
			get_pcb()->sched_esp = tss.esp0;
			//store tss esp
			tss_arr[term_no] = tss.esp0;
			asm volatile(
				"movl %%ebp, %0;"
				"movl %%esp, %1;"

				:"=r"(last_pcb->sched_ebp),  "=r"(last_pcb->sched_esp)
			);
			switch_xy(term_no,0);
			/////// loading new terminal //////////
			//change terminal no
			term_no = 0;
			//get the right pcb
			pcb_t * next_pcb = (pcb_t *)(task_array[0]);
			set_pcb(next_pcb);
			//get it's mapping
			map_prog(next_pcb->phy_addr);
			//restore esp & ebp to tss
			//restore tss esp
			tss.esp0 = tss_arr[term_no];
			tss.ss0 = KERNEL_DS;
			asm volatile(
				"movl %0, %%ebp;"
				"movl %1, %%esp;"

				:
				:"r"(next_pcb->sched_ebp), "r"(next_pcb->sched_esp)
			);
			//turn on video memory
			clear();
			load_video(term_no);
			set_x_pos(svd_xyz[term_no][0]);
			sett_y_pos(svd_xyz[term_no][1]);
			route_video(term_no);
			send_eoi(KEYBOARD_IRQ);

			asm volatile(
				"leave;"
				"ret;"
			);




			// //if changing terminal
			// if(term_no != 0) {
			// 		//save old terminal
			// 		save_video(term_no);
			// 		//turn_off_video(term_no);
			// 		switch_xy(term_no,0);
			// 		term_no = 0;
			// 		load_video(term_no);
			//
			// 		set_attrib(0x7);
			}
			// Save state of terminal
			// Switch to other terminal and load that one from memory

		else if(key == F2CODE) {
			//save the pcb of the current terminal number
			task_array[term_no] =(uint32_t)get_pcb();
			pcb_t * last_pcb = get_pcb();
			//save old video memory
			save_video(term_no);
			svd_xyz[term_no][0] = get_x_pos() - 1;
			svd_xyz[term_no][1] = get_y_pos();
			//save ebp & esp of current process
			//get_pcb()->sched_esp = tss.esp0;
			tss_arr[term_no] = tss.esp0;
			asm volatile(
				"movl %%ebp, %0;"
				"movl %%esp, %1;"

				:"=r"(last_pcb->sched_ebp), "=r"(last_pcb->sched_esp)
			);
			//get the new process
			switch_xy(term_no,1);
			//if not already executed
			if (term_mask[1]==0){
				term_mask[1] = 1;
				term_no = 1;
				clear();
				send_eoi(KEYBOARD_IRQ);
				execute((uint8_t*)"shell ");
			}
			else{
				//change terminal no
				term_no = 1;
				//load the stuff for the next one
				//get the right pcb
				pcb_t * next_pcb = (pcb_t *)(task_array[1]);
				set_pcb(next_pcb);
				//get it's mapping
				map_prog(next_pcb->phy_addr);
				//restore esp & ebp to tss
				//tss.esp0 = (uint32_t)next_pcb->sched_esp;
				tss.esp0 = tss_arr[1];
				tss.ss0 = KERNEL_DS;
				asm volatile(
					"movl %0, %%ebp;"
					"movl %1, %%esp;"

					:
					:"r"(next_pcb->sched_ebp), "r"(next_pcb->sched_esp)
				);

				//turn on video memory
				clear();
				load_video(term_no);
				set_x_pos(svd_xyz[term_no][0]);
				sett_y_pos(svd_xyz[term_no][1]);
				route_video(term_no);
				send_eoi(KEYBOARD_IRQ);
				asm volatile(
					"leave;"
					"ret;"
				);

			}



			// //if we need a new terminal
			// if (term_mask[1]==0){
			// 	term_mask[1] = 1;
			// 	term_no = 1;
			// 	execute((uint8_t*)"shell ");
			// }
			// // Check if term_no changed
			// if(term_no != 1) {
			// 	//save old terminal
			// 	save_video(term_no);
			// 		//turn_off_video(term_no);
			// 		switch_xy(term_no,1);
			// 		term_no = 1;
			// 		load_video(term_no);
			//
			// 		set_attrib(0xA);
			//
			// }
		} else if(key == F3CODE) {
			//save the pcb of the current terminal number
			task_array[term_no] =(uint32_t)get_pcb();
			pcb_t * last_pcb = get_pcb();
			//save old video memory
			save_video(term_no);
			svd_xyz[term_no][0] = get_x_pos() - 1;
			svd_xyz[term_no][1] = get_y_pos();
			//save ebp & esp of current process
			//get_pcb()->sched_esp = tss.esp0;
			tss_arr[term_no] = tss.esp0;
			asm volatile(
				"movl %%ebp, %0;"
				"movl %%esp, %1;"

				:"=r"(last_pcb->sched_ebp), "=r"(last_pcb->sched_esp)
			);
			//get the new process
			switch_xy(term_no,1);
			//if not already executed
			if (term_mask[2]==0){
				term_mask[2] = 1;
				term_no = 2;
				clear();
				send_eoi(KEYBOARD_IRQ);
				execute((uint8_t*)"shell ");
			}
			else{
				//change terminal no
				term_no = 2;
				//load the stuff for the next one
				//get the right pcb
				pcb_t * next_pcb = (pcb_t *)(task_array[2]);
				set_pcb(next_pcb);
				//get it's mapping
				map_prog(next_pcb->phy_addr);
				//restore esp & ebp to tss
				//tss.esp0 = (uint32_t)next_pcb->sched_esp;
				tss.esp0 = tss_arr[2];
				tss.ss0 = KERNEL_DS;
				asm volatile(
					"movl %0, %%ebp;"
					"movl %1, %%esp;"

					:
					:"r"(next_pcb->sched_ebp), "r"(next_pcb->sched_esp)
				);

				//turn on video memory
				clear();
				load_video(term_no);
				set_x_pos(svd_xyz[term_no][0]);
				sett_y_pos(svd_xyz[term_no][1]);
				route_video(term_no);
				send_eoi(KEYBOARD_IRQ);
				asm volatile(
					"leave;"
					"ret;"
				);
			}




		//
		// 	//if we need a new terminal
		// 	if (term_mask[2]==0){
		// 		term_mask[2]=1;
		// 		term_no = 2;
		// 		execute((uint8_t*)"shell ");
		// 	}
		// // Check if term_no changed, if so
		// if(term_no != 2) {
		// 	//save old terminal
		// 	save_video(term_no);
		// 	switch_xy(term_no,2);
		// 	term_no = 2;
		// 	load_video(term_no);
		// 	set_attrib(0xB);
		// }
			// Save state of terminal
			// Switch to other terminal and load that one from memory
			//term_no = 2;
			//set_attrib(0xB);
		}
	}

	// if key is a character --save the last space in case we need a \n character
	if (key != 0 && text_buffer_pos[term_no] < TEXT_BUFFER_SIZE - 1)
	{
		//print key and put it into buffer
		//putc(key);
		text_buffer[term_no][text_buffer_pos[term_no]] = key;
		text_buffer_pos[term_no]++;
		return 1;

	}

	return 0;
		//putc(key);
}

/*
 * print_buffer

 *   DESCRIPTION: prints buffer to screen using putc
 *   INPUTS: scancode_input
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: buffer printed to screen
 */
void print_buffer()
{
	uint8_t count;
	//if(text_buffer_pos > 79)

	set_y_pos();
	for(count = 0; count <= text_buffer_pos[term_no]-1; count++)
	{
		set_x_pos(count /*+ keep_track*/);
		set_y_pos();
		putc(text_buffer[term_no][count]);
		set_x_pos((/*keep_track+*/count)%SCREEN_WIDTH + 1);
		cursor_reset();
	}

}

/*
 * get_text
 *   DESCRIPTION: Gets the text buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: text_buffer
 *   SIDE EFFECTS: none
 */
extern uint8_t* get_text() {
 	return text_buffer[term_no];
 }

/*
 * set_ent_ind
 *   DESCRIPTION: Sets the enter indicator
 *   INPUTS: to_set -- value the enter indicator should be
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
extern void set_ent_ind(uint8_t to_set) {
 	enter_indicator = to_set;
 }

/*
 * get_ent_ind
 *   DESCRIPTION: Gets the indicator of whether or not enter has been pressed
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: enter_indicator (1 or 0)
 *   SIDE EFFECTS: none
 */
extern uint8_t get_ent_ind() {
 	return enter_indicator;
 }


/*
 * get_term_no
 *   DESCRIPTION: Gets the current terminal number
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: term_no
 *   SIDE EFFECTS: none
 */
  extern uint8_t get_term_no() {
 	return term_no;
 }

/*
 * set_term_no
 *   DESCRIPTION: Sets the current terminal number to input
 *   INPUTS: new terminal value
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: terminal number is set to input value
 */
 extern void set_term_no(uint8_t new) {
 	term_no = new;
 }
