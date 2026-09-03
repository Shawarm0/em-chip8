CFLAGS = -std=c17 -Wall -Wextra -Werror
CC = gcc
TARGET = out/chip8

SRC = src/chip8.c src/utils.c src/sdl.c src/CPU.c

all:
	mkdir -p out
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) `sdl2-config --cflags --libs`

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
