#include "SDL_keycode.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "utils.h"
#include <stdint.h>

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

void update_screen(const sdl_t sdl, const config_t config, chip8_t chip8) {
  SDL_Rect rect = {
      .x = 0, .y = 0, .w = config.scale_factor, .h = config.scale_factor};

  // Extract rgba values from full 32 bit colour value
  const uint8_t fg_r = (config.fg_color >> 24) & 0xFF;
  const uint8_t fg_g = (config.fg_color >> 16) & 0xFF;
  const uint8_t fg_b = (config.fg_color >> 8) & 0xFF;
  const uint8_t fg_a = (config.fg_color >> 0) & 0xFF;

  // Extract rgba values from full 32 bit colour value
  const uint8_t bg_r = (config.bg_color >> 24) & 0xFF;
  const uint8_t bg_g = (config.bg_color >> 16) & 0xFF;
  const uint8_t bg_b = (config.bg_color >> 8) & 0xFF;
  const uint8_t bg_a = (config.bg_color >> 0) & 0xFF;

  // Loop through display pixels, draw a rectangle per pixel to the SDL window
  for (uint32_t i = 0; i < sizeof(chip8.display); i++) {
    // Translate 1D index i value ot 2D x/y coordinates
    rect.x = (i % config.window_width) * config.scale_factor;
    rect.y = (i / config.window_width) * config.scale_factor;

    if (chip8.display[i]) {

      SDL_SetRenderDrawColor(sdl.renderer, fg_r, fg_g, fg_b, fg_a);
      SDL_RenderFillRect(sdl.renderer, &rect);

      if (config.pixel_outlines) {
        SDL_SetRenderDrawColor(sdl.renderer, bg_r, bg_g, bg_b, bg_a);
        SDL_RenderDrawRect(sdl.renderer, &rect);
      }
    } else {
      SDL_SetRenderDrawColor(sdl.renderer, bg_r, bg_g, bg_b, bg_a);
      SDL_RenderFillRect(sdl.renderer, &rect);
    }
  }

  SDL_RenderPresent(sdl.renderer);
}

void handle_input(chip8_t *chip8, sdl_t sdl) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {

    // TODO: HANDLE OTHER KEY INPUTS

    switch (event.type) {

    case SDL_QUIT:
      chip8->state = QUIT;
      return;

    case SDL_KEYDOWN:

      switch (event.key.keysym.sym) {
      case SDLK_ESCAPE:
        chip8->state = QUIT;
        return;
      case SDLK_SPACE:
        if (chip8->state == RUNNING) {
          chip8->state = PAUSED;
          set_win_title(sdl.window, "CHIP8 - PAUSED");
          puts("==== PAUSED ====");
        } else {
          chip8->state = RUNNING;
          set_win_title(sdl.window, "CHIP8 - RUNNING");
        }
        return;
      default:
        break;
      }

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
