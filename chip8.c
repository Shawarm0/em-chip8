#include "utils.h"

// Clear screen / SDL Window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
  // Extract rgba values from full 32 bit colour value
  const uint8_t r = (config.bg_color >> 24) & 0xFF;
  const uint8_t g = (config.bg_color >> 16) & 0xFF;
  const uint8_t b = (config.bg_color >> 8) & 0xFF;
  const uint8_t a = (config.bg_color >> 0) & 0xFF;

  SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
  SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl) { SDL_RenderPresent(sdl.renderer); }

int main(int argc, char **argv) {

  // Initialise emulator configuration/options
  config_t config = {0};
  if (!set_config_from_args(&config, argc, argv))
    exit(EXIT_FAILURE);

  // Initialise SDL
  sdl_t sdl = {0};
  if (!init_sdl(&sdl, config))
    exit(EXIT_FAILURE);

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
  term_sdl(sdl);

  exit(EXIT_SUCCESS);
}
