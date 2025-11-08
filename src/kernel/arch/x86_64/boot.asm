extern kernel_main
extern long_mode_entry

section .multiboot_header
align 8 ; must be 64-bit aligned
multiboot_header_start:

    dd 0xE85250D6 ; magic - type u32
    dd 0        ; arhitecture
    dd multiboot_header_end - multiboot_header_start ; header_length
    dd -(0xE85250D6 + (multiboot_header_end - multiboot_header_start)) ; checksum


    dw 0   ; type
    dw 0   ; flags
    dd 8   ; size

multiboot_header_end:

; at entry the CPU is in 32-bit protected mode

%define PAGE_SIZE 4096
%define KERNEL_STACK_SIZE PAGE_SIZE * 2
%define KERNEL_VMA 0xFFFF800000000000 ; 2⁶⁴ − 2⁴⁷, Each half (user - lower or kernel - upper) spans 2⁴⁷ = 128 TB

section .text
bits 32 ; you’re telling NASM: I’m writing code that will run in 32-bit protected mode 
global start
start: 

; Multiboot 2 specification chapter: 3.3 I386 machine state
; When the boot loader invokes the 32-bit operating system, the machine must have the following state:
; EAX contain the magic value 0x36d76289
; EBX contain 32-bit physical address of the Multiboot2 information structure provided by the boot loader
; ...

    mov esi, ebx        ; save multiboot info pointer
    mov edi, eax        ; save magic number

    mov esp, (kernel_stack_top - KERNEL_VMA)

    call set_up_page_tables
    call enable_paging    

    ; load the 64-bit GDT
    lgdt [(gdt64.pointer - KERNEL_VMA)]

    ; jump to 64-bit code segment (kernel code)
    jmp gdt64.kernel_code:(long_mode_entry - KERNEL_VMA)
    
set_up_page_tables:
    ; map first P4 table entry to P3 table entry
    mov eax, (p3_table - KERNEL_VMA)
    or eax, 0b11 ; PTE: set Present (P) and Read/Write (R/W) bits
    mov [(p4_table - KERNEL_VMA)], eax ; store P3 table address + flags into P4[0]

    ; map 256th P4 entry to P3 entry
    mov ecx, 0x100 ; 256
    mov [(p4_table - KERNEL_VMA) + ecx*8], eax

    ; map first P3 entry to P2 entry
    mov eax, (p2_table - KERNEL_VMA)
    or eax, 0b11
    mov [(p3_table - KERNEL_VMA)], eax


    mov ecx, 0  ; counter
.map_p2_table:
    mov eax, 0x200000 ; 2MB page
    mul ecx ; eax * ecx
    or eax, 0b10000011 ; present + writable + huge
    mov [(p2_table - KERNEL_VMA) + ecx*8], eax

    inc ecx ; increase counter
    cmp ecx, 512 ; if counter == 512, the whole P2 table is mapped
    jne .map_p2_table  ; else map the next entry

    ret

enable_paging:
    ; CR3 tells the CPU where the page tables start in physical memory
    mov eax, (p4_table - KERNEL_VMA)
    mov cr3, eax

    ; bit 5 in CR4 is the PAE flag (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080 ; address of a special CPU register, called the EFER MSR
    rdmsr ; read from MSR
    or eax, 1 << 8 ; sets LME (Long Mode Enable) bit
    wrmsr ; wrtie to MSR

    mov eax, cr0
    or eax, 1 << 31 ; sets PG (Paging) bit
    mov cr0, eax

    ret

section .bss
align 4
global kernel_stack_top
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
kernel_stack_top:

section .data
align 4096

global p4_table
p4_table:
    times 512 dq 0

global p3_table
p3_table:
    times 512 dq 0

global p2_table
p2_table:
    times 512 dq 0

global gdt64
global gdt64.pointer

; Global Descriptor Table (64-bit)
; The entries in the GDT are 8 bytes long
gdt64:
.null: equ $ - gdt64 ; The null descriptor
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 0                         ; Granularity.
    db 0                         ; Base (high).
.kernel_code: equ $ - gdt64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011000b                 ; Present=1 + DPL=00 + S=1 (system segment) + Type=1000(Execute only)
    db 00100000b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
.kernel_data: equ $ - gdt64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Present=1 + DPL=00 + S=1 + Type=0010(Read/Write)
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
.pointer:                    ; The GDT-pointer.
    dw $ - gdt64 - 1             ; Limit.
    dq gdt64                     ; Base.
    
