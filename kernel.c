#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>

#include "./include/nanoprintf/nanoprintf.h"
#include "pci.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

/* Hardware text mode color constants. */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char *str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer;

void terminal_initialize(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	terminal_buffer = (uint16_t *) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color)
{
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void scroll_terminal_one_line()
{
	for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t to = y * VGA_WIDTH + x;
			const size_t from = (y + 1) * VGA_WIDTH + x;
			terminal_buffer[to] = terminal_buffer[from];
		}
	}
	for (size_t x = 0; x < VGA_WIDTH; x++) {
		const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
		terminal_buffer[index] = vga_entry(' ', terminal_color);
	}
}

void terminal_putchar(char c)
{
	if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
		if (terminal_row == VGA_HEIGHT) {
			scroll_terminal_one_line();
			terminal_row--;
		}
	} else {
		terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
		if (++terminal_column == VGA_WIDTH) {
			terminal_column = 0;
			if (++terminal_row == VGA_HEIGHT) {
				scroll_terminal_one_line();
				terminal_row--;
			}
		}
	}
}

void terminal_write(const char *data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i]);
}

void terminal_writestring(const char *data)
{
	terminal_write(data, strlen(data));
}

void terminal_printf(const char *fmt, ...)
{
	char buffer[1024];
	va_list val;
	va_start(val, fmt);
	npf_vsnprintf(buffer, 1024, fmt, val);
	va_end(val);
	terminal_writestring(buffer);
}

uint32_t eeprom_read(volatile uint32_t * eeprom_address, uint8_t address)
{
	uint32_t value = 0;

	*eeprom_address = 1 | (((uint32_t) address) << 8);
	do {
		value = *eeprom_address;
	} while (!(value & (1 << 4)));
	return (uint16_t) ((value >> 16) & 0xFFFF);
}

void read_mac_address(volatile uint32_t * eeprom_address, uint8_t * mac_address)
{
	uint32_t value;
	value = eeprom_read(eeprom_address, 0);
	mac_address[0] = value & 0xFF;
	mac_address[1] = value >> 8;
	value = eeprom_read(eeprom_address, 1);
	mac_address[2] = value & 0xFF;
	mac_address[3] = value >> 8;
	value = eeprom_read(eeprom_address, 2);
	mac_address[4] = value & 0xFF;
	mac_address[5] = value >> 8;
}

struct idt_entry_t {
	uint16_t isr_low;	// The lower 16 bits of the ISR's address
	uint16_t kernel_cs;	// The GDT segment selector that the CPU will load into CS before calling the ISR
	uint8_t reserved;	// Set to zero
	uint8_t attributes;	// Type and attributes; see the IDT page
	uint16_t isr_high;	// The higher 16 bits of the ISR's address
} __attribute__((packed));

struct idtr_t {
	uint16_t limit;
	uint32_t base;
} __attribute__((packed));

__attribute__((aligned(0x10)))
static struct idt_entry_t idt[256];	// Create an array of IDT entries; aligned for performance

static struct idtr_t idtr;

void exception_handler(void);
void exception_handler()
{
	terminal_writestring("hello idt");
	__asm__ volatile ("cli; hlt");	// Completely hangs the computer
}

void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags)
{
	struct idt_entry_t *descriptor = &idt[vector];

	descriptor->isr_low = (uint32_t) isr & 0xFFFF;
	descriptor->kernel_cs = 0x08;	// this value can be whatever offset your kernel code selector is in your GDT
	descriptor->attributes = flags;
	descriptor->isr_high = (uint32_t) isr >> 16;
	descriptor->reserved = 0;
}

extern void *isr_stub_table[];

void idt_init(void);
void idt_init()
{
	idtr.base = (uintptr_t) & idt[0];
	idtr.limit = (uint16_t) sizeof(idt) - 1;

	for (uint8_t vector = 0; vector < 32; vector++) {
		idt_set_descriptor(vector, isr_stub_table[vector], 0x8E);
		//vectors[vector] = true;
	}

	__asm__ volatile ("lidt %0"::"m" (idtr));	// load the new IDT
	__asm__ volatile ("sti");	// set the interrupt flag
}

void encode_gdt_entry(uint8_t * target, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
	// Encode the limit
	target[0] = limit & 0xFF;
	target[1] = (limit >> 8) & 0xFF;
	target[6] = (limit >> 16) & 0x0F;

	// Encode the base
	target[2] = base & 0xFF;
	target[3] = (base >> 8) & 0xFF;
	target[4] = (base >> 16) & 0xFF;
	target[7] = (base >> 24) & 0xFF;

	// Encode the access byte
	target[5] = access;

	// Encode the flags
	target[6] |= (flags << 4);
}

void setGdt(uint32_t, uint32_t);

void kernel_main(void)
{
	uint8_t gdt[8 * 3];
	encode_gdt_entry(gdt, 0, 0, 0, 0);
	encode_gdt_entry(gdt + 8, 0, 0xFFFFF, 0x9A, 0xC);
	encode_gdt_entry(gdt + 16, 0, 0xFFFFF, 0x92, 0xC);

	setGdt(sizeof(gdt) - 1, (uint32_t) gdt);
	idt_init();

	/* Initialize terminal interface */
	terminal_initialize();

	__asm__("int $0");

	/*
	   uint16_t bus;
	   uint8_t device;
	   uint8_t function = 0;

	   for (bus = 0; bus < 256; bus++) {
	   for (device = 0; device < 32; device++) {
	   uint16_t vendor_id = get_vendor_id(bus, device, function);

	   if (vendor_id == 0xFFFF)
	   continue;

	   uint16_t device_id = get_device_id(bus, device, function);
	   uint16_t command = get_command(bus, device, function);
	   uint8_t header_type = get_header_type(bus, device, function);

	   terminal_printf("bus: %d device %d %04X %04X %04X %02X", bus, device, vendor_id, device_id, command, header_type);
	   terminal_writestring("\n");

	   if (header_type != 0x00)
	   continue;

	   for (int i = 0; i < 6; i++) {
	   uint32_t bar = get_bar_n(bus, device, function, i);

	   terminal_printf("    %08X\n", bar);
	   }
	   }
	   }

	   uint32_t memory_base_address = get_bar_n(0, 3, 0, 0) & ~3;
	   volatile uint32_t *eeprom_address = (uint32_t *) (memory_base_address + 0x0014);

	   terminal_printf("Memory Base Address: %08X\n", memory_base_address);
	   terminal_printf("EEPROM: %p\n", eeprom_address);

	   uint8_t mac_address[6];
	   read_mac_address(eeprom_address, mac_address);
	   terminal_printf("MAC address %02X:%02X:%02X:%02X:%02X:%02X\n", mac_address[0], mac_address[1], mac_address[2], mac_address[3], mac_address[4], mac_address[5]);

	   volatile uint32_t *mta_address = (uint32_t *) (memory_base_address + 0x5200);
	   for (int i = 0; i < 0x80; i++)
	   *mta_address = 0;
	 */
}
