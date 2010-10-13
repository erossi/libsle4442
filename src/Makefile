MCU = atmega16
INC = -I/usr/avr/include/
OPTLEV = 2
FCPU = 4000000UL
CFLAGS = $(INC) -Wall -Wstrict-prototypes -pedantic -mmcu=$(MCU) -O$(OPTLEV) -D F_CPU=$(FCPU)
LFLAGS = -lm

AR = avr-ar
CC = avr-gcc

LIBNAME = libsle4442.a
OBJCOPY = avr-objcopy -j .text -j .data -O ihex
OBJDUMP = avr-objdump
SIZE = avr-size
REMOVE = rm -f

objects = sle_bbg.o sle.o

GITRELEASE = "\"$(shell git describe --tags)\""

.PHONY: clean indent
.SILENT: help
.SUFFIXES: .c, .o

lib: $(objects)
	$(AR) rcs $(LIBNAME) $(objects)

clean:
	$(REMOVE) $(objects)

allclean: clean
	$(REMOVE) $(LIBNAME)

