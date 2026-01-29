OBJECTS = source/loader.o \
          source/kmain.o \
          source/terminal.o \
          source/strings.o \
          drivers/io.o \
          drivers/frame_buffer.o \
          drivers/hardware_interrupt_enabler.o \
          drivers/interrupt_asm.o \
          drivers/interrupt_handlers.o \
          drivers/interrupts.o \
          drivers/keyboard.o \
          drivers/pic.o \
          drivers/input_buffer.o

CC = gcc

CFLAGS = -I. -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
-nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c

LDFLAGS = -T ./source/link.ld -melf_i386

AS = nasm

ASFLAGS = -f elf

all: kernel.elf

# Explicit rules for each object file
source/loader.o: source/loader.asm
	$(AS) $(ASFLAGS) $< -o $@

source/kmain.o: source/kmain.c
	$(CC) $(CFLAGS) $< -o $@

drivers/%.o: drivers/%.s
	$(AS) $(ASFLAGS) $< -o $@

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	cp stage2_eltorito iso/boot/grub/stage2_eltorito
	echo "default=0\ntimeout=0\ntitle os\nkernel /boot/kernel.elf" > iso/boot/grub/menu.lst
	genisoimage -R \
				-b boot/grub/stage2_eltorito \
				-no-emul-boot \
				-boot-load-size 4 \
				-A os \
				-input-charset utf8 \
				-quiet \
				-boot-info-table \
				-o os.iso \
				iso

run: os.iso
	qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32 -d cpu -D logQ.txt

run-curses: os.iso
	qemu-system-i386 -display curses \
		-monitor telnet::55454,server,nowait \
		-chardev stdio,id=char0 \
		-serial chardev:char0 \
		-boot d \
		-cdrom os.iso \
		-m 32 \
		-d cpu \
		-no-reboot \
		-no-shutdown \
		-D logQ.txt

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o source/*.o drivers/*.o kernel.elf os.iso iso/boot/kernel.elf logQ.txt
