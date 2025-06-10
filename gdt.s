; gdt.s (Assembly)

global gdt_flush         ; Make function globally visible

gdt_flush:
    MOV eax, [esp+4]     ; Get parameter (address of gdt_ptr) from stack
    LGDT [eax]           ; Load GDT using LGDT instruction
    
    ; Reload all segment registers with new GDT
    MOV ax, 0x10         ; 0x10 = offset to kernel data segment in GDT
    MOV ds, ax           ; Data segment register
    MOV es, ax           ; Extra segment register  
    MOV fs, ax           ; FS segment register
    MOV gs, ax           ; GS segment register
    MOV ss, ax           ; Stack segment register
    
    jmp 0x08:.flush      ; Far jump to reload CS register
                         ; 0x08 = offset to kernel code segment in GDT
.flush:
    RET                  ; Return to caller

; Function: tss_flush
; Purpose: Load the Task State Segment (TSS) into the processor's task register
; This function is typically called after setting up the TSS structure in memory

global tss_flush          ; Make function globally accessible

tss_flush:
    MOV ax, 0x2B         ; Load TSS selector (0x2B) into AX register
                         ; 0x2B = segment selector for TSS in GDT
                         ; Bits 0-1: Requested Privilege Level (RPL) = 11b (ring 3)
                         ; Bit 2: Table Indicator (TI) = 0 (GDT)
                         ; Bits 3-15: Index = 5 (6th entry in GDT, 0-indexed)
    
    LTR ax              ; Load Task Register with the TSS selector
                        ; This instruction loads the segment selector into TR
                        ; and caches the TSS descriptor information
    
    RET                 ; Return to caller