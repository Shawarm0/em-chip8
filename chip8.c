#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>



typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer
} sdl_t;

typedef struct {
	uint32_t window_width;
	uint32_t window_height;
} config_t;


// initialise SDL
bool init_sdl(sdl_t *sdl, config_t config) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
		SDL_Log("Could not initialise SDL subsystems! %s\n", SDL_GetError());
		return false;
	}

	sdl->window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, 
											SDL_WINDOWPOS_CENTERED, 
											config.window_width, config.window_height, 0);

	if (!sdl->window) {
		SDL_Log("Could not create SDL window %s\n", SDL_GetError());
		return false;
	}

	sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);	
	if (!sdl->renderer) {
		SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
		return false;
	}

	return true; // Success
}



// Set up initial emulator configuration from passed in arguments
bool set_config_from_args(config_t *config, int argc, char **argv) {
	// default
	
	*config = (config_t){
		.window_width = 64,    // CHIP8 original X resolution
		.window_height = 32,   // CHIP8 original Y resolution
	};

	// Override defaults from passed in arguments
	for (int i=1; i<argc; i++) {
		(void)argv[i]; // prevent compiler error from unused variables argv argc
	}

	return true;	
}


void final_cleanup(const sdl_t *sdl) {
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();
}



int main(int argc, char **argv) {


	// Initialise emulator configuration/options
	config_t config = {0};
	if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);
	

	// Initialise SDL	
	sdl_t sdl = {0};	
	if (!init_sdl(&sdl, config)) exit(EXIT_FAILURE);



	// Final cleanup
	final_cleanup(&sdl);	


	exit(EXIT_SUCCESS);
}
