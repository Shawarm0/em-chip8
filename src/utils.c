#include "utils.h"

/**
 * Initialises SDL, creating the window and renderer.
 *
 * @param sdl    Pointer to the SDL state to initialise.
 * @param config Emulator configuration containing window dimensions and scale.
 *
 * @return true if SDL, the window, and renderer were successfully initialised;
 *         false otherwise.
 */
bool init_sdl(sdl_t *sdl, config_t config) {

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
    SDL_Log("Could not initialise SDL subsystems! %s\n", SDL_GetError());
    return false;
  }

  sdl->window =
      SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       config.window_width * config.scale_factor,
                       config.window_height * config.scale_factor, 0);

  if (!sdl->window) {
    SDL_Log("Could not create SDL window %s\n", SDL_GetError());
    return false;
  }

  sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
  if (!sdl->renderer) {
    SDL_Log("Could not create SDL renderer %s\n", SDL_GetError());
    return false;
  }

  return true;
}

bool init_chip8(chip8_t *chip8) {
  chip8->state = RUNNING; // Default machine state to running.
  return true;
}

/** HACK: Default parameters not handled.
 *
 *  Initialises the emulator configuration with default values and
 *  overrides them using command-line arguments.
 *
 * @param config Configuration to initialise.
 * @param argc Number of command-line arguments.
 * @param argv Command-line arguments.
 *
 * @return true if the configuration was successfully initialised, false
 * otherwise.
 */
bool set_config_from_args(config_t *config, int argc, char **argv) {

  *config = (config_t){
      .window_width = 64,     // CHIP8 original X resolution
      .window_height = 32,    // CHIP8 original Y resolution
      .fg_color = 0xFFFFFFFF, // WHITE
      .bg_color = 0xFFFF00FF, // YELLOW
      .scale_factor = 20,     // Default resolution will be 1280 x 640
  };

  // Override defaults from passed in arguments
  for (int i = 1; i < argc; i++) {
    (void)argv[i]; // prevent compiler error from unused variables argv argc
  }

  return true;
}

/**
 * Terminates SDL, destroying window and renderer
 *
 * @param sdl SDL state to destroy
 *
 */
void term_sdl(const sdl_t sdl) {
  SDL_DestroyRenderer(sdl.renderer);
  SDL_DestroyWindow(sdl.window);
  SDL_Quit();
}
