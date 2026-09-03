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
  uint16_t opcode;
  uint16_t NNN; // 12 bit address/constant
  uint8_t NN;   // 8 bit constant
  uint8_t N;    // 4 bit constant
  uint8_t X;    // 4 bit register identifier
  uint8_t Y;    // 4 bit register identifier
} instruction_t;

typedef struct {
  emulator_state_t state;
  uint8_t memory[4096];
  bool display[64 * 32];
  uint16_t stack[12];   // Subroutine stack
  uint8_t V[16];        // Data registers V0-VF
  uint16_t I;           // Index register
  uint16_t PC;          // Program Counter
  uint8_t delay_timer;  // Decrements at 60hz when >0
  uint8_t sound_timer;  // Decrements at 60hz and plays tone when >0
  bool keypad[16];      // Hexadecimal keypad 0x0 -0xF
  const char *rom_name; // Running ROM
  instruction_t inst;   // Currently executing instruction
} chip8_t;

// utils.c
bool init_sdl(sdl_t *sdl, config_t config);
bool set_config_from_args(config_t *config, int argc, char **argv);
void term_sdl(const sdl_t sdl);

// sdl.c
void clear_screen(const sdl_t sdl, const config_t config);
void update_screen(const sdl_t sdl);
void handle_input(chip8_t *chip8, sdl_t sdl);
void set_win_title(SDL_Window *window, const char *title);

// CPU.c
bool init_chip8(chip8_t *chip8, const char rom_name[]);
void emulate_instruction(chip8_t *chip8);

#endif
