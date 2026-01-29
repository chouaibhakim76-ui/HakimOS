/* framebuffer.c - Implementation of simple VGA text-mode driver */ // File purpose comment
#include "framebuffer.h"                                            // Header with public prototypes

/* Simple VGA text-mode framebuffer driver (memory-mapped at 0xB8000)                // General description
   Provides a small 2D API: move cursor, set color, print strings and ints. */
// Continues description

typedef unsigned short u16; // 16-bit unsigned type alias
typedef unsigned char u8;   // 8-bit unsigned type alias

#define FB_ADDR 0xB8000 // Physical address of VGA text memory
#define FB_COLS 80      // Number of text columns
#define FB_ROWS 25      // Number of text rows

static volatile u16 *const fb = (volatile u16 *)FB_ADDR; // Pointer to framebuffer memory (volatile)
static int cursor_x = 0;                                 // Current cursor column
static int cursor_y = 0;                                 // Current cursor row
static u8 current_color = 0x0F; /* white on black */     // Current attribute byte (fg+bg)

// Port I/O helper functions for cursor control
static inline void outb(u16 port, u8 value)                     // Write byte to I/O port
{                                                               // Inline assembly for port output
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port)); // AT&T syntax: out value to port
}

static void fb_disable_cursor(void) // Disable hardware text cursor
{                                   // VGA cursor control via ports 0x3D4/0x3D5
    outb(0x3D4, 0x0A);              // Select cursor start register
    outb(0x3D5, 0x20);              // Set bit 5 to disable cursor
}

static void fb_update_cursor(void)           // Update hardware cursor position (no-op)
{                                            // Start of function
    /* Cursor disabled - no update needed */ // Explanation
} // End of fb_update_cursor

void fb_init(void)       // Public init function
{                        // Start of function
    fb_disable_cursor(); // Disable blinking hardware cursor
    fb_clear();          // Clear screen and reset state
} // End of fb_init

void fb_clear(void)                                  // Clear entire framebuffer
{                                                    // Start of function
    int i;                                           // Loop counter index
    for (i = 0; i < FB_ROWS * FB_COLS; ++i)          // Iterate over every cell
    {                                                // Loop body
        fb[i] = ((u16)current_color << 8) | (u8)' '; // Write space with current color
    } // End loop
    cursor_x = 0;       // Reset cursor column to start
    cursor_y = 0;       // Reset cursor row to start
    fb_update_cursor(); // (No-op) hardware cursor update
} // End of fb_clear

void fb_set_color(unsigned char fg, unsigned char bg) // Set foreground and background
{                                                     // Start of function
    current_color = (bg << 4) | (fg & 0x0F);          // Compose attribute: high nibble bg, low nibble fg
} // End of fb_set_color

void fb_move(unsigned short x, unsigned short y) // Move logical cursor
{                                                // Start of function
    if (x >= FB_COLS)                            // Check x within bounds
        x = FB_COLS - 1;                         // Clamp to last column if out-of-range
    if (y >= FB_ROWS)                            // Check y within bounds
        y = FB_ROWS - 1;                         // Clamp to last row if out-of-range
    cursor_x = x;                                // Update global cursor column
    cursor_y = y;                                // Update global cursor row
    fb_update_cursor();                          // (No-op) hardware cursor update
} // End of fb_move

static void fb_scroll_if_needed(void)                                // Scroll screen if cursor past bottom
{                                                                    // Start of function
    if (cursor_y < FB_ROWS)                                          // If still inside visible rows
        return;                                                      // Early return: no scroll needed
    int row, col;                                                    // Loop indices for copying rows
    for (row = 1; row < FB_ROWS; ++row)                              // Start at second row, move each up
    {                                                                // Row loop body
        for (col = 0; col < FB_COLS; ++col)                          // Iterate columns in row
        {                                                            // Column loop body
            fb[(row - 1) * FB_COLS + col] = fb[row * FB_COLS + col]; // Copy cell from next row up one line
        } // End column loop
    } // End row loop
    for (col = 0; col < FB_COLS; ++col)                                          // Clear last row after scrolling
    {                                                                            // Column loop body for last row clear
        fb[(FB_ROWS - 1) * FB_COLS + col] = ((u16)current_color << 8) | (u8)' '; // Write space with current color
    } // End clear loop
    cursor_y = FB_ROWS - 1; // Place cursor on last visible row
} // End of fb_scroll_if_needed

void fb_put_char(char c) // Put a single character handling control chars
{                        // Start of function
    if (c == '\n')       // If newline character
    {                    // Newline branch
        cursor_x = 0;    // Reset column to start of line
        ++cursor_y;      // Move to next line
    } // End newline branch
    else if (c == '\r') // Carriage return (return to column 0)
    {                   // Carriage return branch
        cursor_x = 0;   // Reset column only
    } // End carriage return branch
    else                                                                       // Regular printable character handling
    {                                                                          // Start printable branch
        fb[cursor_y * FB_COLS + cursor_x] = ((u16)current_color << 8) | (u8)c; // Store character + attribute in buffer
        ++cursor_x;                                                            // Advance cursor column
        if (cursor_x >= FB_COLS)                                               // Check if we reached end of line
        {                                                                      // Line wrap branch
            cursor_x = 0;                                                      // Wrap to column 0
            ++cursor_y;                                                        // Advance to next row
        } // End wrap branch
    } // End printable branch
    fb_scroll_if_needed(); // Scroll if we moved past bottom
    fb_update_cursor();    // (No-op) hardware cursor update
} // End of fb_put_char

void fb_print(const char *str) // Print a null-terminated C string
{                              // Start of function
    if (!str)                  // Handle NULL pointer input
        return;                // Nothing to print
    const char *p = str;       // Iterator pointer over string
    while (*p)                 // Loop until NUL terminator encountered
    {                          // Loop body
        fb_put_char(*p++);     // Output current char then advance
    } // End loop
} // End of fb_print

void fb_print_int(int value) // Print signed integer in decimal
{                            // Start of function
    char buf[12];            // Temporary buffer (enough for -2147483648 + NUL)
    int i = 0;               // Index for buffer filling
    int negative = 0;        // Flag tracking negativity
    if (value == 0)          // Special case zero
    {                        // Zero branch
        fb_put_char('0');    // Output single zero
        return;              // Finish early
    } // End zero branch
    if (value < 0)      // Check if negative
    {                   // Negative branch
        negative = 1;   // Mark negative
        value = -value; // Work with positive magnitude
    } // End negative branch
    while (value > 0 && i < (int)sizeof(buf) - 1) // Convert digits while space remains
    {                                             // Digit extraction loop body
        buf[i++] = '0' + (value % 10);            // Store least significant digit as ASCII
        value /= 10;                              // Remove least significant digit
    } // End digit loop
    if (negative)            // If original number negative
        buf[i++] = '-';      // Append minus sign
    while (i--)              // Output characters in reverse order
        fb_put_char(buf[i]); // Print next character from buffer
} // End of fb_print_int
