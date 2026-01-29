#include "strings.h"

// ===== STRING UTILITY FUNCTIONS FOR TERMINAL =====
// These are standard C library functions that we implement ourselves
// because we compile with -nostdlib (no standard library available)

/**
 * Compare two null-terminated strings character by character.
 * This is used by the terminal to match commands (e.g., "help" == "help")
 * 
 * Returns: 
 *   0 if strings are equal
 *   negative if s1 < s2 (first different character is smaller in s1)
 *   positive if s1 > s2 (first different character is larger in s1)
 */
s32int strcmp(const char *s1, const char *s2)
{
    // Loop while both strings have characters AND they match
    while (*s1 && (*s1 == *s2))
    {
        s1++;  // Move to next character in first string
        s2++;  // Move to next character in second string
    }
    
    // Return difference of current characters
    // If equal, both will be '\0' (null terminator), so difference is 0
    // If different, we return the ASCII value difference
    return *(const u8int *)s1 - *(const u8int *)s2;
}

/**
 * Get the length of a string (number of characters before null terminator)
 * Not currently used in terminal, but useful utility function
 * 
 * Returns: number of characters (excluding null terminator)
 */
u32int strlen(const char *s)
{
    u32int len = 0;
    
    // Count characters until we hit null terminator
    while (*s++)  // Post-increment: test current, then move forward
    {
        len++;
    }
    
    return len;
}

/**
 * Fill a block of memory with a constant byte value
 * Not currently used in terminal, but useful for clearing buffers
 * 
 * Parameters:
 *   dest - pointer to memory to fill
 *   val - byte value to fill with (e.g., 0 to clear memory)
 *   len - number of bytes to fill
 * 
 * Returns: pointer to dest (same as input, for chaining)
 */
void *memset(void *dest, s32int val, u32int len)
{
    u8int *d = (u8int *)dest;  // Cast to byte pointer for byte-by-byte access
    
    // Fill len bytes with the value
    while (len--)
    {
        *d++ = (u8int)val;  // Write byte and advance pointer
    }
    
    return dest;  // Return original pointer
}
