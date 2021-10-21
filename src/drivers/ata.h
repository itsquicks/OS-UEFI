#pragma once
#include "../defs.h"
#include "../io.h"
#include "../textmode.h"
#include "../heap.h"

struct ATA
{
    uint16_t data;
    uint16_t error;
    uint16_t sector_count;
    uint16_t lba_low;
    uint16_t lba_mid;
    uint16_t lba_high;
    uint16_t device;
    uint16_t command;
    uint16_t control;
    uint16_t byte_per_sector;
    bool master;
};

ATA* ata_init(uint16_t port_base, bool master);

void ata_identify(ATA* ata);
void ata_read(ATA* ata, uint32_t sector, uint8_t* data, uint16_t count);
void ata_write(ATA* ata, uint32_t sector, uint8_t* data, uint16_t count);
void ata_flush(ATA* ata);