#pragma once
#include "../Typedefs.h"
#include "../IO.h"
#include "../TextMode.h"

#define DATA_PORT 0xCFC
#define COMMAND_PORT 0xCF8

struct BAR
{
	bool prefetch;
	uint8* address;
	uint64 size;
	uint8 type;
};

struct PCI_Device
{
	uint16 bus;
	uint16 device;
	uint16 func;

	uint64 portBase;
	uint8 headerType;

	uint16 deviceID;
	uint16 vendorID;

	uint8 _class;
	uint8 subclass;
	uint8 progIF;
};

uint32 PCI_Read(uint16 bus, uint16 device, uint16 func, uint32 registerOffset);
void PCI_Write(uint16 bus, uint16 device, uint16 func, uint32 registerOffset, uint32 value);
bool DeviceHasFunc(uint16 bus, uint16 device);

PCI_Device PCI_GetDeviceDescriptor(uint16 bus, uint16 device, uint16 func);
BAR PCI_GetBaseAddressRegister(uint16 bus, uint16 device, uint16 func, uint8 index);

void SelectDrivers();

void PrintDevices();