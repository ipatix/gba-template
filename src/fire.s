
.equ DISPCNT, 0x04000000

.section .iwram
.global fire_anim
.arm

fire_anim:
    stmfd   sp!, {r4-r11}
    # setup VRAM back and front buffer adress
    mov     r4, #0x06000000
    mov     r5, #0x06000000
    mov     r0, #DISPCNT
    ldr     r0, [r0]
    tst     r0, #0x10           @ check back buffer bit
    addeq   r4, r4, #0xA000     @ r4 = dst (current back buffer)
    addne   r5, r5, #0xA000     @ r5 = src (current front buffer)
    ldr     r6, ran_state       @ hi
    ldr     r7, ran_state_2     @ lo
    add     r8, r5, #(240 * 160) @ r8 = src end

    add     r5, r5, #240
    ldr     r9, =0x01010101
    @ newlib random primes
    ldr     r10, =0x5851F42D    @ hi
    ldr     r11, =0x4C957F2D    @ lo

main_loop:
    # calc random number
    mul     r0, r7, r10
    mla     r6, r11, r6, r0
    umull   r0, r1, r7, r11
    adds    r7, r0, #1
    adc     r6, r1, r6
    # create subtractor
    and     r1, r9, r6
    rsb     r1, r1, r9, lsl#7
    # subtract randomized palette index of 4 pixels
    ldmia   r5!, {r0}
    add     r0, r0, r1
    and     r1, r9, r0, lsr#7
    rsb     r1, r1, r1, lsl#7
    and     r0, r0, r1
    # rotate pixels around a little bit
    movs    r1, r6, lsl#1
    movmi   r0, r0, ror#8
    movcs   r0, r0, ror#24
    stmia   r4!, {r0}
    # repeat
    cmp     r5, r8
    blt     main_loop
    str     r6, ran_state
    str     r7, ran_state_2
    ldmfd   sp!, {r4-r11}
    bx lr

    .pool

ran_state:
    .word 0x12345671
ran_state_2:
    .word 0x9ABCDEF3