CFLAGS=-std=c17 -Wall -Wextra -Werror 


all:
	gcc src/chip8.c src/utils.c src/sdl.c -o out/chip8  $(CFLAGS) `sdl2-config --cflags --libs`

clean:
	rm -f out/chip8
