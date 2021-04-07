#include "kmath.h"

uint64 max(uint64 a, uint64 b)
{
    return (a > b) ? a : b;
}
uint64 min(uint64 a, uint64 b)
{
    return (a < b) ? a : b;
}

int abs(int n)
{
    return (n < 0) ? -n : n;
}