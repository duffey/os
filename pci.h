#ifndef PCI_H
#define PCI_H

uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

uint16_t get_vendor_id(uint8_t bus, uint8_t device, uint8_t function);

uint16_t get_device_id(uint8_t bus, uint8_t device, uint8_t function);

uint8_t get_header_type(uint8_t bus, uint8_t device, uint8_t function);

uint32_t get_bar_n(uint8_t bus, uint8_t device, uint8_t function, int n);

#endif
