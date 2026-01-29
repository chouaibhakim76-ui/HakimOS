#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#define KEYBOARD_MAX_ASCII 83

#include "drivers/type.h"

u8int keyboard_read_scan_code(void);

u8int keyboard_scan_code_to_ascii(u8int);

u8int keyboard_is_ctrl_pressed(void);

#endif /* INCLUDE_KEYBOARD_H */
