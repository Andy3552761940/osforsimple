; Multiboot2-compliant 64-bit kernel entry and long-mode switcher

BITS 32

section .multiboot_header
align 8
multiboot_header_start:
    dd 0xe85250d6            ; magic
    dd 0                     ; architecture (protected mode)
    dd multiboot_header_end - multiboot_header_start
    dd -(0xe85250d6 + 0 + (multiboot_header_end - multiboot_header_start))
    ; End tag
    dw 0
    dw 0
    dd 8
multiboot_header_end:

section .text
align 16
global _start
extern kmain

_start:
    cli
    lgdt [gdt_descriptor]

    ; Set up initial 32-bit stack below 1 MiB to keep addresses <4GiB
    mov esp, stack_top32
    mov ebp, 0

    call enable_long_mode
    jmp 0x08:long_mode_entry

; Enable long mode and paging using identity mapped 2MiB pages
align 16
enable_long_mode:
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5           ; CR4.PAE
    mov cr4, eax

    ; Load PML4 into CR3
    mov eax, pml4_table
    mov cr3, eax

    ; Enable long mode via IA32_EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8           ; EFER.LME
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, (1 << 31) | 1    ; CR0.PG | CR0.PE
    mov cr0, eax
    ret

; Long mode entry point
[BITS 64]
long_mode_entry:
    mov ax, 0x10             ; data segment
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack_top64

    extern kmain
    call kmain

.hang:
    hlt
    jmp .hang

; ================= GDT =================
section .rodata
align 8
gdt_start:
    dq 0
    dq 0x00af9a000000ffff    ; 64-bit code segment
    dq 0x00af92000000ffff    ; 64-bit data segment
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start

; ============= PAGE TABLES =============
section .data
align 4096
pml4_table:
    dq pdpt_table + 0x003
    times 511 dq 0

align 4096
pdpt_table:
    dq pd_table + 0x003
    times 511 dq 0

align 4096
pd_table:
%assign i 0
%rep 512
    dq (i * 0x200000) | 0x083 ; Present | Write | 2MiB page
%assign i i+1
%endrep

section .bss
; Stacks
align 16
stack_space32: resb 8192
stack_top32:

align 16
stack_space64: resb 16384
stack_top64:

section .note.GNU-stack noalloc noexec nowrite progbits
