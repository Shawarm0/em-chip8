#ifndef UTILS_H
#define UTILS_H

#include <SDL.h>
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  SDL_Window *window;
  SDL_Renderer *renderer;
} sdl_t;

typedef struct {
  uint32_t window_width;  // SDL window width
  uint32_t window_height; // SDL window height
  uint32_t fg_color;      // Foreground color RGBA8888
  uint32_t bg_color;      // Background oclor RGBA8888
  uint32_t scale_factor;  // Amount to scale a CHIP8 pixel by e.g. 20x will be a
                          // 20x larger window
} config_t;

#endif
