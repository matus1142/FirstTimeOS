MY FRIST OS!
---
- Environment
    - Ubuntu 20.04.06 LTS
- How to compile OS & test
    - compile kernel.c : `gcc -m32 -fno-stack-protector -fno-builtin -c kernel.c -o kernel.o`
    - compile vga.c : `gcc -m32 -fno-stack-protector -fno-builtin -c vga.c -o vga.o`
    - compile gdt.c : `gcc -m32 -fno-stack-protector -fno-builtin -c gdt.c -o gdt.o`
    - compile boot.s : `nasm -f elf32 boot.s -o boot.o`
    - compile gdt.s : `nasm -f elf32 gdt.s -o gdts.o`
    - link object together : `ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o vga.o gdt.o gdts.o`
    - move kernel : `mv kernel iso/boot/kernel`
    - built iso file : `grub-mkrescue -o kernel.iso iso/`
    - run qemu x86 by using kernel.iso : `qemu-system-i386 kernel.iso`

- Makefile
    - `make clean` : remove all object files
    - `make all` : compile kernel, boot, and image
    - `make emu` : run qemu x86 by using kernel.iso
