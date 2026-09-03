#include "utils.h"

int main(int argc, char **argv) {

  // Initialise emulator configuration/options
  config_t config = {0};
  if (!set_config_from_args(&config, argc, argv))
    exit(EXIT_FAILURE);

  // Initialise SDL
  sdl_t sdl = {0};
  if (!init_sdl(&sdl, config))
    exit(EXIT_FAILURE);

  // Initialise chip8 state
  chip8_t chip8 = {0};
  if (!init_chip8(&chip8))
    exit(EXIT_FAILURE);

  // Main emulator loop
  while (chip8.state != QUIT) {

    handle_input(&chip8);

    clear_screen(sdl, config);
    update_screen(sdl);

    SDL_Delay(16);
  }

  // Final cleanup
  term_sdl(sdl);

  exit(EXIT_SUCCESS);
}
