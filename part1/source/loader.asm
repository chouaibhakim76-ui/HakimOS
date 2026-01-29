global loader                       ; Export 'loader' symbol as ELF entry point

MAGIC_NUMBER equ 0x1BADB002         ; Multiboot 1 magic number required by bootloader
FLAGS        equ 0x0                ; Flags field (we use minimal features here)
CHECKSUM     equ -MAGIC_NUMBER      ; Checksum: magic + flags + checksum == 0 (FLAGS=0 simplifies)
                                     ; Ensures multiboot header validity

extern kmain                        ; C kernel main entry after setup
extern sum_of_three                 ; C function: adds three integers
extern min                          ; C function: returns smaller of two integers
extern absolute                     ; C function: absolute value of integer
extern product                      ; C function: multiplies two integers
extern is_positive                  ; C function: returns 1 if >0 else 0

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

    ; --- Test calls to C functions before entering kmain ---
    push dword 3                    ; Argument 3 for sum_of_three
    push dword 2                    ; Argument 2
    push dword 1                    ; Argument 1 (last pushed ends up lowest address)
    call sum_of_three               ; Call C function; result in EAX (ignored here)
    add esp, 12                     ; Pop 3 pushed args (3 * 4 bytes)

    push dword -1                   ; Second arg for min
    push dword 5                    ; First arg
    call min                        ; Call min; result in EAX
    add esp, 8                      ; Clean two arguments

    push dword -42                  ; Argument for absolute
    call absolute                   ; Compute absolute value
    add esp, 4                      ; Pop argument

    push dword 7                    ; Second arg for product
    push dword 6                    ; First arg
    call product                    ; Multiply
    add esp, 8                      ; Pop args

    push dword -55                  ; Argument for is_positive
    call is_positive                ; Determine positivity
    add esp, 4                      ; Pop argument

    call kmain                      ; Transfer control to C kernel main loop

.loop:                              ; Infinite loop to avoid falling off end
    jmp .loop                       ; Jump to self: halt-like behavior without HLT
