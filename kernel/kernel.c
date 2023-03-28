#include "common.h"
#include "idt.h"
#include "isr.h"
#include "terminal.h"

static void interrupt_handler(__attribute__((unused)) struct registers *registers)
{
    const char *hello_msg = "Hello Interrupt\n";
    terminal_write(hello_msg);
}

void _start(void)
{
    init_terminal();
    const char *hello_msg = "Hello World\n";
    terminal_write(hello_msg);

    init_idt();
    register_interrupt_handler(0xFE, interrupt_handler);
    asm volatile("int $0xFE");
    for (;;) {}

    // We're done, just hang...
    hcf();
}
