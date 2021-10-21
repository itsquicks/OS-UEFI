#include "utils.h"

/*// 512 bytes at a time, one load->store for every ymm register (there are 16)
void* memcpy_256bit_512B_u(void* dest, const void* src, size_t len)
{
    const __m256i_u* s = (__m256i_u*)src;
    __m256i_u* d = (__m256i_u*)dest;

    while (len--)
    {
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 1
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 2
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 3
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 4
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 5
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 6
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 7
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 8
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 9
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 10
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 11
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 12
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 13
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 14
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 15
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++)); // 16
    }

    return dest;
}

// 32 bytes at a time
void* memcpy_256bit_u(void* dest, const void* src, size_t len)
{
    const __m256i_u* s = (__m256i_u*)src;
    __m256i_u* d = (__m256i_u*)dest;

    while (len--)
    {
        _mm256_storeu_si256(d++, _mm256_lddqu_si256(s++));
    }

    return dest;
}*/

void* memcpy(void* address, void* src, uint64_t num)
{
    if (num <= 8)
    {
        uint8_t* val_ptr = (uint8_t*)src;
        for (uint8_t* ptr = (uint8_t*)address; ptr < (uint8_t*)((uint64_t)address + num); ptr++)
        {
            *ptr = *val_ptr;
            val_ptr++;
        }

        return address;
    }

    uint64_t proc_bytes = num % 8;
    uint64_t new_num = num - proc_bytes;
    uint64_t* src_ptr = (uint64_t*)src;

    for (uint64_t* dest_ptr = (uint64_t*)address; dest_ptr < (uint64_t*)((uint64_t)address + new_num); dest_ptr++)
    {
        *dest_ptr = *src_ptr;
        src_ptr++;
    }

    uint8_t* src_ptr8 = (uint8_t*)((uint64_t)src + new_num);
    for (uint8_t* dest_ptr8 = (uint8_t*)((uint64_t)address + new_num); dest_ptr8 < (uint8_t*)((uint64_t)address + num); dest_ptr8++)
    {
        *dest_ptr8 = *src_ptr8;
        src_ptr8++;
    }

    return address;
}

void* memset(void* address, uint64_t value, uint64_t num)
{
    if (num <= 8)
    {
        uint8_t* val_ptr = (uint8_t*)&value;
        for (uint8_t* ptr = (uint8_t*)address; ptr < (uint8_t*)((uint64_t)address + num); ptr++)
        {
            *ptr = *val_ptr;
            val_ptr++;
        }

        return address;
    }

    uint64_t proc_bytes = num % 8;
    uint64_t new_num = num - proc_bytes;

    for (uint64_t* ptr = (uint64_t*)address; ptr < (uint64_t*)((uint64_t)address + new_num); ptr++)
    {
        *ptr = value;
    }

    uint8_t* val_ptr = (uint8_t*)&value;
    for (uint8_t* ptr = (uint8_t*)((uint64_t)address + new_num); ptr < (uint8_t*)((uint64_t)address + num); ptr++)
    {
        *ptr = *val_ptr;
        val_ptr++;
    }

    return address;
}

void* memsetbyte(void* address, uint8_t value, uint64_t num)
{
    for (uint8_t* ptr = (uint8_t*)address; ptr < (uint8_t*)((uint64_t)address + num); ptr++)
        *ptr = value;

    return address;
}

char* strcpy(char* destination, char* source)
{
    uint64_t i = 0;
    while (*(source + i) != 0)
    {
        *(destination + i) = *(source + i);
        i++;
    }
    *(destination + i) = 0;

    return destination;
}

char* strncpy(char* destination, char* source, uint64_t num)
{
    uint64_t i = 0;
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
    uint64_t len = strlen(destination);
    uint64_t i = 0;
    while (*(source + i) != 0)
    {
        *(destination + len + i) = *(source + i);
        i++;
    }
    *(destination + len + i) = 0;

    return destination;
}

char* strncat(char* destination, char* source, uint64_t num)
{
    uint64_t len = strlen(destination);
    uint64_t i = 0;
    while (*(source + i) != 0 && i < num)
    {
        *(destination + len + i) = *(source + i);
        i++;
    }
    *(destination + len + i) = 0;

    return destination;
}

int64_t memcmp(void* ptr1, void* ptr2, uint64_t num)
{
    uint8_t* p1 = (uint8_t*)ptr1;
    uint8_t* p2 = (uint8_t*)ptr2;

    while (num-- > 0)
    {
        if (*p1++ != *p2++)
            return p1[-1] < p2[-1] ? -1 : 1;
    }

    return 0;
}

int64_t strcmp(char* str1, char* str2)
{
    char* ptr1 = str1;
    char* ptr2 = str2;

    while (*ptr1 == *ptr2++)
        if (*ptr1++ == 0)
            return 0;

    return (*(uint8_t*)ptr1 - *(uint8_t*)--str2);
}

int64_t strncmp(char* str1, char* str2, uint64_t num)
{
    char* ptr1 = str1;
    char* ptr2 = str2;

    uint8_t u1, u2;

    while (num-- > 0)
    {
        u1 = (uint8_t)*ptr1++;
        u2 = (uint8_t)*ptr2++;

        if (u1 != u2)
            return u1 - u2;

        if (u1 == 0)
            return 0;
    }
    return 0;
}

void* memchr(void* ptr, uint8_t value, uint64_t num)
{
    for (uint8_t* i = (uint8_t*)ptr;i < (uint8_t*)((uint64_t)ptr + num);i++)
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

uint64_t strlen(char* string)
{
    char* ptr = (char*)string;
    uint64_t ret = 0;

    while (*ptr != 0)
    {
        ptr++;
        ret++;
    }

    return ret;
}

void printf(char* format, ...)
{
    va_list ap;
    va_start(ap, format);

    char* ptr = (char*)format;

    while (*ptr != 0)
    {
        if (*ptr == '%')
        {
            uint64_t hex = 0;
            int64_t val = 0;
            ptr++;
            switch (*ptr)
            {
            case 0:
                va_end(ap);
                return;
                break;
            case 'd':
                val = va_arg(ap, uint64_t);
                if (val > 0xFFFFFFFF)
                    print_string(itos(val));
                else
                    print_string(itos((uint64_t)val));
                break;
            case 'i':
                val = va_arg(ap, uint64_t);
                if (val > 0xFFFFFFFF)
                    print_string(itos(val));
                else
                    print_string(itos((uint64_t)val));
                break;
            case 'u':
                print_string(uitos(va_arg(ap, uint64_t)));
                break;
            case 'x':
                hex = va_arg(ap, uint64_t);
                if (hex <= 0xFF)
                    print_string(xtos((uint8_t)hex));
                else if (hex <= 0xFFFF)
                    print_string(xtos((uint16_t)hex));
                else if (hex <= 0xFFFFFFFF)
                    print_string(xtos((uint32_t)hex));
                else
                    print_string(xtos(hex));
                break;
            case 'f':
                print_string(dtos(va_arg(ap, double)));
                break;
            case 'p':
                print_string(xtos((uint64_t)va_arg(ap, void*)));
                break;
            case 'c':
                print_char((char)va_arg(ap, int));
                break;
            case 's':
                print_string(va_arg(ap, char*));
                break;
            case '%':
                print_char('%');
                break;
            }
        }
        else
        {
            print_char(*ptr);
        }

        ptr++;
    }

    va_end(ap);
}

char buffer[256];

char* itos(int64_t value)
{
    uint8_t negative = 0;

    if (value < 0)
    {
        negative = 1;
        value *= -1;
        buffer[0] = '-';
    }

    uint8_t size;
    uint64_t size_test = value;
    while (size_test / 10 > 0)
    {
        size_test /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        buffer[negative + size - index] = remainder + '0';
        index++;
    }
    uint8_t remainder = value % 10;
    buffer[negative + size - index] = remainder + '0';
    buffer[negative + size + 1] = 0;

    return buffer;
}

char* uitos(uint64_t value)
{
    uint8_t size;
    uint64_t size_test = value;
    while (size_test / 10 > 0) {
        size_test /= 10;
        size++;
    }

    uint8_t index = 0;
    while (value / 10 > 0)
    {
        uint8_t remainder = value % 10;
        value /= 10;
        buffer[size - index] = remainder + '0';
        index++;
    }

    uint8_t remainder = value % 10;
    buffer[size - index] = remainder + '0';
    buffer[size + 1] = 0;

    return buffer;
}

char* dtos(double value, uint8_t decimals)
{
    if (decimals > 20) decimals = 20;

    char* int_ptr = (char*)itos((uint64_t)value);
    char* double_ptr = buffer;

    if (value < 0)  value *= -1;

    while (*int_ptr != 0)
    {
        *double_ptr = *int_ptr;
        int_ptr++;
        double_ptr++;
    }

    *double_ptr = '.';
    double_ptr++;

    double new_value = value - (int)value;

    for (uint8_t i = 0; i < decimals; i++)
    {
        new_value *= 10;
        *double_ptr = (int)new_value + '0';
        new_value -= (int)new_value;
        double_ptr++;
    }

    *double_ptr = 0;

    return buffer;
}

char* xtos(uint8_t value)
{
    uint8_t* val_ptr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 1 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)val_ptr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint16_t value)
{
    uint16_t* val_ptr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 2 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)val_ptr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint32_t value)
{
    uint32_t* val_ptr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 4 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)val_ptr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

char* xtos(uint64_t value)
{
    uint64_t* val_ptr = &value;
    uint8_t* ptr;
    uint8_t tmp;
    uint8_t size = 8 * 2 - 1;
    for (uint8_t i = 0; i < size; i++)
    {
        ptr = ((uint8_t*)val_ptr + i);
        tmp = ((*ptr & 0xF0) >> 4);
        buffer[size - (i * 2 + 1)] = tmp + (tmp > 9 ? 55 : '0');
        tmp = ((*ptr & 0x0F));
        buffer[size - (i * 2)] = tmp + (tmp > 9 ? 55 : '0');
    }
    buffer[size + 1] = 0;

    return buffer;
}

uint64_t stoi(char* string)
{
    uint64_t ret = 0;
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

uint64_t stox(char* string)
{
    uint64_t ret = 0;
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

bool match(char* str1, char* str2)
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

void print_buffer(uint8_t* buffer, uint64_t size)
{
    for (uint64_t i = 0; i < size; i++)
    {
        if ((i % 16 == 0) && i > 0)
            printf("\n");

        printf("%x ", buffer[i]);
    }

    printf("\n");
}


