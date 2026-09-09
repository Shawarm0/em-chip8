#include "SDL_keycode.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "utils.h"
#include <stdint.h>
#include <sys/types.h>

void set_colour(const config_t config, uint8_t *r, uint8_t *g, uint8_t *b,
                uint8_t *a, bool fg_bg) {
  if (fg_bg) {
    *r = (config.fg_color >> 24) & 0xFF;
    *g = (config.fg_color >> 16) & 0xFF;
    *b = (config.fg_color >> 8) & 0xFF;
    *a = (config.fg_color >> 0) & 0xFF;
  } else {
    *r = (config.bg_color >> 24) & 0xFF;
    *g = (config.bg_color >> 16) & 0xFF;
    *b = (config.bg_color >> 8) & 0xFF;
    *a = (config.bg_color >> 0) & 0xFF;
  }
}

// Clear screen / SDL Window to background color
void clear_screen(const sdl_t sdl, const config_t config) {
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;
  uint8_t a = 0;

  set_colour(config, &r, &g, &b, &a, false);

  SDL_SetRenderDrawColor(sdl.renderer, r, g, b, a);
  SDL_RenderClear(sdl.renderer);
}

void update_screen(const sdl_t sdl, const config_t config, chip8_t chip8) {
  SDL_Rect rect = {
      .x = 0, .y = 0, .w = config.scale_factor, .h = config.scale_factor};

  // Extract rgba values from full 32 bit colour value
  uint8_t fg_r = 0;
  uint8_t fg_g = 0;
  uint8_t fg_b = 0;
  uint8_t fg_a = 0;

  // Extract rgba values from full 32 bit colour value
  uint8_t bg_r = 0;
  uint8_t bg_g = 0;
  uint8_t bg_b = 0;
  uint8_t bg_a = 0;

  set_colour(config, &fg_r, &fg_g, &fg_b, &fg_a, true);
  set_colour(config, &bg_r, &bg_g, &bg_b, &bg_a, false);

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

      case SDLK_ESCAPE: {
        chip8->state = QUIT;
        return;
      }

      case SDLK_SPACE: {
        if (chip8->state == RUNNING) {
          chip8->state = PAUSED;
          set_win_title(sdl.window, "CHIP8 - PAUSED");
          puts("==== PAUSED ====");
        } else {
          chip8->state = RUNNING;
          set_win_title(sdl.window, "CHIP8 - RUNNING");
        }
        return;
      }

      case SDLK_1: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x0] = 1;
        }
        break;
      }

      case SDLK_2: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x1] = 1;
        }
        break;
      }

      case SDLK_3: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x2] = 1;
        }
        break;
      }

      case SDLK_4: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x3] = 1;
        }
        break;
      }

      case SDLK_q: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x4] = 1;
        }
        break;
      }

      case SDLK_w: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x5] = 1;
        }
        break;
      }

      case SDLK_e: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x6] = 1;
        }
        break;
      }

      case SDLK_r: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x7] = 1;
        }
        break;
      }

      case SDLK_a: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x8] = 1;
        }
        break;
      }

      case SDLK_s: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x9] = 1;
        }
        break;
      }

      case SDLK_d: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xA] = 1;
        }
        break;
      }

      case SDLK_f: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xB] = 1;
        }
        break;
      }

      case SDLK_z: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xC] = 1;
        }
        break;
      }

      case SDLK_x: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xD] = 1;
        }
        break;
      }

      case SDLK_c: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xE] = 1;
        }
        break;
      }

      case SDLK_v: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xF] = 1;
        }
        break;
      }

      default:
        break;
      }

      break;

    case SDL_KEYUP:

      switch (event.key.keysym.sym) {

      case SDLK_1: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x0] = 0;
        }
        break;
      }

      case SDLK_2: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x1] = 0;
        }
        break;
      }

      case SDLK_3: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x2] = 0;
        }
        break;
      }

      case SDLK_4: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x3] = 0;
        }
        break;
      }

      case SDLK_q: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x4] = 0;
        }
        break;
      }

      case SDLK_w: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x5] = 0;
        }
        break;
      }

      case SDLK_e: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x6] = 0;
        }
        break;
      }

      case SDLK_r: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x7] = 0;
        }
        break;
      }

      case SDLK_a: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x8] = 0;
        }
        break;
      }

      case SDLK_s: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0x9] = 0;
        }
        break;
      }

      case SDLK_d: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xA] = 0;
        }
        break;
      }

      case SDLK_f: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xB] = 0;
        }
        break;
      }

      case SDLK_z: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xC] = 0;
        }
        break;
      }

      case SDLK_x: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xD] = 0;
        }
        break;
      }

      case SDLK_c: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xE] = 0;
        }
        break;
      }

      case SDLK_v: {
        if (chip8->state == RUNNING) {
          chip8->keypad[0xF] = 0;
        }
        break;
      }

      default: {
        puts("Unrecognised button released");
        break;
      }
      }

      break;

    default:
      break;
    }
  }
}

void set_win_title(SDL_Window *window, const char *title) {
  SDL_SetWindowTitle(window, title);
}
