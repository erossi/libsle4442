# This file is part of libsle4442.
# Copyright (C) 2010, 2013 Enrico Rossi
#
# Libsle4442 is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# Libsle4442 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program.
# If not, see <http://www.gnu.org/licenses/>.

# Change the following line to match your hardware

# The tipe of MCU
MCU = atmega164p
# The clock frequency (ex. 4000000UL = 4 Mhz)
FCPU = 1000000UL

# You should not change anything below
INC = -I/usr/avr/include/
OPTLEV = 2
CFLAGS = $(INC) -Wall -Wstrict-prototypes -pedantic -mmcu=$(MCU) -O$(OPTLEV) -D F_CPU=$(FCPU) -D MAKE_LIB
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

