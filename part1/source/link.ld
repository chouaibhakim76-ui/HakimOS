ENTRY(loader)                /* Define 'loader' label as program entry point for linker */

SECTIONS {                   /* Begin section placement directives */
    . = 0x00100000;          /* Set location counter: load everything starting at physical 1MB */

    .text ALIGN (0x1000) :   /* Start .text output section aligned on 4KB boundary */
    {                        /* Opening brace of .text section */
        *(.text)             /* Include all input .text sections from all object files */
    }                        /* End .text section */

    .rodata ALIGN (0x1000) : /* Start .rodata section aligned on 4KB boundary */
    {                        /* Opening brace .rodata */
        *(.rodata*)          /* Include all read-only data sections (match rodata & rodata.*) */
    }                        /* End .rodata section */

    .data ALIGN (0x1000) :   /* Start .data section (initialized writable data) aligned 4KB */
    {                        /* Opening brace .data */
        *(.data)             /* Include all input .data sections */
    }                        /* End .data section */

    .bss ALIGN (0x1000) :    /* Start .bss section (zero-initialized) aligned 4KB */
    {                        /* Opening brace .bss */
        *(COMMON)            /* Place all COMMON symbols (uninitialized tentative defs) */
        *(.bss)              /* Include all input .bss sections */
    }                        /* End .bss section */
}                            /* End SECTIONS block */
