#include "keyboard.h"
#include "io.h"
#include "vga.h"
#include <stdint.h>

static const char keymap[] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=',
    '\b','\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'', '`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',
    0,'*',0,' '
};

void keyboard_init(void) {
    /* Nothing to do: interrupts arrive from PIC after idt_install */
}

void keyboard_handle_interrupt(void) {
    uint8_t scancode = inb(0x60);
    if (scancode < sizeof(keymap)) {
        char c = keymap[scancode];
        if (c) {
            char msg[32] = "[kbd] ";
            msg[6] = c;
            msg[7] = '\0';
            vga_write_line(msg);
        }
    }
}
