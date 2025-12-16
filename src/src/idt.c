#include "idt.h"
#include "io.h"
#include <stddef.h>

extern void isr_stub_timer(void);
extern void isr_stub_keyboard(void);

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];

static inline uint64_t encode_offset(void (*handler)(void)) {
    return (uint64_t)handler;
}

static void idt_set_gate(uint8_t num, void (*handler)(void)) {
    uint64_t offset = encode_offset(handler);
    idt[num].offset_low = offset & 0xFFFF;
    idt[num].selector = 0x08; /* kernel code segment */
    idt[num].ist = 0;
    idt[num].type_attr = 0x8E; /* interrupt gate, present */
    idt[num].offset_mid = (offset >> 16) & 0xFFFF;
    idt[num].offset_high = (offset >> 32) & 0xFFFFFFFF;
    idt[num].zero = 0;
}

static void lidt(const struct idt_ptr *ptr) {
    __asm__ volatile ("lidt (%0)" : : "r"(ptr));
}

void idt_install(void) {
    for (size_t i = 0; i < 256; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].zero = 0;
    }

    idt_set_gate(32, isr_stub_timer);
    idt_set_gate(33, isr_stub_keyboard);

    struct idt_ptr ptr;
    ptr.limit = sizeof(idt) - 1;
    ptr.base = (uint64_t)idt;
    lidt(&ptr);
}
