PKG=dongs
BIN=${PKG}
SRCS=$(wildcard *.S *.c fonts/*.c)
OBJS=$(addsuffix .o,$(basename $(SRCS)))
#OBJS=${PKG}.o
MCU=atmega328p

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-Os -DF_CPU=16000000UL -mmcu=${MCU} -Wall
PORT=/dev/ttyACM0

${BIN}.hex: ${BIN}.elf
	${OBJCOPY} -O ihex $< $@

${BIN}.elf: ${OBJS}
	${CC} -mmcu=${MCU} -o $@ $^

flash: ${BIN}.hex
	avrdude -v -c avrisp2 -p ${MCU} -U flash:w:$<

clean:
	rm -f ${BIN}.elf ${BIN}.hex ${OBJS}

dump: ${BIN}.elf
	avr-objdump -d ${BIN}.elf