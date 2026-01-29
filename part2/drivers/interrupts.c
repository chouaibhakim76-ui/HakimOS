#include "interrupts.h"
#include "pic.h"
#include "io.h"
#include "frame_buffer.h"
#include "keyboard.h"
#include "input_buffer.h"

#define INTERRUPTS_DESCRIPTOR_COUNT 256
#define INTERRUPTS_KEYBOARD 33

struct IDTDescriptor idt_descriptors[INTERRUPTS_DESCRIPTOR_COUNT];
struct IDT idt;

u32int BUFFER_COUNT;

void interrupts_init_descriptor(s32int index, u32int address)
{
    idt_descriptors[index].offset_high = (address >> 16) & 0xFFFF; // offset bits 0..15
    idt_descriptors[index].offset_low = (address & 0xFFFF);        // offset bits 16..31

    idt_descriptors[index].segment_selector = 0x08; // The second (code) segment selector in GDT: one segment is 64b.
    idt_descriptors[index].reserved = 0x00;         // Reserved.

    /*
       Bit:     | 31              16 | 15 | 14 13 | 12 | 11     10 9 8   | 7 6 5 | 4 3 2 1 0 |
       Content: | offset high        | P  | DPL   | S  | D and  GateType | 0 0 0 | reserved
        P	If the handler is present in memory or not (1 = present, 0 = not present). Set to 0 for unused interrupts or for Paging.
        DPL	Descriptor Privilige Level, the privilege level the handler can be called from (0, 1, 2, 3).
        S	Storage Segment. Set to 0 for interrupt gates.
        D	Size of gate, (1 = 32 bits, 0 = 16 bits).
    */
    idt_descriptors[index].type_and_attr = (0x01 << 7) |               // P
                                           (0x00 << 6) | (0x00 << 5) | // DPL
                                           0xe;                        // 0b1110=0xE 32-bit interrupt gate
}

void interrupts_install_idt()
{

    interrupts_init_descriptor(INTERRUPTS_KEYBOARD, (u32int)interrupt_handler_33);

    idt.address = (s32int)&idt_descriptors;
    idt.size = sizeof(struct IDTDescriptor) * INTERRUPTS_DESCRIPTOR_COUNT;
    load_idt((s32int)&idt);

    /*pic_remap(PIC_PIC1_OFFSET, PIC_PIC2_OFFSET);*/
    pic_remap(PIC_1_OFFSET, PIC_2_OFFSET);

    // Unmask keyboard interrupt (IRQ1)
    outb(0x21, inb(0x21) & ~(1 << 1));
}

/* Interrupt handlers ********************************************************/

void interrupt_handler(__attribute__((unused)) struct cpu_state cpu, u32int interrupt, __attribute__((unused)) struct stack_state stack)
{
    u8int input;
    u8int ascii;

    switch (interrupt)
    {
    case INTERRUPTS_KEYBOARD:
        // Keep reading scan codes while the keyboard has data ready
        // The status register (port 0x64) bit 0 tells us if there's data waiting
        while ((inb(0x64) & 1))
        {
            input = keyboard_read_scan_code();

            // Convert raw scan code to ASCII character
            // The keyboard function handles Ctrl key tracking and special combinations
            ascii = keyboard_scan_code_to_ascii(input);

            // Only do something if we got a valid character back (0 means nothing to print)
            if (ascii != 0)
            {
                // Special value 0x0C means user pressed Ctrl+L to clear screen
                if (ascii == 0x0C)
                {
                    fb_clear();
                }
                else
                {
                    // Add character to input buffer for terminal processing
                    input_buffer_putc(ascii);
                    
                    // Don't echo arrow keys (codes 0x80-0x83) or Tab (0x09) 
                    // Arrow keys and Tab are handled by readline_with_history, not displayed directly
                    if ((ascii >= 0x80 && ascii <= 0x83) || ascii == 0x09)
                    {
                        // Skip echo for arrow keys and Tab
                    }
                    // Echo character to screen for visual feedback
                    else if (ascii == '\b')
                    {
                        fb_put_char('\b');
                    }
                    else if (ascii == '\n')
                    {
                        fb_put_char('\n');
                    }
                    else
                    {
                        fb_put_char(ascii);
                    }
                }
            }
        }

        // Tell the interrupt controller we're done handling this interrupt
        pic_acknowledge(interrupt);
        break;

    default:
        break;
    }
}
