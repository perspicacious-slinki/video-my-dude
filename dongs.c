#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "fonts/font8x8.h"

#define F_CPU 16000000UL // 16 MHz
#ifndef __AVR_ATmega328P__
#define __AVR_ATmega328P__
#endif
// Generating PAL interlaced vid is a pain in the arse but here we are!!!!
// TLDR:

// Field 1
// Start of line 1 to mid-way through line 3, Long-sync pulses
// Line 3.5 to end of line 5, short sync pulses
// Line 6(24) - 310, Video!
//  Line 311 to middle of line 313, short sync pulses

// Field 2
// middle of line 313 to end of line 315, long sync pulses (Note how these
// fuckers are half a line out of phase vs field 1) Line 316 to end of line 317,
// short sync pulses Line 318(336) to 622, Video! Line 623,624,625, short sync
// pulses

// Each field has 305 lines of video. These interlace to 610 usable (i.e.
// non-sync) lines. TV usually only renders 576 of these, Leaving a total of 34
// non-video, non-sync lines per frame. Presumably that's 17 per field - I
// believe at the start of the field. There's some whack stuff about
// half-scanline video garbage so I pretend it's 18 per field instead to make my
// brain hurt less. Hope you can get all your real compute done in the time
// afforded by these suckers lmao

// This gives 288 lines of actual vidya per field! Exciting!

// To make the program simpler, Field 1 sync handler will do the end of field
// 2's sync pulses. Ditto field 2 sync handler - it will handle the end of field
// 1 sync. Thus syncs are handled as such:
//  Field 1 sync: (field 2 end short + field 1 start long + field 1 start short)
//  Field 2 sync: (field 1 end short + field 2 start long + field 2 start short)

void field_1_sync();
void field_2_sync();
void line_gen_f1();
void line_gen_f2();
extern void linegoesbrr(uint8_t *ptr, uint8_t counter);
// If we're going to do a smelly global thing for frame stuff, at least bundle
// it up in a struct
volatile uint8_t sanitycheck = 1;
uint8_t stinger = 0;
uint16_t cunt = 0;
char textbuff[18] = {0x48, 0x65, 0x6c, 0x6c, 0x6f};
struct framedata_t {
  uint8_t line1[18];
  uint8_t line2[18];
  uint8_t *l_ptr;
  uint16_t line;
  uint8_t vsync;
  void (*line_handler)(void);
};

// Chuck in a vertical bar pattern for funzies
struct framedata_t fd = {.line1 = {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00,
                                   0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff,
                                   0x00, 0xff},
                         .l_ptr = fd.line1,
                         .line = 0,
                         .vsync = 16,
                         .line_handler = &field_1_sync};

int main() {
  for (int i = 0; i < 18; i++) {
    fd.line1[i] = 0b10101010;
  }

  cli();
  // PB1(OC1A) and PB7 as outputs
  DDRB |= (1 << 1);
  DDRD |= (1 << 7);

  DDRB |= (1 << 5); // blink lol

  // TIM1 memes for timer fuckin' teens
  TCCR1A |= _BV(COM1A1) | _BV(COM1A0); // Set at bottom, clear at compare match
  TCCR1A |= _BV(WGM11);                // weedspeed PWM, TOP = ICR1
  TCCR1B |= _BV(WGM13) | _BV(WGM12);
  // Yeet values

  OCR1A = 26;
  ICR1 = 1024;

  TCCR1B |= _BV(CS10); // no prescale on clock, i.e. 16meg
  TIMSK1 |= _BV(
      TOIE1); // Overflow interrupt enable - i.e. int at start of new scanline
  sei();

  while (1) {
    _delay_ms(100);
    PORTB |= (1 << 5);
    textbuff[5]++;
    _delay_ms(100);
    PORTB &= !(1 << 5);
    cunt++;
  }
  return 0;
}

ISR(TIMER1_OVF_vect) { (*fd.line_handler)(); }

void field_1_sync() {
  switch (fd.vsync) {
  case 16: // First V-sync - end of F2 short pulses
    OCR1A = 16;
    ICR1 = 512;
    break;

  case 10:       // Vsync pulse 7
    OCR1A = 442; // F1 start long pulses
    break;

  case 5: // Vsync pulse 12
    OCR1A = 16;
    break;

  case 0:        // We've been slightly naughty and handled the first video gen
                 // scanline lol
    OCR1A = 26;  // Standard sync pulse length
    ICR1 = 1024; // Standard line rate
    fd.line = 7;
    fd.line_handler = &line_gen_f1;
    break;
  }
  fd.vsync--;
  return;
}

void field_2_sync() {
  switch (fd.vsync) {
  case 14: // First V-sync - end of F1 short pulses
    OCR1A = 16;
    ICR1 = 512;
    break;

  case 9:        // Vsync pulse 7
    OCR1A = 442; // F2 start long pulses
    break;

  case 4: // Vsync pulse 12
    OCR1A = 16;
    break;

  case 0:        // We've been slightly naughty and handled the first video gen
                 // scanline lol
    OCR1A = 26;  // Standard sync pulse length
    ICR1 = 1024; // Standard line rate
    fd.line = 319;
    fd.line_handler = &line_gen_f2;
    break;
  }
  fd.vsync--;
  return;
}

uint8_t off = 32;


uint16_t textY = 0;//((fd.line-48) << 2) + (fd.line-48);
void line_gen_f1(void) {
  if (fd.line >= 24 + off && fd.line <= 309 - off) {
     //__asm__ __volatile__("NOP");
     //__asm__ __volatile__("NOP");
    //_delay_us(10);
    //_delay_us(5);
    uint8_t ctr = 12;

    uint16_t INDEX = textY + cunt;
    
    for(int i = 0; i < ctr; i++){
      fd.line1[i] = pgm_read_byte(&(font8x8[textbuff[i]*8 + (textY/2)]));
    }

     //if(textY < 8){
      //fd.line1[1] = pgm_read_byte(&(font8x8[INDEX]));//font8x8[3 + (63*8) + (textY * 8)];
      //fd.line1[2] = pgm_read_byte(&(font8x8[INDEX+8]));
     //}

    textY+= 1;
    if(textY == 16){
      textY = 0;
    }

    volatile uint8_t *meme = &(fd.line1[0]);
    linegoesbrr(meme, ctr);
  }
  if (fd.line == 310) {
    fd.line_handler = &field_1_sync;
    fd.vsync = 14;
        textY = 0;
  }
  fd.line++;
}

void line_gen_f2(void) {
  if (fd.line >= 336 + off && fd.line <= 622 - off) {
    //_delay_us(10);
    //_delay_us(5);
    uint8_t ctr = 12;
    
    uint16_t INDEX = textY + cunt;
    
    for(int i = 0; i < ctr; i++){
      fd.line1[i] = pgm_read_byte(&(font8x8[textbuff[i]*8 + 3 + (textY)]));
    }

     //if(textY < 8){
      //fd.line1[1] = pgm_read_byte(&(font8x8[INDEX]));//font8x8[3 + (63*8) + (textY * 8)];
      //fd.line1[2] = pgm_read_byte(&(font8x8[INDEX+8]));
     //}

    textY+= 1;
    if(textY == 16){
      textY = 0;
    }

    volatile uint8_t *meme = &(fd.line1[0]);
    linegoesbrr(meme, ctr);
  }
  if (fd.line == 622) {
    fd.line_handler = &field_1_sync;
    fd.vsync = 16;
    textY = 0;
  }
  fd.line++;
}
