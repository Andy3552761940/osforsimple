#include "vga.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "keyboard.h"

static void init_console(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_write_line("osforsimple: entering long mode kernel");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void kmain(void) {
    init_console();

    pic_remap(0x20, 0x28);
    idt_install();
    keyboard_init();
    timer_init(100);

    __asm__ volatile ("sti");

    vga_write_line("Interrupts enabled. Timer + keyboard active.");

    while (1) {
        __asm__ volatile ("hlt");
    }
}
