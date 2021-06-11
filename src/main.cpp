#include <gba.h>

#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "util.h"

static u16 fadeLevel;
void (*stateFunc)(void);

static void State_Init();

static void State_Idle() {
    // do nothing
}

static constexpr u32 exp32(u32 v) {
    return (v << 24) | (v << 16) | (v << 8) | v;
}

extern "C" void fire_anim(void);

static uint32_t ran(void) {
    static uint32_t state = 0x12345678;

    state *= 214013;
    state += 2531011;

    return state >> 16;
}

static void State_Main() {
    const int square_size = 16;

    REG_DISPCNT ^= BACKBUFFER;

    static bool button_pressed = 0;

    bool trigger = false;
    if (!(REG_KEYINPUT & KEY_A) && !button_pressed) {
        button_pressed = true;
        trigger = true;
    } else if (REG_KEYINPUT & KEY_A) {
        button_pressed = false;
    }

    fire_anim();

    int off = (REG_DISPCNT & BACKBUFFER) ? 0x0000 : 0xA000;

    if (trigger) {
        int y = (ran() * (160 - square_size - 1)) >> 16;
        int x = (ran() * (240 - square_size)) >> 16;
        // don't allow misalignment
        x &= ~3;

        for (int i = 0; i < square_size; i++) {
            DmaSet32((u16 *)(0x06000000 + off + (y + i) * 240 + x), exp32(36), square_size);
        }
    }
}

static void State_FadeIn() {
    REG_DISPCNT &= ~LCDC_OFF;
    REG_BLDY = --fadeLevel;

    if (fadeLevel == 0) {
        stateFunc = State_Main;
    }
}

static void State_Init() {
    static const u16 fireColors[37] = {
        RGB8(0x00, 0x00, 0x00),
        RGB8(0x07, 0x07, 0x07),
        RGB8(0x1f, 0x07, 0x07),
        RGB8(0x3f, 0x0f, 0x07),
        RGB8(0x47, 0x1f, 0x07),
        RGB8(0x57, 0x17, 0x07),
        RGB8(0x67, 0x1f, 0x07),
        RGB8(0x77, 0x1f, 0x07),
        RGB8(0x8f, 0x27, 0x07),
        RGB8(0x9f, 0x2f, 0x07),
        RGB8(0xaf, 0x3f, 0x07),
        RGB8(0xbf, 0x47, 0x07),
        RGB8(0xc7, 0x47, 0x07),
        RGB8(0xDF, 0x4F, 0x07),
        RGB8(0xDF, 0x57, 0x07),
        RGB8(0xDF, 0x57, 0x07),
        RGB8(0xD7, 0x5F, 0x07),
        RGB8(0xD7, 0x67, 0x0F),
        RGB8(0xcf, 0x6f, 0x0f),
        RGB8(0xcf, 0x77, 0x0f),
        RGB8(0xcf, 0x7f, 0x0f),
        RGB8(0xCF, 0x87, 0x17),
        RGB8(0xC7, 0x87, 0x17),
        RGB8(0xC7, 0x8F, 0x17),
        RGB8(0xC7, 0x97, 0x1F),
        RGB8(0xBF, 0x9F, 0x1F),
        RGB8(0xBF, 0x9F, 0x1F),
        RGB8(0xBF, 0xA7, 0x27),
        RGB8(0xBF, 0xA7, 0x27),
        RGB8(0xBF, 0xAF, 0x2F),
        RGB8(0xB7, 0xAF, 0x2F),
        RGB8(0xB7, 0xB7, 0x2F),
        RGB8(0xB7, 0xB7, 0x37),
        RGB8(0xCF, 0xCF, 0x6F),
        RGB8(0xDF, 0xDF, 0x9F),
        RGB8(0xEF, 0xEF, 0xC7),
        RGB8(0xFF, 0xFF, 0xFF),
    };

    // init palettes
    DmaCopy16(&BG_PALETTE[0], fireColors, sizeof(fireColors));
    REG_DISPCNT = (REG_DISPCNT & ~0b111) | MODE_4;
    REG_DISPCNT |= BG2_ON;

    // init framebuffer
    DmaSet32((u16 *)0x06000000, 0, 240 * 159);
    DmaSet32((u16 *)(0x06000000 + 240 * 159), exp32(36), 240);
    DmaSet32((u16 *)(0x0600A000 + 240 * 159), exp32(36), 240);

    fadeLevel = 16;
    REG_BLDY = fadeLevel;
    REG_BLDCNT = 0b100100 | 0x80;

    stateFunc = State_FadeIn;
}

int main() {
    irqInit();
    irqEnable(IRQ_VBLANK);

    stateFunc = State_Init;

    while (1) {
        VBlankIntrWait();
        stateFunc();
    }
}
