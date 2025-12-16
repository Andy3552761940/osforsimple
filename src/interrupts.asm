BITS 64

%macro PUSH_ALL 0
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro POP_ALL 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    pop rax
%endmacro

extern timer_tick
extern keyboard_handle_interrupt

section .text
global isr_stub_timer
isr_stub_timer:
    PUSH_ALL
    call timer_tick
    POP_ALL
    mov al, 0x20
    out 0x20, al
    iretq

global isr_stub_keyboard
isr_stub_keyboard:
    PUSH_ALL
    call keyboard_handle_interrupt
    POP_ALL
    mov al, 0x20
    out 0x20, al
    iretq

section .note.GNU-stack noalloc noexec nowrite progbits
