#include "EFI_Memory.h"

const char* EFI_MEMORY_TYPE_STRINGS[]
{
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",
};

/*void memdmp(void* address, uint8 count)
{
    uint8* ptr = (uint8*)address;

    for (uint8 i = 0; i < count; i++)
    {
        uint8* ascii = ptr;

        colorf = GREEN;
        printf("0x%p     ", ptr);

        for (uint8 n = 0; n < 16; n++)
        {
            printf("%x ", *ptr);
            ptr++;
        }
        printf("     ");

        for (uint8 n = 0; n < 16; n++)
        {
            if (*ascii > 31)
            {
                printf("%c", *ascii);
            }
            else
            {
                colorf = GRAY;
                printf(".");
            }

            ascii++;
        }
        printf("\r\n");
    }
}*/