// gdt.h - Global Descriptor Table structures and function declarations
// This header defines the data structures needed for x86 protected mode segmentation

#include "stdint.h"

/**
 * GDT Entry Structure - represents one segment descriptor in the GDT
 * 
 * The x86 GDT entry format is 8 bytes and has a complex layout due to
 * historical compatibility requirements. Each entry describes a memory segment
 * with its base address, limit (size), and access permissions.
 * 
 * Reference: https://wiki.osdev.org/Global_Descriptor_Table
 */
struct gdt_entry_struct{
    uint16_t limit;         // Lower 16 bits of segment limit (bits 0-15)
                           // Combined with flags field to form 20-bit limit
    
    uint16_t base_low;      // Lower 16 bits of base address (bits 0-15)
    uint8_t base_middle;    // Middle 8 bits of base address (bits 16-23)
    
    uint8_t access;         // Access byte containing:
                           // Bit 7: Present (P) - Must be 1 for valid segments
                           // Bits 6-5: Descriptor Privilege Level (DPL) - Ring 0-3
                           // Bit 4: Descriptor Type (S) - 1=code/data, 0=system
                           // Bits 3-0: Type field - defines segment type and permissions
    
    uint8_t flags;          // Combined field containing:
                           // Bits 7-4: Flags (G,D/B,L,AVL)
                           //   G: Granularity (0=byte, 1=4KB blocks)
                           //   D/B: Default operation size (0=16bit, 1=32bit)
                           //   L: Long mode (0=compatibility, 1=64bit) - unused in 32bit
                           //   AVL: Available for OS use
                           // Bits 3-0: Upper 4 bits of segment limit (bits 16-19)
    
    uint8_t base_high;      // Upper 8 bits of base address (bits 24-31)
} __attribute__((packed));  // Prevent compiler padding - must be exactly 8 bytes

/**
 * GDT Pointer Structure - used by LGDT instruction to load the GDT
 * 
 * This 6-byte structure tells the processor where the GDT is located
 * in memory and how large it is.
 */
struct gdt_ptr_struct{
    uint16_t limit;         // Size of GDT in bytes minus 1 (max 2^16-1 = 65535)
                           // For n entries: limit = (n * 8) - 1
    
    unsigned int base;      // Linear address of the first GDT entry
                           // Must be aligned and accessible to processor
} __attribute__((packed));  // Must be exactly 6 bytes for LGDT instruction

/**
 * Task State Segment (TSS) Structure - used for hardware task switching
 * 
 * The TSS stores processor state information and is primarily used for:
 * 1. Privilege level transitions (user to kernel mode)
 * 2. Hardware task switching (rarely used in modern OSes)
 * 3. I/O permission bitmap storage
 * 
 * Most fields are legacy - modern OSes mainly use ss0/esp0 for syscalls.
 */
struct tss_entry_struct{
    // Task linking and hardware task switching (legacy)
    uint32_t prev_tss;      // Selector of previous TSS (for nested tasks)
    
    // Stack pointers for different privilege levels (Ring 0-2)
    uint32_t esp0;          // Stack pointer for Ring 0 (kernel mode)
    uint32_t ss0;           // Stack segment for Ring 0 (kernel mode)
    uint32_t esp1;          // Stack pointer for Ring 1 (rarely used)
    uint32_t ss1;           // Stack segment for Ring 1 (rarely used)
    uint32_t esp2;          // Stack pointer for Ring 2 (rarely used)
    uint32_t ss2;           // Stack segment for Ring 2 (rarely used)
    
    // Memory management and processor state (saved during task switch)
    uint32_t cr3;           // Page directory base register (PDBR)
    uint32_t eip;           // Instruction pointer
    uint32_t eflags;        // Processor flags register
    
    // General purpose registers (saved during task switch)
    uint32_t eax;           // Accumulator register
    uint32_t ecx;           // Counter register  
    uint32_t edx;           // Data register
    uint32_t ebx;           // Base register
    uint32_t esp;           // Current stack pointer
    uint32_t ebp;           // Base pointer register
    uint32_t esi;           // Source index register
    uint32_t edi;           // Destination index register
    
    // Segment registers (saved during task switch)
    uint32_t es;            // Extra segment selector
    uint32_t cs;            // Code segment selector
    uint32_t ss;            // Stack segment selector
    uint32_t ds;            // Data segment selector
    uint32_t fs;            // Additional segment selector
    uint32_t gs;            // Additional segment selector
    
    // System management
    uint32_t ldt;           // Local Descriptor Table selector (rarely used)
    uint32_t trap;          // Debug trap flag (bit 0) + reserved bits
    uint32_t iomap_base;    // Offset to I/O permission bitmap (from TSS base)
                           // If >= TSS limit, no I/O bitmap exists
} __attribute__((packed));  // Must match hardware TSS format exactly

// Function declarations for GDT management

/**
 * initGdt - Initialize the Global Descriptor Table
 * Sets up basic kernel/user code/data segments and TSS
 */
void initGdt();

/**
 * setGdtGate - Configure a single GDT entry
 * @num: GDT entry index (0-based)
 * @base: 32-bit base address of segment
 * @limit: 20-bit segment limit (size)
 * @access: Access byte (privilege, type, etc.)
 * @gran: Granularity and flags byte
 */
void setGdtGate(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

/**
 * writeTSS - Set up Task State Segment in GDT
 * @num: GDT entry index for TSS descriptor
 * @ss0: Kernel stack segment selector (for privilege transitions)
 * @esp0: Kernel stack pointer (for privilege transitions)  
 */
void writeTSS(uint32_t num, uint16_t ss0, uint32_t esp0);