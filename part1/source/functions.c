/* Adds three numbers and returns their sum (book version) */ // High-level description of next function
#include "functions.h"                                        // Include header with function declarations
int sum_of_three(int arg1, int arg2, int arg3)                // Define function taking three ints
{                                                             // Start of function body
    return arg1 + arg2 + arg3;                                // Compute and return sum of arguments
} // End of sum_of_three

/* Finds the smaller of two numbers */ // High-level description
int min(int x, int y)                  // Define function comparing two ints
{                                      // Start of function body
    return (x < y) ? x : y;            // Ternary operator: return smaller of x and y
} // End of min

/* Returns the absolute (non-negative) value of a number */ // High-level description
int absolute(int x)                                         // Define absolute value function
{                                                           // Start of function body
    return (x < 0) ? -x : x;                                // If negative, negate; else return as-is
} // End of absolute

/* Multiplies two numbers together */ // High-level description
int product(int x, int y)             // Define product function
{                                     // Start of function body
    return x * y;                     // Return multiplication of x and y
} // End of product

/* Checks if a number is positive: returns 1 if yes, 0 if no */ // High-level description
int is_positive(int x)                                          // Define positivity test function
{                                                               // Start of function body
    return (x > 0) ? 1 : 0;                                     // Return 1 if >0 else 0
} // End of is_positive
