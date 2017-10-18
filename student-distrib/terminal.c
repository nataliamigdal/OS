#include "types.h"
#include "keyboard.h"
#include "lib.h"
#include "terminal.h"
#include "i8259.h"
#include "pages.h"


/*
 * open_terminal
 *   DESCRIPTION: Opens the terminal
 *   INPUTS: filename - name of file to be opened
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t
open_terminal(const uint8_t* filename) {

	return 0;

}


/*
 * close_terminal
 *   DESCRIPTION: closes terminal
 *   INPUTS: fd - file descriptor index to be closed
 *   OUTPUTS: none
 *   RETURN VALUE: always returns 0
 *   SIDE EFFECTS: none
 */
int32_t
close_terminal(int32_t fd) {

	return 0;

}


/*
 * read_terminal
 *   DESCRIPTION: Reads what's on the terminal into a buffer
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: number of bytes read
 *   SIDE EFFECTS: none
 */
int32_t
read_terminal(int32_t fd, void* buf, int32_t nbytes) {
	
	uint8_t* buffer = (uint8_t *)buf;
	int32_t i, k, j, res=0;
	//sti();

	set_ent_ind(1);
	uint8_t* text_buffer = get_text();

	//wait for enter to be pressed
	while(get_ent_ind());
	set_ent_ind(1);

	//save display into buffer
	for(i = 0; ((i < (nbytes)) && (i < TEXT_BUFFER_SIZE)); i++) {
		buffer[i] = (char)text_buffer[i]; // THIS WILL NEED TO BE CHANGED ACCORDING TO WHAT IT IS NAMED
		if(buffer[i] != '\0'){
			res = i;
			k = i;
		}
	}
	// CALL FUNCTION WHICH CLEARS THE BUFFER
	for(j=0; j<TEXT_BUFFER_SIZE; j++) {
		text_buffer[j] = '\0';
	}

	return res+1;

}

/*
 * write_terminal
 *   DESCRIPTION: Displays buffer data to screen
 *   INPUTS: fd - file descriptor index
 *			 buf - points to buffer
 *			 nbytes - number of bytes to display
 *   OUTPUTS: none
 *   RETURN VALUE: -1 - failure
 *					result - number of bytes displayed
 *   SIDE EFFECTS: none
 */
int32_t
write_terminal(int32_t fd, const void* buf, int32_t nbytes) {
	int i,result;

	//cast pointer from void type
	int8_t* buffer = (int8_t *)buf;

	if(buffer != NULL) {
		//start printing characters to the screen up to nbytes
		for(i = 0; i < nbytes; i++)
		{
				putc(buffer[i]);
				cursor_reset(); //set new position for cursor
				result++; //increment by a byte
		}

	}
	else {
		printf("Buffer pointer was NULL");
		sti();
		return -1; //nothing to display
	}
	// CHECK TO SEE IF PRINTING FROM WHERE CURSOR IS
	sti();
	return result;
}
