#include "keyboard.h"
#include "lib.h"




// https://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
// * specifies keycodes to be done later
static unsigned char scancode_to_key[51] =
	{
		    0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0, 0, 0, 0,	// 00 - of
			'q', 'w', 'e', 'r', 't', 'y', 'u', 'i','o', 'p', 0, 0, 0, 0, 'a', 's',	// 10 - 1f
			'd', 'f', 'g', 'h', 'j', 'k', 'l', 0, 0, 0, 0, 0, 'z', 'x', 'c', 'v',	// 20 - 2f
			'b', 'n', 'm'																// 30 - 32
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
	disable_irq(KEYBOARD_IRQ);
	enable_irq( KEYBOARD_IRQ );
}

/*
 * keyboard_handler
	
 *   DESCRIPTION: Handles keyboard, takes scan code inputs and prints key value to screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: key value is printed to screen
 */

void keyboard_handler()
{
	//cli();
	uint8_t scancode_input, key;

	// get scan code 
	scancode_input = inb (KEYBOARD_PORT);
	// check if inside current values a-z, 0-9, will change later
	if (scancode_input >= 51)
		key = 0;
	else
		key = scancode_to_key[scancode_input];

	// if pressed key is a-z, 0-9 print
	if (key != 0)
		//print key
		printf("%c", key);
		//putc(key);

	send_eoi(KEYBOARD_IRQ);
	//sti();
}

