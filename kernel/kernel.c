#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "common.h"
#include "isr.h"
#include "idt.h"

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent.
static volatile struct limine_terminal_request terminal_request = {
    .id = LIMINE_TERMINAL_REQUEST,
    .revision = 0
};

static void breakpoint_handler(__attribute__((unused)) struct registers *registers)
{
    const char *hello_msg = "Hello Interrupt\n";
    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, hello_msg, strlen(hello_msg));
}

// Halt and catch fire function.
static void hcf(void) {
    asm ("cli");
    for (;;) {
        asm ("hlt");
    }
}

// The following will be our kernel's entry point.
// If renaming _start() to something else, make sure to change the
// linker script accordingly.
void _start(void) {
    // Ensure we got a terminal
    if (terminal_request.response == NULL
     || terminal_request.response->terminal_count < 1) {
        hcf();
    }

    // We should now be able to call the Limine terminal to print out
    // a simple "Hello World" to screen.
    const char *hello_msg = "Hello World\n";

    struct limine_terminal *terminal = terminal_request.response->terminals[0];
    terminal_request.response->write(terminal, hello_msg, strlen(hello_msg));

    init_idt();
    register_interrupt_handler(3, breakpoint_handler);
    asm volatile("int $0x3");
    for (;;) {}

    // We're done, just hang...
    hcf();
}
