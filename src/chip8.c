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

  srand(time(NULL));
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

    const uint64_t frame_start = SDL_GetPerformanceCounter();

    handle_input(&chip8, sdl);

    if (chip8.state == PAUSED) {
      SDL_Delay(16); // Don't spin at 100% CPU while paused
      continue;
    }

    // Run one frame's worth of instructions, then tick the 60Hz timers once
    for (uint32_t i = 0; i < config.insts_per_second / 60; i++) {
      emulate_instruction(&chip8, config);
      if (chip8.state != RUNNING)
        break;
    }

    update_timers(&chip8);

    clear_screen(sdl, config);
    update_screen(sdl, config, chip8);

    // Sleep for whatever is left of the ~16.67ms frame
    const double elapsed_ms = (double)(SDL_GetPerformanceCounter() -
                                       frame_start) *
                              1000.0 / (double)SDL_GetPerformanceFrequency();
    if (elapsed_ms < 16.67)
      SDL_Delay((uint32_t)(16.67 - elapsed_ms));
  }

  // Final cleanup
  term_sdl(sdl);

  exit(EXIT_SUCCESS);
}
