CFLAGS=-std=c17 -Wall -Wextra -Werror 


all:
	gcc chip8.c -o out/chip8 $(CFLAGS) `sdl2-config --cflags --libs`
