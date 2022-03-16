; #include <avr/io.h>
; #define F_CPU 16000000UL // 16 MHz
; #ifndef __AVR_ATmega328P__
; #define __AVR_ATmega328P__
; #endif

.global linegoesbrr
linegoesbrr: ;R22 contains the ctr value
    mov     r30, r24 
    mov     r31, r25
    loop:
    ld      r24, Z
    out     0x0b, r24
    nop
    add     r24, r24
    dec     r22         ;Dec CTR - pixel byte counter
    out     0x0b, r24
    add     r24, r24
    adiw    r30,1
    nop
    out     0x0b, r24
    nop
    nop
    add     r24, r24
    out     0x0b, r24
    add     r24, r24
    nop
    nop
    out     0x0b, r24
    add     r24, r24
    nop
    nop
    out     0x0b, r24
    add     r24, r24
    nop
    nop
    out     0x0b, r24
    add     r24, r24
    cpi     r22, 0
    nop
    out     0x0b, r24
    brne    loop
    add     r24, r24 ;Make sure last pixel on the line is a 0!
    out     0x0b, r24
    ret


    ;vid = fd.line1[ctr];
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   asm volatile("dec %0" : "+r"(ctr));
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   __asm__ __volatile__("NOP");
    ;   vid = vid << 1;
    ;   PORTD = vid;
    ; }