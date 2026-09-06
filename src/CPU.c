#include "utils.h"

bool init_chip8(chip8_t *chip8, const char rom_name[]) {
  const uint32_t entry_point = 0x200;
  const uint8_t font[] = {
      0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
      0x20, 0x60, 0x20, 0x20, 0x70, // 1
      0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
      0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
      0x90, 0x90, 0xF0, 0x10, 0x10, // 4
      0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
      0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
      0xF0, 0x10, 0x20, 0x40, 0x40, // 7
      0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
      0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
      0xF0, 0x90, 0xF0, 0x90, 0x90, // A
      0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
      0xF0, 0x80, 0x80, 0x80, 0xF0, // C
      0xE0, 0x90, 0x90, 0x90, 0xE0, // D
      0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
      0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // Load font
  memcpy(&chip8->memory[0], font, sizeof(font));

  // Load Rom
  FILE *rom = fopen(rom_name, "rb");
  if (!rom) {
    SDL_Log("Rom file %s is invalid or does not exist\n", rom_name);
    return false;
  }

  // Get/check rom size
  fseek(rom, 0, SEEK_END);
  const size_t rom_size = ftell(rom);
  const size_t max_size = sizeof chip8->memory - entry_point;
  rewind(rom);

  if (rom_size > max_size) {
    SDL_Log("Rom file %s is too big! Rom size: %zu, Max size allowed: %zu\n",
            rom_name, rom_size, max_size);
    return false;
  }

  // Load ROM
  if (fread(&chip8->memory[entry_point], rom_size, 1, rom) != 1) {
    SDL_Log("Could not read Rom file %s into CHIP8 memory\n", rom_name);
    return false;
  }

  fclose(rom);

  // Set chip8 machine defaults
  chip8->state = RUNNING;
  chip8->PC = entry_point;
  chip8->rom_name = rom_name;

  return true;
}

// Emulate 1 CHIP8 instruction TODO: implement this and figure out the decode
void emulate_instruction(chip8_t *chip8) {
  chip8->inst.opcode =
      (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
  chip8->PC += 2; // Pre-increment program counter for next opcode

  // Fill out current instruction format

  chip8->inst.NNN = chip8->inst.opcode & 0x0FFF;
  chip8->inst.NN = chip8->inst.opcode & 0x00FF;
  chip8->inst.N = chip8->inst.opcode & 0x000F;

  chip8->inst.X = (chip8->inst.opcode & 0x0F00) >> 8;
  chip8->inst.Y = (chip8->inst.opcode & 0x00F0) >> 4;

  // Emulate opcode
  switch (chip8->inst.opcode & 0xF000) {
  case 0x0000:
    switch (chip8->inst.NNN) {
    case 0x0E0:
      break;
    case 0x0EE:
      chip8->PC = chip8->stack[chip8->SP];
      chip8->SP -= 1;
      break;
    default:
      chip8->PC = chip8->inst.NNN;
    }
    break;
  case 0x1000:
    chip8->PC = chip8->inst.NNN;
    break;

  case 0x2000:
    chip8->SP += 1;
    chip8->stack[chip8->SP] = chip8->PC;
    chip8->PC = chip8->inst.NNN;
    break;

  case 0x3000:

    if (chip8->V[chip8->inst.X] == chip8->inst.NN) {
      chip8->PC += 2;
    };

    break;

  case 0x4000:

    if (chip8->V[chip8->inst.X] != chip8->inst.NN) {
      chip8->PC += 2;
    };

    break;
  case 0x5000:

    if (chip8->V[chip8->inst.X] == chip8->V[chip8->inst.Y]) {
      chip8->PC += 2;
    };
    break;

  }
}
