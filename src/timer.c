#include "timer.h"
#include "io.h"
#include "vga.h"
#include <stddef.h>

static uint64_t tick_count = 0;

void timer_init(uint32_t frequency) {
    const uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void timer_tick(void) {
    tick_count++;
    if (tick_count % 100 == 0) {
        vga_write_line("[timer] 100 ticks elapsed");
    }
}

uint64_t timer_ticks(void) {
    return tick_count;
}
