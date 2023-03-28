#include "isr.h"
#include "common.h"
#include "terminal.h"

isr_t interrupt_handlers[256];

void register_interrupt_handler(int n, isr_t handler)
{
    interrupt_handlers[n] = handler;
}

// This gets called from our ASM interrupt handler stub.
void isr_handler(struct registers *regs)
{
    terminal_write("Received interrupt: ");
    terminal_write_dec(regs->int_no);
    terminal_write(" Error code: ");
    terminal_write_dec(regs->err_code);
    terminal_put('\n');

    if (regs->int_no < 32) {
        hcf();
    }

    if (interrupt_handlers[regs->int_no] != 0) {
        isr_t handler = interrupt_handlers[regs->int_no];
        handler(regs);
    }

    // IRQs
    if (regs->int_no >= 32 && regs->int_no <= 47) {
        // Send an EOI (end of interrupt) signal to the PICs.
        // If this interrupt involved the slave.
        if (regs->int_no >= 40) {
            // Send reset signal to slave.
            outb(0xA0, 0x20);
        }
        // Send reset signal to master. (As well as slave, if necessary).
        outb(0x20, 0x20);
    }
}