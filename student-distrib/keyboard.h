#ifndef _KEYBOARD_H
#define _KEYBOARD_H


#include "types.h"
#include "lib.h"
#include "i8259.h"

// keyboard IRQ number
#define KEYBOARD_IRQ 1
//Keyboard Port line
#define KEYBOARD_PORT 0x60


#define LEFT_SHIFT_PRESSED 		0x2A
#define LEFT_SHIFT_RELEASED 	0xAA
#define RIGHT_SHIFT_PRESSED 	0x36
#define RIGHT_SHIFT_RELEASED 	0xB6
#define CAPS_LOCK_PRESSED 		0x3A
#define CTRL_PRESSED 			0x1D
#define CTRL_RELEASED			0x9D
#define	BACKSPACE_PRESSED		0x0E
#define BACKSPACE_RELEASED		0x8E
#define ENTER_PRESSED			0x1C
#define ENTER_RELEASD			0x9C
#define ONE_PRESSED				0x26
#define ALT_PRESSED				0x38
#define ALT_RELEASED			0xB8
#define F1CODE					1
#define F2CODE					14
#define F3CODE 					15

#define TEXT_BUFFER_SIZE 		128
#define SCREEN_WIDTH			80
#define MAX_INDEX				62


// takes scancode and prints key value to screen
 extern void keyboard_handler ();
 // initlizes keyboard
 extern void initialize_keyboard ();
// gets the key from scancode also handles shift capslock and ctrl+l
 extern int get_key_from_scancode(uint8_t scancode_input);
// prints buffer to screen
 extern void print_buffer();
//gets text buffer
 extern uint8_t* get_text();
//sets enter indicator
 extern void set_ent_ind(uint8_t to_set);
//gets the enter indicator
extern uint8_t get_ent_ind();
//gets the current teerminal number
 extern uint8_t get_term_no();
 // sets the current terminal number
 extern void set_term_no(uint8_t new);

 #endif

