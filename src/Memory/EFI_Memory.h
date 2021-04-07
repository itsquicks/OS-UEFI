#pragma once
#include "../Typedefs.h"
#include "../TextMode.h"
#include "../kconvert.h"

struct EFI_MEMORY_DESCRIPTOR
{
    uint32 type;
    void* physicalAddress;
    void* virtualAddress;
    uint64 numPages;
    uint64 attributes;
};

extern const char* EFI_MEMORY_TYPE_STRINGS[];