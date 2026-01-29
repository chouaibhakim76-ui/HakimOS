// kmain.c                                                               // Source file for kernel main
#include "functions.h"              // Include arithmetic/test functions
#include "../drivers/framebuffer.h" // Include framebuffer API

// Kernel main function called from loader.asm after setup               // Entry after low-level init
int kmain(void)                                        // Define kmain returning int
{                                                      // Start of kmain body
    /* Initialize framebuffer and print test output */ // High-level step description
    fb_init();                                         // Initialize and clear framebuffer
    fb_set_color(0x0F, 0x00); /* white on black */     // Set starting color scheme
    fb_clear();                                        // Explicit clear (already done by init)

    fb_print("Welcome to Hakim OS\n");                   // Greeting banner
    fb_print("Framebuffer initialized successfully.\n"); // Status confirmation line

    fb_set_color(0x05, 0x00); /* purple (magenta) on black */ // Color for sum_of_three line
    fb_print("sum_of_three(1,2,3) = ");                       // Label for sum_of_three result
    fb_print_int(sum_of_three(1, 2, 3));                      // Print computed sum
    fb_put_char('\n');                                        // Line break after result

    fb_set_color(0x0B, 0x00); /* cyan on black */ // Color for min
    fb_print("min(5,-1) = ");                     // Label for min result
    fb_print_int(min(5, -1));                     // Print minimum value
    fb_put_char('\n');                            // Newline

    fb_set_color(0x02, 0x00); /* green on black */ // Color for absolute
    fb_print("absolute(-42) = ");                  // Label for absolute result
    fb_print_int(absolute(-42));                   // Print absolute value
    fb_put_char('\n');                             // Newline

    fb_set_color(0x04, 0x00); /* red on black */ // Color for product
    fb_print("product(6,7) = ");                 // Label for product result
    fb_print_int(product(6, 7));                 // Print multiplication result
    fb_put_char('\n');                           // Newline

    fb_set_color(0x05, 0x00); /* magenta on black */ // Color for is_positive
    fb_print("is_positive(-55) = ");                 // Label for positivity check
    fb_print_int(is_positive(-55));                  // Print boolean-like result (0/1)
    fb_put_char('\n');                               // Newline

    fb_set_color(0x0F, 0x00); /* restore white */ // Restore default color for any further output

    // Framebuffer driver test section
    fb_put_char('\n');                      // Add blank line
    fb_set_color(0x0F, 0x00);               // White text
    fb_print("Framebuffer driver test:\n"); // Section header

    // Show cursor movement test - actually move the cursor
    fb_move(15, 9);                          // Actually move cursor to (15, 9)
    fb_set_color(0x0F, 0x00);                // White text
    fb_print("Cursor moved to (15, 9)\n\n"); // Show cursor movement info

    // Number display tests
    fb_set_color(0x09, 0x00); // Light blue text
    fb_print("Number: ");     // Label
    fb_print_int(123456789);  // Display positive number
    fb_print("  Negative: "); // Negative label
    fb_print_int(-37);        // Display negative number
    fb_put_char('\n');        // Newline

    // Hexadecimal display
    fb_set_color(0x05, 0x00);      // Purple/Magenta text
    fb_print("Hex: 0xCAFEBABE\n"); // Display hex value

    fb_put_char('\n'); // Blank line

    // Color display tests (6 colors)
    fb_set_color(0x04, 0x00); // Red
    fb_print("Red ");         // Red text

    fb_set_color(0x09, 0x00); // Blue (light blue)
    fb_print("Blue ");        // Blue text

    fb_set_color(0x02, 0x00); // Green
    fb_print("Green ");       // Green text

    fb_set_color(0x0F, 0x00); // White
    fb_print("White ");       // White text

    fb_set_color(0x0E, 0x00); // Yellow
    fb_print("Yellow ");      // Yellow text

    fb_set_color(0x05, 0x00); // Magenta
    fb_print("Magenta");      // Magenta text

    fb_put_char('\n'); // Newline
    fb_put_char('\n'); // Blank line

    // Buffer write test
    fb_set_color(0x09, 0x00);       // Light blue
    fb_print("Buffer write test!"); // Buffer write test message

    fb_put_char('\n'); // Newline
    fb_put_char('\n'); // Blank line

    fb_set_color(0x09, 0x00);                 // Light blue
    fb_print("Framebuffer 2D API complete!"); // Confirmation message

    while (1)                     // Infinite loop to keep kernel active
    {                             // Loop body start
        /* keep kernel running */ // Prevent returning to loader / halting
    } // Loop body end (never reached exit)
    return 0; /* not reached */ // Return statement for completeness
} // End of kmain
