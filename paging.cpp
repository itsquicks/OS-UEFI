#include "paging.h"

PageTable* pml4;

void paging_init()
{
    pml4 = (PageTable*)request_page();
    memset(pml4, 0, 4096);
}

void map_memory(void* virtual_address, void* physical_address)
{
    uint64_t address = (uint64_t)virtual_address;

    address >>= 12;
    uint64_t p_i = address & 0x1ff;
    address >>= 9;
    uint64_t pt_i = address & 0x1ff;
    address >>= 9;
    uint64_t pd_i = address & 0x1ff;
    address >>= 9;
    uint64_t pdp_i = address & 0x1ff;

    Page pde;

    pde = pml4->entries[pdp_i];
    PageTable* pdp;

    if (pde.get_flag(PRESENT))
    {
        pdp = (PageTable*)((uint64_t)pde.get_address() << 12);
    }
    else
    {
        pdp = (PageTable*)request_page();
        memset(pdp, 0, 4096);
        pde.set_address((uint64_t)pdp >> 12);
        pde.set_flag(PRESENT, true);
        pde.set_flag(READ_WRITE, true);
        pml4->entries[pdp_i] = pde;
    }

    pde = pdp->entries[pd_i];
    PageTable* pd;
    if (pde.get_flag(PRESENT))
    {
        pd = (PageTable*)((uint64_t)pde.get_address() << 12);
    }
    else
    {
        pd = (PageTable*)request_page();
        memset(pd, 0, 4096);
        pde.set_address((uint64_t)pd >> 12);
        pde.set_flag(PRESENT, true);
        pde.set_flag(READ_WRITE, true);
        pdp->entries[pd_i] = pde;
    }

    pde = pd->entries[pt_i];
    PageTable* pt;
    if (pde.get_flag(PRESENT))
    {
        pt = (PageTable*)((uint64_t)pde.get_address() << 12);
    }
    else
    {
        pt = (PageTable*)request_page();
        memset(pt, 0, 4096);
        pde.set_address((uint64_t)pt >> 12);
        pde.set_flag(PRESENT, true);
        pde.set_flag(READ_WRITE, true);
        pd->entries[pt_i] = pde;
    }

    pde = pt->entries[p_i];
    pde.set_address((uint64_t)physical_address >> 12);
    pde.set_flag(PRESENT, true);
    pde.set_flag(READ_WRITE, true);
    pt->entries[p_i] = pde;
}