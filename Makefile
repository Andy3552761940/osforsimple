AS = nasm
CC = gcc
CFLAGS = -ffreestanding -fno-stack-protector -Wall -Wextra -O2 -m64 -mno-red-zone -mcmodel=kernel -std=gnu11 -mgeneral-regs-only -fno-pic -fno-pie
LDFLAGS = -T linker.ld -nostdlib -z max-page-size=0x1000

C_SOURCES = $(wildcard src/*.c)
ASM_SOURCES = $(wildcard src/*.asm)
OBJECTS = $(C_SOURCES:.c=.o) $(ASM_SOURCES:.asm=.o)

all: kernel.elf

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) -f elf64 $< -o $@

kernel.elf: $(OBJECTS) linker.ld
	ld -o $@ $(OBJECTS) $(LDFLAGS)

iso: kernel.elf
	mkdir -p iso/boot/grub
	cp kernel.elf iso/boot/kernel.elf
	printf "menuentry \"osforsimple\" {\n    multiboot2 /boot/kernel.elf\n    boot\n}\n" > iso/boot/grub/grub.cfg
	grub-mkrescue -o osforsimple.iso iso

run: kernel.elf
	qemu-system-x86_64 -kernel kernel.elf -serial stdio

clean:
	rm -f src/*.o kernel.elf
	rm -rf iso osforsimple.iso

.PHONY: all clean iso run
