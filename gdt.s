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