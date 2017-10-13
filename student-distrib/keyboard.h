#ifndef _KEYBOARD_H
#define _KEYBOARD_H


#include "types.h"
#include "lib.h"
#include "i8259.h"

// keyboard IRQ number
#define KEYBOARD_IRQ 1
//Keyboard Port line
#define KEYBOARD_PORT 0x60

// takes scancode and prints key value to screen
 extern void keyboard_handler ();
 // initlizes keyboard
 extern void initialize_keyboard ();

 #endif

