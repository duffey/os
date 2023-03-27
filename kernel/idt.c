#include "common.h"
#include "idt.h"
#include "isr.h"

extern void idt_flush(uint64_t);

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr;

// Extern the ISR handler array so we can nullify them on startup.
extern isr_t interrupt_handlers[];

static void idt_set_gate(uint8_t num, uint64_t offset, uint16_t selector, uint8_t type_attributes)
{
    idt_entries[num].offset_1 = offset & 0xFFFF;
    idt_entries[num].offset_2 = (offset >> 16) & 0xFFFF;
    idt_entries[num].offset_3 = (offset >> 32) & 0xFFFFFFFF;

    idt_entries[num].selector = selector;
    idt_entries[num].ist = 0;
    idt_entries[num].type_attributes = type_attributes;
    idt_entries[num].zero = 0;
}

void init_idt(void)
{
    idt_ptr.size = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.offset = (uint64_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);
    memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);

    // Remap the irq table.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    idt_set_gate( 0, (uint64_t) isr0 , 0x28, 0x8E);
    idt_set_gate( 1, (uint64_t) isr1 , 0x28, 0x8E);
    idt_set_gate( 2, (uint64_t) isr2 , 0x28, 0x8E);
    idt_set_gate( 3, (uint64_t) isr3 , 0x28, 0x8E);
    idt_set_gate( 4, (uint64_t) isr4 , 0x28, 0x8E);
    idt_set_gate( 5, (uint64_t) isr5 , 0x28, 0x8E);
    idt_set_gate( 6, (uint64_t) isr6 , 0x28, 0x8E);
    idt_set_gate( 7, (uint64_t) isr7 , 0x28, 0x8E);
    idt_set_gate( 8, (uint64_t) isr8 , 0x28, 0x8E);
    idt_set_gate( 9, (uint64_t) isr9 , 0x28, 0x8E);
    idt_set_gate(10, (uint64_t) isr10, 0x28, 0x8E);
    idt_set_gate(11, (uint64_t) isr11, 0x28, 0x8E);
    idt_set_gate(12, (uint64_t) isr12, 0x28, 0x8E);
    idt_set_gate(13, (uint64_t) isr13, 0x28, 0x8E);
    idt_set_gate(14, (uint64_t) isr14, 0x28, 0x8E);
    idt_set_gate(15, (uint64_t) isr15, 0x28, 0x8E);
    idt_set_gate(16, (uint64_t) isr16, 0x28, 0x8E);
    idt_set_gate(17, (uint64_t) isr17, 0x28, 0x8E);
    idt_set_gate(18, (uint64_t) isr18, 0x28, 0x8E);
    idt_set_gate(19, (uint64_t) isr19, 0x28, 0x8E);
    idt_set_gate(20, (uint64_t) isr20, 0x28, 0x8E);
    idt_set_gate(21, (uint64_t) isr21, 0x28, 0x8E);
    idt_set_gate(22, (uint64_t) isr22, 0x28, 0x8E);
    idt_set_gate(23, (uint64_t) isr23, 0x28, 0x8E);
    idt_set_gate(24, (uint64_t) isr24, 0x28, 0x8E);
    idt_set_gate(25, (uint64_t) isr25, 0x28, 0x8E);
    idt_set_gate(26, (uint64_t) isr26, 0x28, 0x8E);
    idt_set_gate(27, (uint64_t) isr27, 0x28, 0x8E);
    idt_set_gate(28, (uint64_t) isr28, 0x28, 0x8E);
    idt_set_gate(29, (uint64_t) isr29, 0x28, 0x8E);
    idt_set_gate(30, (uint64_t) isr30, 0x28, 0x8E);
    idt_set_gate(31, (uint64_t) isr31, 0x28, 0x8E);
    idt_set_gate(32, (uint64_t) irq0, 0x28, 0x8E);
    idt_set_gate(33, (uint64_t) irq1, 0x28, 0x8E);
    idt_set_gate(34, (uint64_t) irq2, 0x28, 0x8E);
    idt_set_gate(35, (uint64_t) irq3, 0x28, 0x8E);
    idt_set_gate(36, (uint64_t) irq4, 0x28, 0x8E);
    idt_set_gate(37, (uint64_t) irq5, 0x28, 0x8E);
    idt_set_gate(38, (uint64_t) irq6, 0x28, 0x8E);
    idt_set_gate(39, (uint64_t) irq7, 0x28, 0x8E);
    idt_set_gate(40, (uint64_t) irq8, 0x28, 0x8E);
    idt_set_gate(41, (uint64_t) irq9, 0x28, 0x8E);
    idt_set_gate(42, (uint64_t) irq10, 0x28, 0x8E);
    idt_set_gate(43, (uint64_t) irq11, 0x28, 0x8E);
    idt_set_gate(44, (uint64_t) irq12, 0x28, 0x8E);
    idt_set_gate(45, (uint64_t) irq13, 0x28, 0x8E);
    idt_set_gate(46, (uint64_t) irq14, 0x28, 0x8E);
    idt_set_gate(47, (uint64_t) irq15, 0x28, 0x8E);

    idt_flush((uint64_t) &idt_ptr);
}