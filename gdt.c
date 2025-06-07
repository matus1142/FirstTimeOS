// gdt.c
#include "gdt.h"

// External assembly function that loads the GDT
extern void gdt_flush(addr_t);

// Array of 5 GDT entries
struct gdt_entry_struct gdt_entries[5];
// GDT pointer structure
struct gdt_ptr_struct gdt_ptr;

// Initialize the Global Descriptor Table
void initGdt(void) {
    // Calculate GDT limit: size of all entries minus 1
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 5) - 1;
    // Set base address to point to our GDT entries array
    gdt_ptr.base = &gdt_entries;

    // Set up the 5 required segments:
    setGdtGate(0, 0, 0, 0, 0);                    // Null segment (required)
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);    // Kernel code segment
    // 0x9A = 10011010 = Present, Ring 0, Code, Execute/Read
    // 0xCF = 11001111 = 4KB granularity, 32-bit, Limit[19:16] = 0xF
    
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);    // Kernel data segment  
    // 0x92 = 10010010 = Present, Ring 0, Data, Read/Write
    
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);    // User code segment
    // 0xFA = 11111010 = Present, Ring 3, Code, Execute/Read
    
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);    // User data segment
    // 0xF2 = 11110010 = Present, Ring 3, Data, Read/Write

    // Load the GDT using assembly function
    gdt_flush(&gdt_ptr);
}

// Set up a single GDT entry
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    // Set lower 16 bits of limit
    gdt_entries[num].limit = limit & 0xFFFF;

    // Split 32-bit base address into three parts
    gdt_entries[num].base_low = base & 0xFFFF;          // Bits 0-15 of base
    gdt_entries[num].base_middle = (base >> 16) & 0xFF; // Bits 16-23 of base  
    gdt_entries[num].base_high = (base >> 24) & 0xFF;   // Bits 24-31 of base
    
    // Set upper 4 bits of limit in flags field
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    // Combine limit bits with granularity/flags (upper 4 bits)
    gdt_entries[num].flags = gdt_entries[num].flags | (gran & 0xF0);
    
    // Set access byte (privilege level, segment type, etc.)
    gdt_entries[num].access = access;
}