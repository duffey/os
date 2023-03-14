#include <stdint.h>

static uint32_t inl(uint32_t port)
{
	uint32_t value;
	__asm__ __volatile__("inl %%dx, %%eax":"=a"(value):"d"(port));
	return value;
}

static void outl(uint32_t port, uint32_t value)
{
	__asm__ __volatile__("outl %%eax, %%dx"::"d"(port), "a"(value));
}

uint32_t pci_read_register(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus = (uint32_t) bus;
	uint32_t ldevice = (uint32_t) device;
	uint32_t lfunction = (uint32_t) function;

	address = (uint32_t) ((lbus << 16) | (ldevice << 11) | (lfunction << 8) | (offset & 0xFC) | ((uint32_t) 0x80000000));

	outl(0xCF8, address);
	return inl(0xCFC);
}

uint16_t pci_read_word(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset)
{
	uint32_t reg;

	reg = pci_read_register(bus, device, function, offset);
	return (uint16_t) ((reg >> ((offset & 2) * 8)) & 0xFFFF);
}

uint16_t get_vendor_id(uint8_t bus, uint8_t device, uint8_t function)
{
	return pci_read_word(bus, device, function, 0);
}

uint16_t get_device_id(uint8_t bus, uint8_t device, uint8_t function)
{
	return pci_read_word(bus, device, function, 2);
}

uint8_t get_command(uint8_t bus, uint8_t device, uint8_t function)
{
	return pci_read_word(bus, device, function, 4);
}

uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t) pci_read_word(bus, device, function, 14);
}

uint32_t get_bar_n(uint8_t bus, uint8_t device, uint8_t function, int n)
{
	return pci_read_register(bus, device, function, 16 + n * 4);

}

int get_bar_n_type(uint8_t bus, uint8_t device, uint8_t function, int n)
{
	uint32_t bar = get_bar_n(bus, device, function, n);
	return (bar >> 1) & ~1;
}
