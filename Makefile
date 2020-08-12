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
	#rm -f $(PROG)

echo:
	@echo === END ===
	@clear
