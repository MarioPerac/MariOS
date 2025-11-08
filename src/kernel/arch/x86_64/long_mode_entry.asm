global long_mode_entry

extern kernel_main
extern kernel_stack_top
extern gdt64.pointer

section .text
bits 64

long_mode_entry:
    ; Clear interrupts
    cli
    
    ; Load GDT
    mov rax, gdt64.pointer
    lgdt [rax]
    
    ; Set up data segments
    mov ax, 0x10
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Set up stack
    mov rsp, kernel_stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if we return
    hlt
