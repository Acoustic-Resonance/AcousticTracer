#include "../src/at_bvh.h"

#include <stdbool.h>

uint32_t flt_to_int(float num)
{
    uint32_t res = *(int *)&num;
    bool is_neg = signbit(num);
    if (is_neg) {
        res ^= 0xFFFFFFFF;
    } else {
        res ^= 0x80000000;
    }

    return res;
}

float int_to_flt(uint32_t num)
{
    bool is_neg = signbit(num);
    if (is_neg) {
        num ^= 0x80000000;
    } else {
        num ^= 0xFFFFFFFF;
    }

    return *(float *)&num;
}

unsigned char get_nth_byte(uint32_t num, int n)
{
    int offset = 8 * n;
    return (num & (0xFF << offset)) >> offset;
}

// Run through items converting to int and keeping track of largest number to get num digits
// Go byte by byte
void AT_BVH_sort_triangles(AT_Triangle *triangles)
{
}
