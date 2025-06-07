#include "vga.h"
#include "gdt.h"

void kmain(void);

void kmain(void) {
    // VGA
    Reset();
    print("Hello World!\r\n");
    
    // GDT
    initGdt();
    print("GDT is done!\r\n");
}