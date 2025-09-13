#pragma once

#include <malloc.h>
#include <memory.h>
#include <assert.h>

#define BIT(n) (1 << n)

#undef NULL
#define NULL 0

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#define U8_MAX  0xff
#define U16_MAX 0xffff

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef u8 b8;
typedef u32 b32;


// 16-bit byte swap
// @param[in] hword Half-word to perform byteswap on
// @returns `hword`, bytes wapped
inline static u16 bswap16(u16 hword) {
    return (u16)(((hword & 0x00ff) << 8) | ((hword & 0xff00) >> 8));
}