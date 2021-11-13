#include <gba.h>
#include <gba_sound.h>

/*******************************************************************************
 * type definitions
 ***/

typedef struct {
	u16 type;
	u16 stat;
	u32 freq;
	u32 loop;
	u32 size;
	s8 data[];
} WaveDataCorrect;

typedef struct {
    u8 ctl[4];
    const WaveDataCorrect *wp;
    u8 a, d, s, r;
} ToneData;

typedef struct {
    u8 numTracks;
    u8 dummy;
    u8 prio;
    u8 reverb;
    const ToneData *tone;
    const u8 *tracks[];
} Song;

typedef struct {
    u32 dummy[20];
} MTrack;

typedef struct {
    u32 dummy[16];
} MPlayer;

/*******************************************************************************
 * svc interfaces
 ***/

static inline void SoundInit(SoundArea *sa) {
    register SoundArea *_sa asm("r0") = sa;
    asm volatile("svc #0x1a" : "+l" (_sa) :: "cc", "r1", "r3", "r12");
}

static inline void SoundMode(u32 mode) {
    register u32 _mode asm("r0") = mode;
    asm volatile("svc #0x1b" : "+l" (_mode) :: "cc", "r1", "r3", "r12");
}

static inline void MPlayOpen(MPlayer *player, MTrack *tracks, u32 count) {
    register u32 _player asm("r0") = (u32)player;
    register u32 _tracks asm("r1") = (u32)tracks;
    register u32 _count asm("r2") = count;
    asm volatile("svc #0x20" : "+l" (_player), "+l" (_tracks), "+l" (_count) :: "cc", "r3", "r12");
}

static inline void SoundMain() {
    asm volatile("svc #0x1c" ::: "cc", "r0", "r1", "r3", "r12");
}

static inline void SoundVSync() {
    asm volatile("svc #0x1d" ::: "cc", "r0", "r1", "r3", "r12");
}

static inline void MPlayStart(MPlayer *player, const Song *song) {
    register u32 _player asm("r0") = (u32)player;
    register u32 _song asm("r1") = (u32)song;
    asm volatile("svc #0x21" : "+l" (_player), "+l" (_song) :: "cc", "r3", "r12");
}

/*******************************************************************************
 * demo sound data
 ***/

static const WaveDataCorrect testSample = {
    .type = 0,
    .stat = 0x4000,
    .freq = 68583572,
    .loop = 0,
    .size = 256,
    .data = { 0, 3, 6, 9, 12, 16, 19, 22, 25, 28, 31, 34, 37, 40, 43, 46, 49, 51, 54, 57, 60, 63, 65, 68, 71, 73, 76, 78, 81, 83, 85, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 107, 109, 111, 112, 113, 115, 116, 117, 118, 120, 121, 122, 122, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 122, 122, 121, 120, 118, 117, 116, 115, 113, 112, 111, 109, 107, 106, 104, 102, 100, 98, 96, 94, 92, 90, 88, 85, 83, 81, 78, 76, 73, 71, 68, 65, 63, 60, 57, 54, 51, 49, 46, 43, 40, 37, 34, 31, 28, 25, 22, 19, 16, 12, 9, 6, 3, 0, -3, -6, -9, -12, -16, -19, -22, -25, -28, -31, -34, -37, -40, -43, -46, -49, -51, -54, -57, -60, -63, -65, -68, -71, -73, -76, -78, -81, -83, -85, -88, -90, -92, -94, -96, -98, -100, -102, -104, -106, -107, -109, -111, -112, -113, -115, -116, -117, -118, -120, -121, -122, -122, -123, -124, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, -127, -127, -126, -126, -126, -125, -125, -124, -123, -122, -122, -121, -120, -118, -117, -116, -115, -113, -112, -111, -109, -107, -106, -104, -102, -100, -98, -96, -94, -92, -90, -88, -85, -83, -81, -78, -76, -73, -71, -68, -65, -63, -60, -57, -54, -51, -49, -46, -43, -40, -37, -34, -31, -28, -25, -22, -19, -16, -12, -9, -6, -3, 0 },
};

static const ToneData testTone = {
    .ctl = { 0, 60, 0, 0 },
    .wp = &testSample,
    .a = 127,
    .d = 245,
    .s = 180,
    .r = 165,
};

static const u8 testSongTrack0[] = {
    0xbc, 12,        // KEYSH, 0
    0xbb, 120/2,    // TEMPO, 120 BPM
    0xbd, 0,        // VOICE, 0
    0xbe, 100,      // VOL, 100
    0xbf, 64,
    0xe1, 60, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 62, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 64, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 65, 127,  // N12, Cn3, v127
    0x8c,           // W12

    0xe1, 67, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 69, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 71, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xe1, 72, 127,  // N12, Cn3, v127
    0x8c,           // W12
    0xb1,           // FINE
};

static const Song testSong = {
    .numTracks = 1,
    .dummy = 0,
    .prio = 0,
    .reverb = 0,
    .tone = &testTone,
    .tracks = {
        &testSongTrack0[0],
    },
};


/*******************************************************************************
 * demo code
 ***/

static SoundArea gSA;
static MPlayer gMPlayPlayerBGM;
static MTrack gMPlayTrackBGM[16];

static void vblank_isr() {
    SoundVSync();
    SoundMain();
}

int main() {
    irqInit();
    irqEnable(IRQ_VBLANK);
    irqSet(IRQ_VBLANK, vblank_isr);

    SoundInit(&gSA);
    u32 mode = 0;
    mode |= 0x50 << 0;  // 0x50 reverb
    mode |= 12 << 8;    // 12 max voices
    mode |= 15 << 12;   // 15 master volume
    mode |= 7 << 16;    // 21024 Hz samplerate
    mode |= 9 << 20;    // 8 bit 65 kHz DAC PWM
    SoundMode(mode);
    MPlayOpen(&gMPlayPlayerBGM, gMPlayTrackBGM, 16);
    MPlayStart(&gMPlayPlayerBGM, &testSong);

    while (1) { }
}
