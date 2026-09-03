#include "utils.h"
#include <stdlib.h>

int main(int argc, char **argv) {

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <rom_name> \n", argv[0]);
    exit(EXIT_FAILURE);
  }

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
  const char *rom_name = argv[1];
  if (!init_chip8(&chip8, rom_name))
    exit(EXIT_FAILURE);

  // Track the state
  if (chip8.state == RUNNING) {
    set_win_title(sdl.window, "CHIP8 - RUNNING");
  }

  // Main emulator loop
  while (chip8.state != QUIT) {

    handle_input(&chip8, sdl);

    if (chip8.state == PAUSED)
      continue;

    emulate_instruction(&chip8);

    clear_screen(sdl, config);
    update_screen(sdl);

    SDL_Delay(16);
  }

  // Final cleanup
  term_sdl(sdl);

  exit(EXIT_SUCCESS);
}
