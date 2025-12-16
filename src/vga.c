#include "vga.h"
#include <stdbool.h>

static uint16_t *const VGA_MEMORY = (uint16_t *)0xB8000;
static size_t cursor_row = 0;
static size_t cursor_col = 0;
static uint8_t current_color = 0;

static inline uint8_t make_color(vga_color fg, vga_color bg) {
    return fg | bg << 4;
}

static inline uint16_t make_vga_entry(char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

static void move_cursor(void) {
    uint16_t pos = (uint16_t)(cursor_row * VGA_WIDTH + cursor_col);
    const uint16_t command_port = 0x3D4;
    const uint16_t data_port = 0x3D5;
    __asm__ volatile ("outb %0, %1" : : "a"((uint8_t)(pos >> 8)), "Nd"(command_port));
    __asm__ volatile ("outb %0, %1" : : "a"((uint8_t)pos), "Nd"(data_port));
}

void vga_set_color(vga_color fg, vga_color bg) {
    current_color = make_color(fg, bg);
}

void vga_clear(void) {
    current_color = make_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t idx = y * VGA_WIDTH + x;
            VGA_MEMORY[idx] = make_vga_entry(' ', current_color);
        }
    }
    cursor_row = cursor_col = 0;
    move_cursor();
}

static void newline(void) {
    cursor_col = 0;
    if (++cursor_row >= VGA_HEIGHT) {
        for (size_t y = 1; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
            }
        }
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = make_vga_entry(' ', current_color);
        }
        cursor_row = VGA_HEIGHT - 1;
    }
}

static void put_char(char c) {
    if (c == '\n') {
        newline();
        move_cursor();
        return;
    }
    VGA_MEMORY[cursor_row * VGA_WIDTH + cursor_col] = make_vga_entry(c, current_color);
    if (++cursor_col >= VGA_WIDTH) {
        newline();
    }
    move_cursor();
}

void vga_write(const char *str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        put_char(str[i]);
    }
}

void vga_write_line(const char *str) {
    vga_write(str);
    put_char('\n');
}
