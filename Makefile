PORT_ID=/dev/ttyACM0
MCU=atmega328p
F_CPU=16000000UL
PROGRAMMER_ID=arduino

CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-std=c99 -Wall -g -Os -mmcu=${MCU} -DF_CPU=${F_CPU} -I.

TARGET=main
SRCS= ./src/uart.c ./src/main.c
BAUD_RATE=115200

all:
	${CC} ${CFLAGS} -o ${TARGET}.bin ${SRCS} ${PATH_DS18B20}
	${OBJCOPY} -j .text -j .data -O ihex ${TARGET}.bin ${TARGET}.hex

flash:
	avrdude -v -P ${PORT_ID} -b ${BAUD_RATE} -c ${PROGRAMMER_ID} -p ${MCU} -U flash:w:${TARGET}.hex

clean:
	rm -f *.bin *.elf *.o
