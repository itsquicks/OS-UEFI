#pragma once
#include <stdint.h>

#define null 0

#define GB 1073741824
#define MB 1048576
#define KB 1024

#define BACKGROUND 0x112136
#define SELECT 0x3d4e65
#define FRONT 0xE0EEEE

#define RED 0xff5857
#define YELLOW 0xfff95c
#define GREEN 0x8cfb6b
#define CYAN 0x6cf9ef
#define BLUE 0x6ea2ed
#define MAGENTA 0xFF00FF
#define WHITE 0xFFFFFF
#define GRAY 0x4c5e75
#define BLACK 0x000000

struct Bitmap
{
    uint64_t size;
    uint8_t* buffer;

    bool operator[](uint64_t index)
    {
        if (index > size * 8) return false;

        uint64_t byte_index = index / 8;
        uint8_t bit_index = index % 8;
        uint8_t selector = 0b10000000 >> bit_index;

        return buffer[byte_index] & selector;
    }

    bool set(uint64_t index, bool value)
    {
        if (index > size * 8) return false;

        uint64_t byte_index = index / 8;
        uint8_t bit_index = index % 8;
        uint8_t selector = 0b10000000 >> bit_index;

        buffer[byte_index] &= ~selector;

        if (value)
            buffer[byte_index] |= selector;

        return true;
    }
};
