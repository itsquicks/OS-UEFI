#pragma once
#include "defs.h"
#include "drivers/ata.h"

extern struct ATA* hd;

void disk_init();

void disk_read(void* dest, uint32_t start, uint32_t bytes);
void disk_write(void* src, uint32_t start, uint32_t bytes);