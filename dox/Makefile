# Copyright (C) 2013 Enrico Rossi
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

PRG_NAME = libsle4442
PRGNAME = $(PRG_NAME)
GIT_TAG = "Unknown"
# Uncomment if git tag is in use
GIT_TAG = $(shell git describe --tags|sed -e "s/^\(.*\)-.*/\1/")
PRGNAME = $(PRG_NAME)_$(GIT_TAG)_$(MCU)
MYWEBSERVER = www.tecnobrain.com:/home/doxygen/libsle4442/

.PHONY: clean indent
.SILENT: help
.SUFFIXES: .c, .o

all: doc

clean:
	rm -rf ../doc/html

version:
	# Last Git tag: $(GIT_TAG)

doc:
	cd ../ && ( cat dox/Doxyfile ; echo "PROJECT_NUMBER=$(GIT_TAG)" ) | doxygen -

install: doc
	cd ../ && ( rsync -crlvz --delete --whole-file doc/html/ $(MYWEBSERVER) )

