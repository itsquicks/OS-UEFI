#include "Paging.h"

PageTable* PML4;

void InitializePaging()
{
    PML4 = (PageTable*)RequestPage();
    memset(PML4, 0, 4096);
}

void MapMemory(void* virtualAddress, void* physicalAddress)
{
    uint64 address = (uint64)virtualAddress;

    address >>= 12;
    uint64 P_i = address & 0x1ff;
    address >>= 9;
    uint64 PT_i = address & 0x1ff;
    address >>= 9;
    uint64 PD_i = address & 0x1ff;
    address >>= 9;
    uint64 PDP_i = address & 0x1ff;

    PageDirectoryEntry PDE;

    PDE = PML4->entries[PDP_i];
    PageTable* PDP;
    if (PDE.GetFlag(PRESENT))
    {
        PDP = (PageTable*)((uint64)PDE.GetAddress() << 12);
    }
    else
    {
        PDP = (PageTable*)RequestPage();
        memset(PDP, 0, 4096);
        PDE.SetAddress((uint64)PDP >> 12);
        PDE.SetFlag(PRESENT, true);
        PDE.SetFlag(READ_WRITE, true);
        PML4->entries[PDP_i] = PDE;
    }

    PDE = PDP->entries[PD_i];
    PageTable* PD;
    if (PDE.GetFlag(PRESENT))
    {
        PD = (PageTable*)((uint64)PDE.GetAddress() << 12);
    }
    else
    {
        PD = (PageTable*)RequestPage();
        memset(PD, 0, 4096);
        PDE.SetAddress((uint64)PD >> 12);
        PDE.SetFlag(PRESENT, true);
        PDE.SetFlag(READ_WRITE, true);
        PDP->entries[PD_i] = PDE;
    }

    PDE = PD->entries[PT_i];
    PageTable* PT;
    if (PDE.GetFlag(PRESENT))
    {
        PT = (PageTable*)((uint64)PDE.GetAddress() << 12);
    }
    else
    {
        PT = (PageTable*)RequestPage();
        memset(PT, 0, 4096);
        PDE.SetAddress((uint64)PT >> 12);
        PDE.SetFlag(PRESENT, true);
        PDE.SetFlag(READ_WRITE, true);
        PD->entries[PT_i] = PDE;
    }

    PDE = PT->entries[P_i];
    PDE.SetAddress((uint64)physicalAddress >> 12);
    PDE.SetFlag(PRESENT, true);
    PDE.SetFlag(READ_WRITE, true);
    PT->entries[P_i] = PDE;
}