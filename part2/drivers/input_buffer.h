#ifndef INPUT_BUFFER_H
#define INPUT_BUFFER_H

#include "drivers/type.h"

/**
 * Input Buffer API - Part 2
 * 
 * This module provides a circular buffer for storing keyboard input
 * captured by the interrupt handler. It allows safe retrieval of
 * characters one at a time (getc) or line by line (readline).
 */

/**
 * Initialize the input buffer.
 * Must be called before any other input buffer functions.
 */
void input_buffer_init(void);

/**
 * Get a single character from the input buffer.
 * 
 * This function removes and returns one character from the buffer.
 * If the buffer is empty, it blocks (waits) until a character becomes available.
 * 
 * Returns: The next character from the buffer as an unsigned char
 */
u8int getc(void);

/**
 * Read a line of input from the buffer.
 * 
 * Reads characters from the buffer until a newline ('\n') is encountered.
 * The newline character is included in the buffer and the string is null-terminated.
 * 
 * Parameters:
 *   buf - Pointer to the destination buffer where the line will be stored
 *   size - Maximum size of the buffer (including space for null terminator)
 * 
 * Returns: Number of characters read (excluding null terminator)
 * 
 * Safety:
 *   - If the line exceeds (size-1) characters, reading stops and the buffer
 *     is null-terminated at position (size-1)
 *   - The newline character is included if there's space
 */
s32int readline(char *buf, s32int size);

/**
 * Internal function: Add a character to the input buffer.
 * 
 * This should only be called by the keyboard interrupt handler.
 * If the buffer is full, the character is silently dropped.
 * 
 * Parameters:
 *   c - The character to add to the buffer
 */
void input_buffer_putc(u8int c);

#endif /* INPUT_BUFFER_H */
