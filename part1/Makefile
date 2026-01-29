# ====== Makefile for My OS Project ======                 # Title banner

# Name for my final ISO output                             # Variable: ISO filename
ISO = os.iso                                               # Assign ISO variable

# My kernel sources and build locations                    # Source and object mappings
KERNEL_SRC = source/loader.asm                             # Assembly loader source path
KERNEL_OBJ = source/loader.o                               # Assembly loader object file
KMAIN_SRC = source/kmain.c                                 # Kernel main C source path
KMAIN_OBJ = source/kmain.o                                 # Kernel main object file
FUNCTIONS_SRC = source/functions.c                         # Utility functions source
FUNCTIONS_OBJ = source/functions.o                         # Utility functions object
FRAMEBUF_SRC = drivers/framebuffer.c                       # Framebuffer driver source
FRAMEBUF_OBJ = drivers/framebuffer.o                       # Framebuffer driver object
KERNEL_ELF = kernel.elf                                    # Linked kernel ELF output name

# GRUB and ISO directory locations for my project          # Directory variables for build tree
# Root ISO staging directory (keep definition tight to avoid whitespace issues)
ISO_DIR=iso
# GRUB directory path inside ISO tree
GRUB_DIR=$(ISO_DIR)/boot/grub

# Default action: build my OS ISO                          # 'all' target builds final ISO
all: $(ISO)                                                # Depend on ISO rule

# Assemble my kernel (loader.asm -> loader.o)              # Rule converting assembly to object
# -f elf: Output format is ELF, for later linking          # Explanation of nasm flag
$(KERNEL_OBJ): $(KERNEL_SRC)                               # Target depends on source
	nasm -f elf $(KERNEL_SRC) -o $(KERNEL_OBJ)              # Invoke NASM assembler

# Compile kernel C code (kmain.c -> kmain.o)               # Rule for compiling kmain.c
$(KMAIN_OBJ): $(KMAIN_SRC)                                 # Dependency line
	# Compile kernel main (freestanding 32-bit)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		-Wall -Wextra -Werror -c $(KMAIN_SRC) -o $(KMAIN_OBJ)

# Compile functions.c to functions.o                       # Rule for functions.c
$(FUNCTIONS_OBJ): $(FUNCTIONS_SRC)                         # Dependency line
	# Compile utility functions (freestanding)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		-Wall -Wextra -Werror -c $(FUNCTIONS_SRC) -o $(FUNCTIONS_OBJ)

# Compile framebuffer driver                               # Rule for framebuffer driver
$(FRAMEBUF_OBJ): $(FRAMEBUF_SRC)                           # Dependency line
	# Compile framebuffer driver (freestanding)
	gcc -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs \
		-Wall -Wextra -Werror -c $(FRAMEBUF_SRC) -o $(FRAMEBUF_OBJ)

# Link object files using linker script to produce kernel.elf # Link step rule
$(KERNEL_ELF): $(KERNEL_OBJ) $(KMAIN_OBJ) $(FUNCTIONS_OBJ) $(FRAMEBUF_OBJ) # Kernel depends on all objects
	# Link all objects into final ELF
	ld -T source/link.ld -melf_i386 $(KERNEL_OBJ) $(KMAIN_OBJ) $(FUNCTIONS_OBJ) $(FRAMEBUF_OBJ) -o $(KERNEL_ELF)

# Create my ISO directory structure with GRUB files and kernel, # ISO build rule header
# then use genisoimage to build my ISO image                     # Explanation of steps
$(ISO): $(KERNEL_ELF)                                          # ISO depends on kernel ELF
	# Prepare GRUB directory tree
	mkdir -p $(GRUB_DIR)
	# Copy kernel and GRUB assets
	cp $(KERNEL_ELF) $(ISO_DIR)/boot/
	cp /opt/os/stage2_eltorito $(GRUB_DIR)/
	cp source/menu.lst $(GRUB_DIR)/
	# Build bootable ISO image
	genisoimage -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -A os \
		-input-charset utf8 -quiet -boot-info-table -o $(ISO) $(ISO_DIR)

# Rule to run QEMU with my ISO image and trace CPU activity to logQ.txt (curses + telnet monitor) # Run target purpose
run: $(ISO)                                                   # Ensure ISO is built before running
	# Launch QEMU emulator with curses display and telnet monitor
	qemu-system-i386 -display curses -monitor telnet::45454,server,nowait -serial mon:stdio \
		-boot d -cdrom $(ISO) -m 32 -d cpu -D logQ.txt

# Remove built files and clean up everything except my source  # Clean target description
clean:                                                         # Clean rule label
	# Remove build artifacts
	rm -f $(KERNEL_ELF) $(KERNEL_OBJ) $(KMAIN_OBJ) $(FUNCTIONS_OBJ) $(FRAMEBUF_OBJ) $(ISO) logQ.txt
	rm -rf $(ISO_DIR)/boot/kernel.elf

# ===== End of Makefile =====                                  # Footer banner
