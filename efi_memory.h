#pragma once
#include "../defs.h"
#include "../textmode.h"
#include "../utils.h"

struct EFI_Descriptor
{
    uint32_t type;
    void* physical_address;
    void* virtual_address;
    uint64_t num_pages;
    uint64_t attributes;
};
