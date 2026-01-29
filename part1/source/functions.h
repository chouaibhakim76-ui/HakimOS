#ifndef FUNCTIONS_H // Include guard start to prevent double inclusion
#define FUNCTIONS_H // Define the guard macro

/* Adds three numbers and returns their sum */  // Declaration comment: sums three integers
int sum_of_three(int arg1, int arg2, int arg3); // Prototype: takes 3 ints, returns int

/* Finds the smaller of two numbers */ // Declaration comment: returns min(x,y)
int min(int x, int y);                 // Prototype: two ints, returns int

/* Returns the absolute (non-negative) value of a number */ // Declaration comment
int absolute(int x);                                        // Prototype: one int, returns int absolute value

/* Multiplies two numbers together */ // Declaration comment
int product(int x, int y);            // Prototype: multiply x and y

/* Checks if a number is positive: returns 1 if yes, 0 if no */ // Declaration comment
int is_positive(int x);                                         // Prototype: test for positive

/* Kernel main function: called from loader.asm after stack setup and test calls */ // Kernel entry from assembly
int kmain(void);                                                                    // Prototype for kernel main

#endif /* FUNCTIONS_H */ // End of include guard
