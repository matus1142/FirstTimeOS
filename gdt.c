// gdt.c - Global Descriptor Table implementation for x86 protected mode
#include "gdt.h"
#include "util.h"

// External assembly function that loads the GDT into the processor
extern void gdt_flush(uint32_t addr_t);

// External assembly function that flushes the Task State Segment
extern void tss_flush();

// Array of 6 GDT entries (0-5: null, kernel code/data, user code/data, TSS)
struct gdt_entry_struct gdt_entries[6];

// GDT pointer structure containing limit and base address for LGDT instruction
struct gdt_ptr_struct gdt_ptr;

// Task State Segment entry for hardware task switching and privilege transitions
struct tss_entry_struct tss_entry;

/**
 * initGdt - Initialize the Global Descriptor Table
 * 
 * Sets up a basic GDT with kernel and user segments, plus TSS.
 * This enables protected mode memory segmentation and privilege levels.
 */
void initGdt(void) {
    // Calculate GDT limit: total size of all entries minus 1 (as per Intel spec)
    gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * 6) - 1;
    // Set base address to point to our GDT entries array
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // Set up the 6 required segments:
    setGdtGate(0, 0, 0, 0, 0);                    // Null segment (required by Intel)
    
    setGdtGate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);    // Kernel code segment (Ring 0)
    // 0x9A = 10011010b: Present=1, DPL=00b(Ring 0), S=1, Type=1010b(Code Execute/Read)
    // 0xCF = 11001111b: G=1(4KB granularity), D/B=1(32-bit), L=0, AVL=0, Limit[19:16]=1111b
    
    setGdtGate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);    // Kernel data segment (Ring 0)
    // 0x92 = 10010010b: Present=1, DPL=00b(Ring 0), S=1, Type=0010b(Data Read/Write)
    
    setGdtGate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);    // User code segment (Ring 3)
    // 0xFA = 11111010b: Present=1, DPL=11b(Ring 3), S=1, Type=1010b(Code Execute/Read)
    
    setGdtGate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);    // User data segment (Ring 3)
    // 0xF2 = 11110010b: Present=1, DPL=11b(Ring 3), S=1, Type=0010b(Data Read/Write)

    // Set up TSS entry at index 5 with kernel stack (ss0=0x10, esp0=0x0)
    writeTSS(5, 0x10, 0x0);

    // Load the GDT into processor using assembly function
    gdt_flush((uint32_t)&gdt_ptr);
    // Load TSS selector into task register
    tss_flush();
}

/**
 * writeTSS - Write Task State Segment entry to GDT
 * @num: GDT entry index for the TSS
 * @ss0: Kernel stack segment selector (used for privilege transitions)
 * @esp0: Kernel stack pointer (used when switching from user to kernel mode)
 * 
 * The TSS is used by the processor for hardware task switching and storing
 * kernel stack information for privilege level transitions.
 */
void writeTSS(uint32_t num, uint16_t ss0, uint32_t esp0){
    // Calculate base address of our TSS structure
    uint32_t base = (uint32_t)&tss_entry;
    // Calculate limit (size of TSS structure)
    uint32_t limit = base + sizeof(tss_entry);

    // Create GDT entry for TSS: 0xE9 = Present, Ring 3 access, TSS type
    // 0xE9 = 11101001b: Present=1, DPL=11b, S=0(system), Type=1001b(Available TSS)
    setGdtGate(num, base, limit, 0xE9, 0x00);

    // Initialize TSS structure to zero
    memset(&tss_entry, 0, sizeof(tss_entry));

    // Set kernel stack for privilege transitions (Ring 3 -> Ring 0)
    tss_entry.ss0 = ss0;    // Kernel stack segment (typically 0x10)
    tss_entry.esp0 = esp0;  // Kernel stack pointer

    // Set default segment selectors with Ring 3 privilege
    tss_entry.cs = 0x08 | 0x3;  // Code segment: kernel code (0x08) + Ring 3 (0x3)
    // Set all data segments to kernel data (0x10) + Ring 3 (0x3)
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}

/**
 * setGdtGate - Set up a single GDT entry
 * @num: Index of the GDT entry (0-5)
 * @base: 32-bit base address of the segment
 * @limit: 20-bit limit (size) of the segment
 * @access: Access byte (privilege level, segment type, etc.)
 * @gran: Granularity and flags byte
 * 
 * The GDT entry format is complex due to historical x86 compatibility.
 * This function properly splits the base and limit across multiple fields.
 */
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran){
    // Set lower 16 bits of the 20-bit limit
    gdt_entries[num].limit = limit & 0xFFFF;

    // Split 32-bit base address into three parts (Intel segmentation legacy)
    gdt_entries[num].base_low = base & 0xFFFF;          // Bits 0-15 of base
    gdt_entries[num].base_middle = (base >> 16) & 0xFF; // Bits 16-23 of base  
    gdt_entries[num].base_high = (base >> 24) & 0xFF;   // Bits 24-31 of base
    
    // Set upper 4 bits of limit (bits 16-19) in the flags field
    gdt_entries[num].flags = (limit >> 16) & 0x0F;
    // Combine limit bits with granularity/flags (upper 4 bits)
    // gran contains: G(granularity), D/B(default size), L(long mode), AVL(available)
    gdt_entries[num].flags = gdt_entries[num].flags | (gran & 0xF0);
    
    // Set access byte containing privilege and type information
    gdt_entries[num].access = access;
}