#include "drivers/frame_buffer.h"
#include "drivers/interrupts.h"
#include "drivers/hardware_interrupt_enabler.h"
#include "drivers/input_buffer.h"
#include "drivers/io.h"
#include "terminal.h"

int kmain(void)
{
    // Initialize framebuffer
    fb_init();

    // Install the IDT and configure PIC
    interrupts_install_idt();

    // Initialize input buffer
    input_buffer_init();
    
    // Flush the keyboard controller buffer before enabling interrupts
    // This clears any stray scan codes that might be sitting in the hardware buffer
    // The keyboard controller (port 0x60) may have data from BIOS/bootloader
    while (inb(0x64) & 0x01)
    {
        inb(0x60); // Read and discard the byte
    }

    // Enable hardware interrupts
    enable_hardware_interrupts();

    // Small delay to let any spurious interrupts settle
    for (volatile int i = 0; i < 1000000; i++);
    
    // Re-initialize buffer after delay to clear any spurious data
    input_buffer_init();

    // Initialize and run terminal (does not return)
    terminal_init();
    terminal_run();

    return 0;
}
