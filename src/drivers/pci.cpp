#include "pci.h"

uint32_t pci_read(uint16_t bus, uint16_t device, uint16_t func, uint32_t register_offset)
{
	uint32_t id =
		0x1 << 31
		| ((bus & 0xff) << 16)
		| ((device & 0x1f) << 11)
		| ((func & 0x07) << 8)
		| (register_offset & 0xFC);

	outl(COMMAND_PORT, id);
	uint32_t ret = inl(DATA_PORT);
	return ret >> (8 * (register_offset % 4));
}

void pci_write(uint16_t bus, uint16_t device, uint16_t func, uint32_t register_offset, uint32_t value)
{
	uint32_t id =
		0x1 << 31
		| ((bus & 0xff) << 16)
		| ((device & 0x1f) << 11)
		| ((func & 0x07) << 8)
		| (register_offset & 0xFC);

	outl(COMMAND_PORT, id);
	outl(DATA_PORT, value);
}

bool pci_device_has_func(uint16_t bus, uint16_t device)
{
	return pci_read(bus, device, 0, 0x0E) & (1 << 7);
}

PCI_Device pci_get_device(uint16_t bus, uint16_t device, uint16_t func)
{
	PCI_Device ret;

	ret.bus = bus;
	ret.device = device;
	ret.func = func;

	ret.header_type = pci_read(bus, device, func, 0x0E) & ~0x80;

	ret.device_id = pci_read(bus, device, func, 0x02);
	ret.vendor_id = pci_read(bus, device, func, 0x00);

	ret._class = pci_read(bus, device, func, 0x0b);
	ret.subclass = pci_read(bus, device, func, 0x0a);
	ret.prog_if = pci_read(bus, device, func, 0x09);

	return ret;
}

BAR pci_get_bar(int8_t bus, uint16_t device, uint16_t func, uint8_t index)
{
	BAR ret;

	uint32_t bar_value = pci_read(bus, device, func, 0x10 + 4 * index);

	ret.type = (bar_value & 0x01) ? 1 : 0;

	uint32_t temp = bar_value;
	uint64_t size;

	if (ret.type == 0)
	{
		switch ((bar_value >> 1) & 0x03)
		{
		case 0:

			pci_write(bus, device, func, 0x10 + 4 * index, 0xFFFFFFFF);

			size = pci_read(bus, device, func, 0x10 + 4 * index);

			pci_write(bus, device, func, 0x10 + 4 * index, temp);

			size = ~(uint32_t)(size & 0xFFFFFFF0) + 1;
			ret.size = size;

			ret.address = (uint8_t*)(uint64_t)(bar_value & 0xFFFFFFF0);
			break;

		case 1:
			break;

		case 2:

			uint32_t bar_value2 = pci_read(bus, device, func, 0x10 + 4 * (index + 1));

			uint32_t temp2 = bar_value2;
			pci_write(bus, device, func, 0x10 + 4 * index, 0xFFFFFFFF);
			pci_write(bus, device, func, 0x10 + 4 * (index + 1), 0xFFFFFFFF);

			size = pci_read(bus, device, func, 0x10 + 4 * index);
			size += (uint64_t)pci_read(bus, device, func, 0x10 + 4 * (index + 1)) << 32;

			pci_write(bus, device, func, 0x10 + 4 * index, temp);
			pci_write(bus, device, func, 0x10 + 4 * (index + 1), temp2);

			size = ~(size & 0xFFFFFFFFFFFFFFF0) + 1;
			ret.size = size;

			ret.address = (uint8_t*)((bar_value & 0xFFFFFFF0) + ((uint64_t)(bar_value2 & 0xFFFFFFFF) << 32));
			break;
		}

		ret.prefetch = (bar_value >> 3) & 0x01;
	}
	else
	{
		ret.address = (uint8_t*)(uint64_t)(bar_value & 0xFFFFFFFC);
		ret.prefetch = false;
	}

	return ret;
}

void select_drivers()
{
	for (uint16_t bus = 0; bus < 256; bus++)
	{
		for (uint8_t device = 0; device < 32; device++)
		{
			uint8_t func_count = pci_device_has_func(bus, device) ? 8 : 1;

			for (uint8_t func = 0; func < func_count; func++)
			{
				PCI_Device dev = pci_get_device(bus, device, func);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xffff)
					continue;

				/*uint8_t num = 0;

				if (dev.header_type == 0x00)
					num = 6;
				else if (dev.header_type == 0x01)
					num = 2;

				for (uint8_t i = 0; i < num; i++)
				{
					BAR bar = pci_get_bar(bus, device, func, i);
					dev.portBase = (uint64_t)bar.address;

					if (dev.portBase != 0)
					{
						GetDriver(dev);
					}
				}*/

				//GetDriver(dev);
			}
		}
	}
}

void print_devices()
{
	for (uint16_t bus = 0; bus < 256; bus++)
	{
		for (uint8_t device = 0; device < 32; device++)
		{
			uint8_t func_count = pci_device_has_func(bus, device) ? 8 : 1;

			for (uint8_t func = 0; func < func_count; func++)
			{
				PCI_Device dev = pci_get_device(bus, device, func);

				if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xffff)
					continue;

				/*printf("%x.%x.%x   %x:%x   ", (uint8_t)dev.bus, (uint8_t)dev.device, (uint8_t)dev.func, dev.vendor_id, dev.device_id);

				if (dev._class == 0 && dev.subclass == 0)
					printf("Unclassified - Non-VGA-Compatible Device\r\n");

				else if (dev._class == 0 && dev.subclass == 1)
					printf("Unclassified - VGA-Compatible Device\r\n");

				else if (dev._class == 1 && dev.subclass == 0)
					printf("Mass Storage Controller - SCSI Bus Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 1)
					printf("Mass Storage Controller - IDE Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 2)
					printf("Mass Storage Controller - Floppy Disk Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 3)
					printf("Mass Storage Controller - IPI Bus Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 4)
					printf("Mass Storage Controller - RAID Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 5)
					printf("Mass Storage Controller - ATA Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 6)
					printf("Mass Storage Controller - Serial ATA\r\n");

				else if (dev._class == 1 && dev.subclass == 7)
					printf("Mass Storage Controller - Serial Attached SCSI\r\n");

				else if (dev._class == 1 && dev.subclass == 8)
					printf("Mass Storage Controller - Non-Volatile Memory Controller\r\n");

				else if (dev._class == 1 && dev.subclass == 0x80)
					printf("Mass Storage Controller - Other\r\n");

				else if (dev._class == 2 && dev.subclass == 0)
					printf("Network Controller - Ethernet Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 1)
					printf("Network Controller - Token Ring Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 2)
					printf("Network Controller - FDDI Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 3)
					printf("Network Controller - ATM Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 4)
					printf("Network Controller - ISDN Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 5)
					printf("Network Controller - WorldFip Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 6)
					printf("Network Controller - PICMG 2.14 Multi Computing\r\n");

				else if (dev._class == 2 && dev.subclass == 7)
					printf("Network Controller - Infiniband Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 8)
					printf("Network Controller - Fabric Controller\r\n");

				else if (dev._class == 2 && dev.subclass == 0x80)
					printf("Network Controller - Other\r\n");

				else if (dev._class == 3 && dev.subclass == 0)
					printf("Display Controller - VGA Compatible Controller\r\n");

				else if (dev._class == 3 && dev.subclass == 1)
					printf("Display Controller - XGA Controller\r\n");

				else if (dev._class == 3 && dev.subclass == 2)
					printf("Display Controller - 3D Controller (Not VGA-Compatible)\r\n");

				else if (dev._class == 3 && dev.subclass == 0x80)
					printf("Display Controller - Other\r\n");

				else if (dev._class == 4 && dev.subclass == 0)
					printf("Multimedia Controller - Multimedia Video Controller\r\n");

				else if (dev._class == 4 && dev.subclass == 1)
					printf("Multimedia Controller - Multimedia Video Controller\r\n");

				else if (dev._class == 4 && dev.subclass == 2)
					printf("Multimedia Controller - Multimedia Audio Controller\r\n");

				else if (dev._class == 4 && dev.subclass == 3)
					printf("Multimedia Controller - Audio Device\r\n");

				else if (dev._class == 4 && dev.subclass == 0x80)
					printf("Multimedia Controller - Other\r\n");

				else if (dev._class == 5 && dev.subclass == 0)
					printf("Memory Controller - RAM Controller\r\n");

				else if (dev._class == 5 && dev.subclass == 1)
					printf("Memory Controller - Flash Controller\r\n");

				else if (dev._class == 5 && dev.subclass == 0x80)
					printf("Memory Controller - Other\r\n");

				else if (dev._class == 6 && dev.subclass == 0)
					printf("Bridge Device - Host Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 1)
					printf("Bridge Device - ISA Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 2)
					printf("Bridge Device - EISA Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 3)
					printf("Bridge Device - MCA Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 4)
					printf("Bridge Device - PCI-to-PCI Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 5)
					printf("Bridge Device - PCMCIA Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 6)
					printf("Bridge Device - NuBus Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 7)
					printf("Bridge Device - CardBus Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 8)
					printf("Bridge Device - RACEway Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 9)
					printf("Bridge Device - PCI-to-PCI Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 0x0a)
					printf("Bridge Device - InfiniBand-to-PCI Host Bridge\r\n");

				else if (dev._class == 6 && dev.subclass == 0x80)
					printf("Bridge Device - Other\r\n");

				else if (dev._class == 7 && dev.subclass == 0)
					printf("Simple Communication Controller - Serial Controller\r\n");

				else if (dev._class == 7 && dev.subclass == 1)
					printf("Simple Communication Controller - Parallel Controller\r\n");

				else if (dev._class == 7 && dev.subclass == 2)
					printf("Simple Communication Controller - Multiport Serial Controller\r\n");

				else if (dev._class == 7 && dev.subclass == 3)
					printf("Simple Communication Controller - Modem\r\n");

				else if (dev._class == 7 && dev.subclass == 4)
					printf("Simple Communication Controller - IEEE 488.1/2 (GPIB) Controller\r\n");

				else if (dev._class == 7 && dev.subclass == 5)
					printf("Simple Communication Controller - Smart Card\r\n");

				else if (dev._class == 7 && dev.subclass == 0x80)
					printf("Simple Communication Controller - Other\r\n");

				else if (dev._class == 8 && dev.subclass == 0)
					printf("Base System Peripheral - PIC\r\n");

				else if (dev._class == 8 && dev.subclass == 1)
					printf("Base System Peripheral - DMA Controller\r\n");

				else if (dev._class == 8 && dev.subclass == 2)
					printf("Base System Peripheral - Timer\r\n");

				else if (dev._class == 8 && dev.subclass == 3)
					printf("Base System Peripheral - RTC Controller\r\n");

				else if (dev._class == 8 && dev.subclass == 4)
					printf("Base System Peripheral - PCI Hot-Plug Controller\r\n");

				else if (dev._class == 8 && dev.subclass == 5)
					printf("Base System Peripheral - SD Host controller\r\n");

				else if (dev._class == 8 && dev.subclass == 6)
					printf("Base System Peripheral - IOMMU\r\n");

				else if (dev._class == 8 && dev.subclass == 0x80)
					printf("Base System Peripheral - Other\r\n");

				else if (dev._class == 9 && dev.subclass == 0)
					printf("Input Device Controller - Keyboard Controller\r\n");

				else if (dev._class == 9 && dev.subclass == 1)
					printf("Input Device Controller - Digitizer Pen\r\n");

				else if (dev._class == 9 && dev.subclass == 2)
					printf("Input Device Controller - Mouse Controller\r\n");

				else if (dev._class == 9 && dev.subclass == 3)
					printf("Input Device Controller - Scanner Controller\r\n");

				else if (dev._class == 9 && dev.subclass == 4)
					printf("Input Device Controller - Gameport Controller\r\n");

				else if (dev._class == 9 && dev.subclass == 0x80)
					printf("Input Device Controller - Other\r\n");

				else if (dev._class == 0x0a && dev.subclass == 0x0)
					printf("Docking Station - Generic\r\n");

				else if (dev._class == 0x0a && dev.subclass == 0x80)
					printf("Docking Station - Other\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0)
					printf("Processor - 386\r\n");

				else if (dev._class == 0x0b && dev.subclass == 1)
					printf("Processor - 486\r\n");

				else if (dev._class == 0x0b && dev.subclass == 2)
					printf("Processor - Pentium\r\n");

				else if (dev._class == 0x0b && dev.subclass == 3)
					printf("Processor - Pentium Pro\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0x10)
					printf("Processor - Alpha\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0x20)
					printf("Processor - PowerPC\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0x30)
					printf("Processor - MIPS\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0x40)
					printf("Processor - Co-Processor\r\n");

				else if (dev._class == 0x0b && dev.subclass == 0x80)
					printf("Processor - Other\r\n");

				else if (dev._class == 0x0c && dev.subclass == 0)
					printf("Serial Bus Controller - FireWire (IEEE 1394) Controller\r\n");

				else if (dev._class == 0x0c && dev.subclass == 1)
					printf("Serial Bus Controller - ACCESS Bus\r\n");

				else if (dev._class == 0x0c && dev.subclass == 2)
					printf("Serial Bus Controller - SSA\r\n");

				else if (dev._class == 0x0c && dev.subclass == 3)
					printf("Serial Bus Controller - USB Controller\r\n");

				else if (dev._class == 0x0c && dev.subclass == 4)
					printf("Serial Bus Controller - Fibre Channel\r\n");

				else if (dev._class == 0x0c && dev.subclass == 5)
					printf("Serial Bus Controller - SMBus\r\n");

				else if (dev._class == 0x0c && dev.subclass == 6)
					printf("Serial Bus Controller - InfiniBand\r\n");

				else if (dev._class == 0x0c && dev.subclass == 7)
					printf("Serial Bus Controller - IPMI Interface\r\n");

				else if (dev._class == 0x0c && dev.subclass == 8)
					printf("Serial Bus Controller - SERCOS Interface (IEC 61491)\r\n");

				else if (dev._class == 0x0c && dev.subclass == 9)
					printf("Serial Bus Controller - CANbus\r\n");

				else if (dev._class == 0x0c && dev.subclass == 0x80)
					printf("Serial Bus Controller - Other\r\n");

				else if (dev._class == 0x0d)
					printf("Wireless Controller\r\n");

				else if (dev._class == 0x0e)
					printf("Intelligent Controller\r\n");

				else if (dev._class == 0x0f)
					printf("Satellite Communication Controller\r\n");

				else if (dev._class == 0x10)
					printf("Encryption Controller\r\n");

				else if (dev._class == 0x11)
					printf("Signal Processing Controller\r\n");

				else if (dev._class == 0x12)
					printf("Processing Accelerator\r\n");

				else if (dev._class == 0x13)
					printf("Non-Essential Instrumentation\r\n");

				else if (dev._class == 0x14)
					printf("0x3F (Reserved)\r\n");

				else if (dev._class == 0x40)
					printf("Co-Processor\r\n");

				else if (dev._class == 0x41)
					printf("0xFE (Reserved)\r\n");*/
			}
		}
	}
}