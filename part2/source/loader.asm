global loader                       ; Export 'loader' symbol as ELF entry point

MAGIC_NUMBER equ 0x1BADB002         ; Multiboot 1 magic number required by bootloader
FLAGS        equ 0x0                ; Flags field (we use minimal features here)
CHECKSUM     equ -MAGIC_NUMBER      ; Checksum: magic + flags + checksum == 0 (FLAGS=0 simplifies)
                                     ; Ensures multiboot header validity

extern kmain                        ; C kernel main entry after setup

section .bss                        ; Uninitialized data section
align 4                             ; Align following symbol on 4-byte boundary
kernel_stack:                       ; Label for stack memory region base
    resb 4096                       ; Reserve 4KB (4096 bytes) for kernel stack space

section .text                       ; Code (executable) section
align 4                             ; Ensure multiboot header is 4-byte aligned
    dd MAGIC_NUMBER                 ; Write multiboot magic number
    dd FLAGS                        ; Write flags field
    dd CHECKSUM                     ; Write computed checksum (verifies header)

loader:                             ; Entry point label (matches linker ENTRY)
    mov eax, 0xCAFEBABE             ; Place recognizable constant into EAX (debug marker)

    mov esp, kernel_stack + 4096    ; Initialize stack pointer to top of reserved stack

    call kmain                      ; Transfer control to C kernel main loop

.loop:                              ; Infinite loop to avoid falling off end
    jmp .loop                       ; Jump to self: halt-like behavior without HLT
