#include "intrinsics.h"

u8 is_system_big_endian()
{
    static const i32 __one = 1;
    return ((*(byte*)&__one) == 0);
}

u16 u16_reverse_bits(u16 value)
{
    return (value << 8) | (value >> 8);
}

u32 u32_reverse_bits(u32 value)
{
    value = ((value << 8) & 0xFF00FF00) | ((value >> 8) & 0xFF00FF); 
    return (value << 16) | (value >> 16);
}

u64 u64_reverse_bits(u64 value)
{
    value = ((value << 8) & 0xFF00FF00FF00FF00ULL) | ((value >> 8) & 0x00FF00FF00FF00FFULL);
    value = ((value << 16) & 0xFFFF0000FFFF0000ULL) | ((value >> 16) & 0x0000FFFF0000FFFFULL);
    return (value << 32) | (value >> 32);
}
