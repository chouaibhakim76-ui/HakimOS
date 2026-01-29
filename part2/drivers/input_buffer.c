#include "input_buffer.h"

// Size of our circular buffer - 256 bytes is enough for typical input
#define INPUT_BUFFER_SIZE 256

// ===== TASK 2: INPUT BUFFER API =====
// This implements a circular buffer that stores keyboard input from interrupts
// The buffer allows the interrupt handler (producer) and terminal (consumer) 
// to work independently without losing data

// The actual storage for our characters - a simple array
static u8int buffer[INPUT_BUFFER_SIZE];

// Position where we'll read the next character from (consumer's position)
// Marked volatile because interrupt handler modifies write_pos
static volatile u32int read_pos = 0;

// Position where we'll write the next character to (producer's position)
// Marked volatile because it can be modified by interrupt handler
static volatile u32int write_pos = 0;

/**
 * Initialize the input buffer.
 * This must be called before any keyboard input is processed.
 */
void input_buffer_init(void)
{
    // Clear the entire buffer array to prevent any garbage data
    // from appearing in our input (important on first boot)
    for (u32int i = 0; i < INPUT_BUFFER_SIZE; i++)
    {
        buffer[i] = 0;
    }
    
    // Start with both positions at 0 (buffer is empty)
    read_pos = 0;
    write_pos = 0;
}

/**
 * Check if the buffer is empty.
 * When read and write positions are equal, there's nothing to read.
 * Returns: 1 if empty, 0 if data available
 */
static inline u8int buffer_is_empty(void)
{
    return read_pos == write_pos;
}

/**
 * Check if the buffer is full.
 * We can't let write_pos catch up to read_pos, or we couldn't tell
 * if the buffer is full or empty. So we always keep one slot unused.
 * Returns: 1 if full, 0 if space available
 */
static inline u8int buffer_is_full(void)
{
    // If advancing write_pos by one would equal read_pos, buffer is full
    return ((write_pos + 1) % INPUT_BUFFER_SIZE) == read_pos;
}

/**
 * Add a character to the input buffer (called by interrupt handler).
 * This is the "producer" side - the keyboard interrupt adds characters here.
 * If buffer is full, we drop the character (alternative: could overwrite oldest).
 */
void input_buffer_putc(u8int c)
{
    // Disable interrupts (cli = clear interrupt flag) to make this atomic
    // We don't want another interrupt messing with our positions mid-operation
    __asm__ volatile("cli");
    
    // Only add if there's space - prevents overwriting unread data
    if (!buffer_is_full())
    {
        // Store character at current write position
        buffer[write_pos] = c;
        
        // Move write position forward, wrapping around at buffer end (circular)
        // The % operator makes it wrap: (255 + 1) % 256 = 0
        write_pos = (write_pos + 1) % INPUT_BUFFER_SIZE;
    }
    // If buffer is full, character is silently dropped (could add error handling)
    
    // Re-enable interrupts (sti = set interrupt flag)
    __asm__ volatile("sti");
}

/**
 * TASK 2 REQUIREMENT: getc() function
 * Get a single character from the buffer - removes and returns it.
 * This is the "consumer" side - the terminal reads characters from here.
 * Blocks (waits) if buffer is empty, which is safe because keyboard
 * interrupts will eventually add characters.
 */
u8int getc(void)
{
    u8int c;
    
    // Wait until there's something to read (buffer not empty)
    // This is a "blocking" operation - we wait for input
    while (buffer_is_empty())
    {
        // Halt CPU until next interrupt (saves power, wakes on keyboard press)
        // Without this, we'd burn CPU cycles spinning in the loop
        __asm__ volatile("hlt");
    }
    
    // Now we know there's data. Disable interrupts so we can safely read it
    // without the interrupt handler changing things mid-operation
    __asm__ volatile("cli");
    
    // Read the character at current read position
    c = buffer[read_pos];
    
    // Move read position forward, wrapping around at buffer end (circular)
    read_pos = (read_pos + 1) % INPUT_BUFFER_SIZE;
    
    // Re-enable interrupts - we're done with the critical section
    __asm__ volatile("sti");
    
    // Return the character we read
    return c;
}

/**
 * TASK 2 REQUIREMENT: readline() function
 * Read a complete line of input from the buffer.
 * Reads characters one by one using getc() until user presses Enter.
 * The resulting string is null-terminated and ready to use.
 * 
 * Parameters:
 *   buf - destination buffer where the line will be stored
 *   size - maximum size of buffer (including space for null terminator)
 * 
 * Returns: number of characters read (excluding null terminator)
 */
s32int readline(char *buf, s32int size)
{
    s32int i = 0;  // Current position in output buffer
    u8int c;       // Character we just read
    
    // Keep reading until we hit Enter or run out of buffer space
    // We use size-1 to leave room for the null terminator
    while (i < size - 1)
    {
        // Get next character (this blocks if buffer is empty)
        c = getc();
        
        // If user pressed Enter, we're done reading this line
        // Don't include the newline in the output string
        if (c == '\n')
        {
            break;
        }
        
        // Handle backspace - user wants to delete previous character
        // This properly handles line editing (not just visual backspace)
        if (c == '\b')
        {
            // Only go back if there's something to delete
            if (i > 0)
            {
                i--;  // Move back one position (overwrites last char on next write)
            }
            continue;  // Don't add backspace char to buffer, just process it
        }
        
        // Normal character - add it to output buffer and advance position
        buf[i++] = c;
    }
    
    // Add null terminator to make it a proper C string
    // This goes at position i (right after last character)
    buf[i] = '\0';
    
    // Return how many characters we read (for caller's info)
    return i;
}
