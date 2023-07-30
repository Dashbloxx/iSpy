CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

SOURCES = $(wildcard source/*.c)
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: ispy

ispy: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

source/%.o: source/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) ispy
