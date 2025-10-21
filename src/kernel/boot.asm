
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

%define KERNEL_STACK_SIZE   0x2000

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

    mov esp, kernel_stack_top


section .bss
align 4
kernel_stack_bottom:
    resb KERNEL_STACK_SIZE
kernel_stack_top:

