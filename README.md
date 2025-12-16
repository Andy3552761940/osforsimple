# osforsimple
一个极简的 x86_64 教学内核实现，使用 Multiboot2 从 GRUB 启动。启动后会进入长模式，初始化 VGA 文本输出、可编程中断控制器 (PIC)、可编程定时器 (PIT) 以及 PS/2 键盘中断处理。

## 目录结构
- `src/boot.asm`：Multiboot2 头、GDT、页表以及切换到 64 位长模式的入口。
- `src/interrupts.asm`：定时器和键盘的中断服务例程桩代码。
- `src/*.c`：内核主体代码（VGA 输出、IDT、PIC、定时器、键盘、入口等）。
- `linker.ld`：将内核放置在 1MiB 处的链接脚本。
- `Makefile`：构建、生成 ISO、运行 QEMU 的辅助命令。

## 构建与运行
该项目使用主机默认的 x86_64 工具链，无需交叉编译器。需要 `nasm`、`gcc`、`ld`，可选 `qemu-system-x86_64` 与 `grub-mkrescue`。

```bash
make            # 生成 kernel.elf
make run        # 使用 QEMU 直接启动内核（需要 qemu-system-x86_64）
make iso        # 构建带 GRUB 的可启动 ISO（需要 grub-mkrescue/xorriso）
```

内核启动后会在屏幕输出启动提示，并启用中断。每 100 个时钟节拍会打印提示，键盘按键也会被回显到屏幕。
