#pragma once
#include "../defs.h"
#include "../io.h"
#include "../textmode.h"

#define DATA_PORT 0xCFC
#define COMMAND_PORT 0xCF8

struct BAR
{
	bool prefetch;
	uint8_t* address;
	uint64_t size;
	uint8_t type;
};

struct PCI_Device
{
	uint16_t bus;
	uint16_t device;
	uint16_t func;

	uint64_t port_base;
	uint8_t header_type;

	uint16_t device_id;
	uint16_t vendor_id;

	uint8_t _class;
	uint8_t subclass;
	uint8_t prog_if;
};

uint32_t pci_read(uint16_t bus, uint16_t device, uint16_t func, uint32_t register_offset);
void pci_write(uint16_t bus, uint16_t device, uint16_t func, uint32_t register_offset, uint32_t value);
bool pci_device_has_func(uint16_t bus, uint16_t device);

PCI_Device pci_get_device(uint16_t bus, uint16_t device, uint16_t func);
BAR pci_get_bar(uint16_t bus, uint16_t device, uint16_t func, uint8_t index);

void select_drivers();

void print_devices();