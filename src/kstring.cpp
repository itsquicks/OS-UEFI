#include "kstring.h"

void* memcpy(void* address, void* src, uint64 num)
{
    if (num <= 8)
    {
        uint8* valPtr = (uint8*)src;
        for (uint8* ptr = (uint8*)address; ptr < (uint8*)((uint64)address + num); ptr++)
        {
            *ptr = *valPtr;
            valPtr++;
        }

        return address;
    }

    uint64 procBytes = num % 8;
    uint64 newnum = num - procBytes;
    uint64* srcPtr = (uint64*)src;

    for (uint64* destPtr = (uint64*)address; destPtr < (uint64*)((uint64)address + newnum); destPtr++)
    {
        *destPtr = *srcPtr;
        srcPtr++;
    }

    uint8* srcPtr8 = (uint8*)((uint64)src + newnum);
    for (uint8* destPtr8 = (uint8*)((uint64)address + newnum); destPtr8 < (uint8*)((uint64)address + num); destPtr8++)
    {
        *destPtr8 = *srcPtr8;
        srcPtr8++;
    }

    return address;
}

void* memset(void* address, uint64 value, uint64 num)
{
    if (num <= 8)
    {
        uint8* valPtr = (uint8*)&value;
        for (uint8* ptr = (uint8*)address; ptr < (uint8*)((uint64)address + num); ptr++)
        {
            *ptr = *valPtr;
            valPtr++;
        }

        return address;
    }

    uint64 procBytes = num % 8;
    uint64 newnum = num - procBytes;

    for (uint64* ptr = (uint64*)address; ptr < (uint64*)((uint64)address + newnum); ptr++)
    {
        *ptr = value;
    }

    uint8* valPtr = (uint8*)&value;
    for (uint8* ptr = (uint8*)((uint64)address + newnum); ptr < (uint8*)((uint64)address + num); ptr++)
    {
        *ptr = *valPtr;
        valPtr++;
    }

    return address;
}

char* strcpy(char* destination, char* source)
{
    uint64 i = 0;
    while (*(source + i) != 0)
    {
        *(destination + i) = *(source + i);
        i++;
    }
    *(destination + i) = 0;

    return destination;
}

char* strncpy(char* destination, char* source, uint64 num)
{
    uint64 i = 0;
    while (*(source + i) != 0 && i < num)
    {
        *(destination + i) = *(source + i);
        i++;
    }
    *(destination + i) = 0;

    return destination;
}

char* strcat(char* destination, char* source)
{
    uint64 len = strlen(destination);
    uint64 i = 0;
    while (*(source + i) != 0)
    {
        *(destination + len + i) = *(source + i);
        i++;
    }
    *(destination + len + i) = 0;

    return destination;
}

char* strncat(char* destination, char* source, uint64 num)
{
    uint64 len = strlen(destination);
    uint64 i = 0;
    while (*(source + i) != 0 && i < num)
    {
        *(destination + len + i) = *(source + i);
        i++;
    }
    *(destination + len + i) = 0;

    return destination;
}

int64 memcmp(void* ptr1, void* ptr2, uint64 num)
{
    uint8* p1 = (uint8*)ptr1;
    uint8* p2 = (uint8*)ptr2;

    while (num-- > 0)
    {
        if (*p1++ != *p2++)
            return p1[-1] < p2[-1] ? -1 : 1;
    }

    return 0;
}

int64 strcmp(char* str1, char* str2)
{
    char* ptr1 = str1;
    char* ptr2 = str2;

    while (*ptr1 == *ptr2++)
        if (*ptr1++ == 0)
            return 0;

    return (*(uint8*)ptr1 - *(uint8*)--str2);
}

int64 strncmp(char* str1, char* str2, uint64 num)
{
    char* ptr1 = str1;
    char* ptr2 = str2;

    uint8 u1, u2;

    while (num-- > 0)
    {
        u1 = (uint8)*ptr1++;
        u2 = (uint8)*ptr2++;

        if (u1 != u2)
            return u1 - u2;

        if (u1 == 0)
            return 0;
    }
    return 0;
}

void* memchr(void* ptr, uint8 value, uint64 num)
{
    for (uint8* i = (uint8*)ptr;i < (uint8*)((uint64)ptr + num);i++)
    {
        if (*i == value)
            return i;
    }

    return 0;
}

char* strchr(char* str, char value)
{
    char* ptr = str;
    while (*ptr != 0)
    {
        if (*ptr == value)
            return ptr;

        ptr++;
    }

    return 0;
}

uint64 strlen(char* string)
{
    char* ptr = (char*)string;
    uint64 ret = 0;

    while (*ptr != 0)
    {
        ptr++;
        ret++;
    }

    return ret;
}

bool Match(char* str1, char* str2)
{
    char* ptr1 = str1;
    char* ptr2 = str2;

    while (*ptr1 != 0)
    {
        if (*ptr1 != *ptr2)
            return false;

        ptr1++;
        ptr2++;
    }

    if (*ptr2 != 0)
        return false;

    return true;
}