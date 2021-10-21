#include "math.h"

uint64_t max(uint64_t a, uint64_t b)
{
    return (a > b) ? a : b;
}
uint64_t min(uint64_t a, uint64_t b)
{
    return (a < b) ? a : b;
}

uint64_t round(double n)
{
    double diff = n - (int)n;

    if (diff >= 0.5)
        return (int)n + 1;
    else
        return (int)n;
}

int abs(int n)
{
    return (n < 0) ? -n : n;
}