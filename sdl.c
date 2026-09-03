#include "SDL_video.h"
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

void handle_input(chip8_t *chip8) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {

    case SDL_QUIT:
      // TODO: Handle other key inputs.
      chip8->state = QUIT;
      return;

    case SDL_KEYDOWN:
      break;

    case SDL_KEYUP:
      break;

    default:
      break;
    }
  }
}

void set_win_title(SDL_Window *window, const char *title) {
  SDL_SetWindowTitle(window, title);
}
