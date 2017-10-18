#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"

//opens terminal
int32_t open_terminal(const uint8_t* filename);

//closes terminal
int32_t close_terminal(int32_t fd);

//reads display into buffer
int32_t read_terminal(int32_t fd, void* buf, int32_t nbytes);

//writes buffer to display
int32_t write_terminal(int32_t fd, const void* buf, int32_t nbytes);

#endif
