#pragma once

#include <gba.h>

static inline void DmaCopy16(u16 *dst, const u16 *src, u32 len) {
    register u32 r_src asm("r0") = (u32)src;
    register u32 r_dst asm("r1") = (u32)dst;
    register u32 r_ctl asm("r2") = (u32)(len >> 1) | DMA_ENABLE;
    register u32 r_reg asm("r3") = (u32)0x040000d4;
    asm volatile("stmia %3!, {%0, %1, %2}\n\tsub %3, %3, #12" :: "l" (r_src), "l" (r_dst), "l" (r_ctl), "l" (r_reg) : "memory", "cc");
}

static inline void DmaSet16(u16 *dst, u16 val, u32 len) {
    register u32 r_src asm("r0") = (u32)&val;
    register u32 r_dst asm("r1") = (u32)dst;
    register u32 r_ctl asm("r2") = (u32)(len >> 1) | DMA_ENABLE | DMA_SRC_FIXED;
    register u32 r_reg asm("r3") = (u32)0x040000d4;
    asm volatile("stmia %3!, {%0, %1, %2}\n\tsub %3, %3, #12" :: "l" (r_src), "l" (r_dst), "l" (r_ctl), "l" (r_reg) : "memory", "cc");
}

static inline void DmaSet32(u16 *dst, u32 val, u32 len) {
    register u32 r_src asm("r0") = (u32)&val;
    register u32 r_dst asm("r1") = (u32)dst;
    register u32 r_ctl asm("r2") = (u32)(len >> 2) | DMA_ENABLE | DMA_SRC_FIXED | DMA32;
    register u32 r_reg asm("r3") = (u32)0x040000d4;
    asm volatile("stmia %3!, {%0, %1, %2}\n\tsub %3, %3, #12" :: "l" (r_src), "l" (r_dst), "l" (r_ctl), "l" (r_reg) : "memory", "cc");
}
