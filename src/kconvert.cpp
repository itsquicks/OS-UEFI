#include "kconvert.h"

char buffer[256];

char* itos(int64 value)
{
    uint8 negative = 0;

    if (value < 0)
    {
        negative = 1;
        value *= -1;
        buffer[0] = '-';
    }

    uint8 size;
    uint64 sizeTest = value;
    while (sizeTest / 10 > 0)
    {
        sizeTest /= 10;
        size++;
    }

    uint8 index = 0;
    while (value / 10 > 0)
    {
        uint8 remainder = value % 10;
        value /= 10;
        buffer[negative + size - index] = remainder + '0';
        index++;
    }
    uint8 remainder = value % 10;
    buffer[negative + size - index] = remainder + '0';
    buffer[negative + size + 1] = 0;

    return buffer;
}

char* uitos(uint64 value)
{
    uint8 size;
    uint64 sizeTest = value;
    while (sizeTest / 10 > 0) {
        sizeTest /= 10;
        size++;
    }

    uint8 index = 0;
    while (value / 10 > 0)
    {
        uint8 remainder = value % 10;
        value /= 10;
        buffer[size - index] = remainder + '0';
        index++;
    }

    uint8 remainder = value % 10;
    buffer[size - index] = remainder + '0';
    buffer[size + 1] = 0;

    return buffer;
}

char* dtos(double value, uint8 decimals)
{
    if (decimals > 20) decimals = 20;

    char* intPtr = (char*)itos((int64)value);
    char* doublePtr = buffer;

    if (value < 0)  value *= -1;

    while (*intPtr != 0)
    {
        *doublePtr = *intPtr;
        intPtr++;
        doublePtr++;
    }

    *doublePtr = '.';
    doublePtr++;

    double newValue = value - (int)value;

    for (uint8 i = 0; i < decimals; i++)
    {
        newValue *= 10;
        *doublePtr = (int)newValue + '0';
        newValue -= (int)newValue;
        doublePtr++;
    }

    *doublePtr = 0;

    return buffer;
}

char* xtos(uint8 value)
{
    uint8* valPtr = &value;
    uint8* ptr;
    uint8 tmp;
    uint8 size = 1 * 2 - 1;
    for (uint8 i = 0; i < size; i++)
    {
        ptr = ((uint8*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint16 value)
{
    uint16* valPtr = &value;
    uint8* ptr;
    uint8 tmp;
    uint8 size = 2 * 2 - 1;
    for (uint8 i = 0; i < size; i++)
    {
        ptr = ((uint8*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint32 value)
{
    uint32* valPtr = &value;
    uint8* ptr;
    uint8 tmp;
    uint8 size = 4 * 2 - 1;
    for (uint8 i = 0; i < size; i++)
    {
        ptr = ((uint8*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint64 value)
{
    uint64* valPtr = &value;
    uint8* ptr;
    uint8 tmp;
    uint8 size = 8 * 2 - 1;
    for (uint8 i = 0; i < size; i++)
    {
        ptr = ((uint8*)valPtr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

uint64 stoi(char* string)
{
    uint64 ret = 0;
    char* ptr = string;

    while (*ptr != 0)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            ret += *ptr - 0x30;
        }

        ret = ret * 10;
        ptr++;
    }

    ret = ret / 10;
    return ret;
}

uint64 stox(char* string)
{
    uint64 ret = 0;
    char* ptr = string;

    while (*ptr != 0)
    {
        if (*ptr >= '0' && *ptr <= '9')
        {
            ret += *ptr - 0x30;
        }
        if (*ptr >= 'a' && *ptr <= 'f')
        {
            ret += *ptr - 0x57;
        }
        if (*ptr >= 'A' && *ptr <= 'F')
        {
            ret += *ptr - 0x37;
        }

        ret = ret << 4;
        ptr++;
    }

    ret = ret >> 4;
    return ret;
}


