#ifndef __INTRINSICS_H

#include <stdint.h>

// NOTE: 64-bit
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned char byte;

#endif

// TODO: Request page size from the OS instead
#define PAGE_SIZE 4096

#define mod_pow_2(operand1, operand2) ((operand1) & ((operand2) - 1))

#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#elif defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)
#define FORCEINLINE __attribute__((always_inline)) inline
#else
#error Unsupported compiler
#endif
    
#define __INTRINSICS_H
#endif
