#include "ata.h"

ATA* ata_init(uint16_t port_base, bool master)
{
    ATA* ata = (ATA*)calloc(sizeof(ATA));

    ata->data = port_base;
    ata->error = port_base + 1;
    ata->sector_count = port_base + 2;
    ata->lba_low = port_base + 3;
    ata->lba_mid = port_base + 4;
    ata->lba_high = port_base + 5;
    ata->device = port_base + 6;
    ata->command = port_base + 7;
    ata->control = port_base + 0x206;
    ata->byte_per_sector = 512;
    ata->master = master;

    return ata;
}

void ata_identify(ATA* ata)
{
    outb(ata->device, ata->master ? 0xa0 : 0xb0);
    outb(ata->control, 0x00);

    outb(ata->device, 0xa0);
    uint8_t status = inb(ata->command);

    if (status == 0xFF)
        return;

    outb(ata->device, ata->master ? 0xa0 : 0xb0);
    outb(ata->sector_count, 0x00);
    outb(ata->lba_low, 0x00);
    outb(ata->lba_mid, 0x00);
    outb(ata->lba_high, 0x00);
    outb(ata->command, 0xec);

    status = inb(ata->command);

    if (status == 0x00) // No drive found
        return;

    while ((status & 0x80) == 0x80 && (status & 0x01) != 0x01)
        status = inb(ata->command);

    if (status & 0x01) // Error while identifying ATA
        return;

    for (uint16_t i = 0; i < 256; i++)
    {
        uint16_t data = inw(ata->data);
    }
}

void ata_read(ATA* ata, uint32_t sector, uint8_t* data, uint16_t count)
{
    if (sector & 0xf0000000)
        return;

    outb(ata->device, (ata->master ? 0xe0 : 0xf0) | (sector & 0x0f00000) >> 24);
    outb(ata->error, 0);
    outb(ata->sector_count, 1);

    outb(ata->lba_low, sector & 0x000000ff);
    outb(ata->lba_mid, (sector & 0x0000ff00) >> 8);
    outb(ata->lba_high, (sector & 0x00ff0000) >> 16);
    outb(ata->command, 0x20);

    uint8_t status = inb(ata->command);
    while ((status & 0x80) == 0x80 && (status & 0x01) != 0x01)
        status = inb(ata->command);

    if (status & 0x01) // Error while reading
        return;

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t rdata = inw(ata->data);

        data[i] = rdata & 0x00ff;
        if (i + 1 < count)
            data[i + 1] = (rdata >> 8) & 0x00ff;
    }

    for (uint16_t i = count + (count % 2); i < 512; i += 2)
    {
        inw(ata->data);
    }
}

void ata_write(ATA* ata, uint32_t sector, uint8_t* data, uint16_t count)
{
    if (sector & 0xf0000000)
        return;

    if (count > 512)
        return;

    outb(ata->device, (ata->master ? 0xe0 : 0xf0) | (sector & 0x0f00000) >> 24);
    outb(ata->error, 0);
    outb(ata->sector_count, 1);

    outb(ata->lba_low, sector & 0x000000ff);
    outb(ata->lba_mid, (sector & 0x0000ff00) >> 8);
    outb(ata->lba_high, (sector & 0x00ff0000) >> 16);
    outb(ata->command, 0x30);

    for (uint16_t i = 0; i < count; i += 2)
    {
        uint16_t wdata = data[i];
        if (i + 1 < count)
            wdata |= ((uint16_t)data[i + 1]) << 8;

        outw(ata->data, wdata);
    }

    for (uint16_t i = count + (count % 2); i < 512; i += 2)
    {
        outw(ata->data, 0x0000);
    }
}

void ata_flush(ATA* ata)
{
    outb(ata->device, ata->master ? 0xe0 : 0xf0);
    outb(ata->command, 0xe7);

    uint8_t status = inb(ata->command);
    while ((status & 0x80) == 0x80 && (status & 0x01) != 0x01)
        status = inb(ata->command);

    if (status & 0x01) // Error while flushing
        return;
}