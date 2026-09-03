#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>



typedef struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
} sdl_t;

typedef struct {
	uint32_t window_width;  // SDL window width
	uint32_t window_height; // SDL window height
	uint32_t fg_color; 		// Foreground color RGBA8888
	uint32_t bg_color; 		// Background oclor RGBA8888
	uint32_t scale_factor;  // Amount to scale a CHIP8 pixel by e.g. 20x will be a 20x larger window
} config_t;


// initialise SDL
bool init_sdl(sdl_t *sdl, config_t config) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
		SDL_Log("Could not initialise SDL subsystems! %s\n", SDL_GetError());
		return false;
	}

	sdl->window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, 
											SDL_WINDOWPOS_CENTERED, 
											config.window_width * config.scale_factor, config.window_height * config.scale_factor, 0);

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
		.fg_color = 0xFFFFFFFF,// WHITE
		.bg_color = 0xFFFF00FF,// YELLOW
		.scale_factor = 20, // Default resolution will be 1280 x 640
	};

	// Override defaults from passed in arguments
	for (int i=1; i<argc; i++) {
		(void)argv[i]; // prevent compiler error from unused variables argv argc
	}

	return true;	
}


void final_cleanup(const sdl_t sdl) {
	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();
}

// Clear screen / SDL Window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
	// Extract rgba values from full 32 bit colour value
	const uint8_t r = (config.bg_color >> 24) & 0xFF;
	const uint8_t g = (config.bg_color >> 16) & 0xFF;
	const uint8_t b = (config.bg_color >>  8) & 0xFF;
	const uint8_t a = (config.bg_color >>  0) & 0xFF;

	printf("R: %u, G: %u, B: %u, A: %u\n", r, g, b, a);

	SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
	SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl) {
	SDL_RenderPresent(sdl.renderer);
}

int main(int argc, char **argv) {


	// Initialise emulator configuration/options
	config_t config = {0};
	if (!set_config_from_args(&config, argc, argv)) exit(EXIT_FAILURE);
	

	// Initialise SDL	
	sdl_t sdl = {0};	
	if (!init_sdl(&sdl, config)) exit(EXIT_FAILURE);
	


	// Main emulator loop
	while (true) {
		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				goto cleanup;
			}
		}


		clear_screen(sdl, config);
		update_screen(sdl);

		SDL_Delay(16);
	}

	// Final cleanup
cleanup:
	final_cleanup(sdl);	


	exit(EXIT_SUCCESS);
}
