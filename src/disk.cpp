#include "disk.h"

ATA* hd;

void disk_init()
{
    hd = ata_init(0x1f0, false);
}

void disk_read(void* dest, uint32_t start, uint32_t bytes)
{
    uint32_t sectors = bytes / 512;
    uint32_t remainder = bytes % 512;

    int i = 0;

    for (;i < sectors;i++)
        ata_read(hd, start + i, (uint8_t*)dest + i * 512, 512);

    if (remainder != 0)
        ata_read(hd, start + i, (uint8_t*)dest + i * 512, remainder);
}

void disk_write(void* src, uint32_t start, uint32_t bytes)
{
    uint32_t sectors = bytes / 512;
    uint32_t remainder = bytes % 512;

    int i = 0;

    for (;i < sectors;i++)
    {
        ata_write(hd, start + i, (uint8_t*)src + i * 512, 512);
        ata_flush(hd);
    }

    if (remainder != 0)
    {
        ata_write(hd, start + i, (uint8_t*)src + i * 512, remainder);
        ata_flush(hd);
    }
}