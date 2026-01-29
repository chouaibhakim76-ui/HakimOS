#ifndef TERMINAL_H
#define TERMINAL_H

/**
 * Terminal Implementation - Part 3
 * 
 * Provides a basic command-line interface for the operating system.
 * Users can enter commands which are parsed and executed.
 */

/**
 * Initialize the terminal system.
 * Sets up command table and displays welcome message.
 */
void terminal_init(void);

/**
 * Run the terminal main loop.
 * Displays prompt, reads input, parses and executes commands.
 * This function does not return.
 */
void terminal_run(void);

#endif /* TERMINAL_H */
