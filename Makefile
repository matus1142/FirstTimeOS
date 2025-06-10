CFLAG = -m32 -fno-stack-protector -fno-builtin

all: clean kernel boot image

clean:
	rm -f *.o
kernel:
	gcc $(CFLAG) -c kernel.c -o kernel.o
	gcc $(CFLAG) -c vga.c -o vga.o
	gcc $(CFLAG) -c gdt.c -o gdt.o
	gcc $(CFLAG) -c util.c -o util.o
boot:
	nasm -f elf32 boot.s -o boot.o
	nasm -f elf32 gdt.s -o gdts.o
image:
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o vga.o gdt.o gdts.o util.o
	mv kernel iso/boot/kernel
	grub-mkrescue -o kernel.iso iso/
	rm *.o
emu:
	qemu-system-i386 kernel.iso