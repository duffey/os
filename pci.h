#ifndef PCI_H
#define PCI_H

uint16_t pci_get_vendor_id(uint8_t bus, uint8_t slot);
const char *pci_get_vendor_name(uint16_t vendor_id);

#endif