#    Makefile -- default makefile
#    Copyright (C) 2020  Johannes Schmatz <johannes_schmatz@gmx.de>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

CC=gcc
CFLAGS?=-g
CFLAGS+=-std=c11 -Wall

LIBS=-lm

PROG=main

OBJS=$(patsubst %.c, %.o, $(wildcard *.c))
HEADERS=$(wildcard *.h)

all: $(PROG)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(PROG): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f $(PROG)

# syntax
# vim: ts=8 sts=8 sw=8 noet si syntax=make
