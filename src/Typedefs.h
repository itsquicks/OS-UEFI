#pragma once

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

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

struct FILE
{
    uint8 flags;

    uint8* read_ptr;
    uint8* write_ptr;

    uint8* buffer_base;
    uint8* buffer_end;

    uint8 fileno;
}__attribute__((packed));