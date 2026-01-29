#ifndef FRAMEBUFFER_H // Begin include guard for framebuffer interface
#define FRAMEBUFFER_H // Define guard macro

/* Basic VGA text-mode framebuffer API (2D) */ // Overview: exposes 2D text operations

/* Initialize the framebuffer (clears screen) */ // Prepares video memory, resets cursor
void fb_init(void);                              // Prototype for init

/* Clear the framebuffer and reset cursor */ // Fills screen with spaces, home cursor
void fb_clear(void);                         // Prototype for clear

/* Move cursor to (x,y) where (0,0) is top-left */ // Bounds checked move
void fb_move(unsigned short x, unsigned short y);  // Prototype for move

/* Set text color: fg (0-15) and bg (0-7) */           // VGA attribute: lower 4 bits fg, upper 4 bits bg
void fb_set_color(unsigned char fg, unsigned char bg); // Prototype for color set

/* Put a single character at the current cursor position */ // Advances cursor, handles control chars
void fb_put_char(char c);                                   // Prototype for put char

/* Print a null-terminated string starting at current cursor */ // Calls fb_put_char for each byte
void fb_print(const char *str);                                 // Prototype for print string

/* Print an integer in base 10 */ // Converts number to decimal text
void fb_print_int(int value);     // Prototype for print integer

/* Print an unsigned integer in hexadecimal with 0x prefix */ // Useful for addresses/debug
void fb_print_hex(unsigned int value);                        // Prototype for hex print

/* Get current cursor position */                         // Returns cursor coordinates via pointers
void fb_get_cursor(unsigned short *x, unsigned short *y); // Prototype for cursor getter

#endif /* FRAMEBUFFER_H */ // End include guard
