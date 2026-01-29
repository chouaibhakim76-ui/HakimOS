#ifndef STRINGS_H
#define STRINGS_H

#include "drivers/type.h"

/**
 * String utility functions for terminal command parsing
 */

/**
 * Compare two strings.
 * 
 * Parameters:
 *   s1 - first string
 *   s2 - second string
 * 
 * Returns: 0 if strings are equal, non-zero if different
 */
s32int strcmp(const char *s1, const char *s2);

/**
 * Get length of a string.
 * 
 * Parameters:
 *   s - the string
 * 
 * Returns: length of string (excluding null terminator)
 */
u32int strlen(const char *s);

/**
 * Set memory to a specific value.
 * 
 * Parameters:
 *   dest - destination memory
 *   val - value to set
 *   len - number of bytes
 * 
 * Returns: pointer to dest
 */
void *memset(void *dest, s32int val, u32int len);

#endif /* STRINGS_H */
