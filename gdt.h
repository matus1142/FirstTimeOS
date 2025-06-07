// gdt.h
#include "stdint.h"

// GDT entry structure - represents one segment descriptor
struct gdt_entry_struct{
    // Segment Descriptor : https://wiki.osdev.org/Global_Descriptor_Table

    uint16_t limit;         // Lower 16 bits of segment limit
    uint16_t base_low;      // Lower 16 bits of base address
    uint8_t base_middle;    // Middle 8 bits of base address (bits 16-23)
    uint8_t access;         // Access byte (present, privilege level, type, etc.)
    uint8_t flags;          // Upper 4 bits of limit + flags (granularity, size, etc.)
    uint8_t base_high;      // Upper 8 bits of base address (bits 24-31)
} __attribute__((packed));  // Pack structure to prevent padding

// GDT pointer structure - used by LGDT instruction
struct gdt_ptr_struct{
    uint16_t limit;         // Size of GDT in bytes minus 1
    unsigned int base;      // Address of the GDT
} __attribute__((packed));  // Pack structure to prevent padding

// Function declarations
void initGdt();
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);
