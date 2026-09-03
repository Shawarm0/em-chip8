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

typedef enum {
  QUIT,
  RUNNING,
  PAUSED,
} emulator_state_t;

typedef struct {
  emulator_state_t state;
} chip8_t;

// utils.c
bool init_sdl(sdl_t *sdl, config_t config);
bool init_chip8(chip8_t *chip8);
bool set_config_from_args(config_t *config, int argc, char **argv);
void term_sdl(const sdl_t sdl);

// sdl.c
void clear_screen(const sdl_t sdl, const config_t config);
void update_screen(const sdl_t sdl);
void handle_input(chip8_t *chip8);
void set_win_title(SDL_Window *window, const char *title);

#endif
