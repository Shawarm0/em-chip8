#include "utils.h"
#include <stdint.h>
#include <sys/types.h>

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
void emulate_instruction(chip8_t *chip8, const config_t config) {
  chip8->inst.opcode =
      (chip8->memory[chip8->PC] << 8) | chip8->memory[chip8->PC + 1];
  chip8->PC += 2; // Pre-increment program counter for next opcode

  // Fill out current instruction format

  chip8->inst.NNN = chip8->inst.opcode & 0x0FFF;
  chip8->inst.NN = chip8->inst.opcode & 0x00FF;
  chip8->inst.N = chip8->inst.opcode & 0x000F;

  chip8->inst.X = (chip8->inst.opcode >> 8) & 0x000F;
  chip8->inst.Y = (chip8->inst.opcode >> 4) & 0x000F;

#ifdef DEBUG
  print_debug_info(chip8);
#endif

  // Emulate opcode
  switch (chip8->inst.opcode & 0xF000) {
  case 0x0000:

    switch (chip8->inst.NNN) {
    case 0x0E0:
      memset(&chip8->display[0], false, sizeof(chip8->display));
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

  case 0x6000:
    chip8->V[chip8->inst.X] = chip8->inst.NN;
    break;
  case 0x7000:
    chip8->V[chip8->inst.X] += chip8->inst.NN;
    break;
  case 0x8000:

    switch (chip8->inst.N) {
    case 0x0:
      chip8->V[chip8->inst.X] = chip8->V[chip8->inst.Y];
      break;
    case 0x1:
      chip8->V[chip8->inst.X] =
          chip8->V[chip8->inst.X] | chip8->V[chip8->inst.Y];
      break;
    case 0x2:
      chip8->V[chip8->inst.X] =
          chip8->V[chip8->inst.X] & chip8->V[chip8->inst.Y];
      break;
    case 0x3:
      chip8->V[chip8->inst.X] =
          chip8->V[chip8->inst.X] ^ chip8->V[chip8->inst.Y];
      break;
    case 0x4: {
      uint16_t sum = chip8->V[chip8->inst.X] + chip8->V[chip8->inst.Y];
      uint8_t carry = (sum > 0xFF);
      chip8->V[chip8->inst.X] = (uint8_t)sum; // truncation to low 8 bits
      chip8->V[0xF] = carry;                  // after, so 8F14 leaves the flag
      break;
    }
    case 0x5: {
      uint8_t no_borrow = chip8->V[chip8->inst.X] >= chip8->V[chip8->inst.Y];
      chip8->V[chip8->inst.X] -= chip8->V[chip8->inst.Y];
      chip8->V[0xF] = no_borrow;
      break;
    }
    case 0x6: {
      uint8_t lsb = chip8->V[chip8->inst.X] & 0x1;
      chip8->V[chip8->inst.X] >>= 1;
      chip8->V[0xF] = lsb;
      break;
    }
    case 0x7: {
      uint8_t no_borrow = chip8->V[chip8->inst.Y] >= chip8->V[chip8->inst.X];
      chip8->V[chip8->inst.X] =
          chip8->V[chip8->inst.Y] - chip8->V[chip8->inst.X];
      chip8->V[0xF] = no_borrow;
      break;
    }
    case 0xE: {
      uint8_t msb = (chip8->V[chip8->inst.X] & 0x80) >> 7;
      chip8->V[chip8->inst.X] <<= 1;
      chip8->V[0xF] = msb;
      break;
    }
    }

    break;

  case 0x9000:
    if (chip8->V[chip8->inst.X] != chip8->V[chip8->inst.Y]) {
      chip8->PC += 2;
    }
    break;

  case 0xA000:
    chip8->I = chip8->inst.NNN;
    break;

  case 0xB000:
    chip8->PC = (chip8->inst.NNN + chip8->V[0x0]) & 0x0FFF;
    break;

  case 0xC000: {
    int n = rand() % 256;
    uint8_t sum = n & chip8->inst.NN;
    chip8->V[chip8->inst.X] = sum;
    break;
  }

  case 0xD000: {
    // 0xDXYN: Draw N-height sprite at coords, X,Y; read from memory location I
    // Screen pixels are XOR'd with sprite bits,
    // VF (Carry flag) is set if any screen pixels are set off; This is useful
    // for collision detection

    uint8_t X = chip8->V[chip8->inst.X] % config.window_width;
    uint8_t Y = chip8->V[chip8->inst.Y] % config.window_height;
    const uint8_t orig_X = X;

    chip8->V[0xF] = 0; // Initialize carry flag

    // loop through the rows of the sprite
    for (uint8_t i = 0; i < chip8->inst.N; i++) {
      // Get next byte/row of sprite data
      const uint8_t sprite_data = chip8->memory[chip8->I + i];
      X = orig_X; // Reset X for next row draw

      for (int8_t j = 7; j >= 0; j--) {
        bool *pixel = &chip8->display[Y * config.window_width + X];
        const bool sprite_bit = (sprite_data & (1 << j));

        if (sprite_bit && *pixel) {
          chip8->V[0xF] = 1;
        }
        *pixel ^= sprite_bit;

        // Stop drawing if hit right edge of screen;
        if (++X >= config.window_width)
          break;
      }

      // Stop drawing entire sprite if bottom edge of scren
      if (++Y >= config.window_height)
        break;
    }
    break;
  }

  default:

    puts("Not implemented");

    break; // Unimplemented or invalid opcode
  }
}

#ifdef DEBUG
void print_debug_info(chip8_t *chip8) {
  printf("Address: 0x%04X, Opcode: 0x%04X Desc: ", chip8->PC - 2,
         chip8->inst.opcode);

  switch (chip8->inst.opcode & 0xF000) {
  case 0x0000:
    switch (chip8->inst.NNN) {
    case 0x0E0:
      // 0x00E0: Clear the screen
      printf("Clear screen\n");
      break;

    case 0x0EE:
      // 0x00EE: Return from subroutine
      printf("Return from subroutine to address 0x%04X\n",
             chip8->stack[chip8->SP]);
      break;

    default:
      // 0x0NNN: Call machine code routine at NNN
      printf("Call machine code routine at NNN (0x%03X)\n", chip8->inst.NNN);
      break;
    }
    break;

  case 0x1000:
    // 0x1NNN: Jump to address NNN
    printf("Jump to address NNN (0x%03X)\n", chip8->inst.NNN);
    break;

  case 0x2000:
    // 0x2NNN: Call subroutine at NNN
    printf("Call subroutine at NNN (0x%03X)\n", chip8->inst.NNN);
    break;

  case 0x3000:
    // 0x3XNN: Skip next instruction if VX == NN
    printf("Check if V%X (0x%02X) == NN (0x%02X), skip next instruction if "
           "true\n",
           chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.NN);
    break;

  case 0x4000:
    // 0x4XNN: Skip next instruction if VX != NN
    printf("Check if V%X (0x%02X) != NN (0x%02X), skip next instruction if "
           "true\n",
           chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.NN);
    break;

  case 0x5000:
    // 0x5XY0: Skip next instruction if VX == VY
    printf("Check if V%X (0x%02X) == V%X (0x%02X), skip next instruction if "
           "true\n",
           chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
           chip8->V[chip8->inst.Y]);
    break;

  case 0x6000:
    // 0x6XNN: Set VX to NN
    printf("Set V%X = NN (0x%02X)\n", chip8->inst.X, chip8->inst.NN);
    break;

  case 0x7000:
    // 0x7XNN: Add NN to VX
    printf("Set V%X (0x%02X) += NN (0x%02X). Result: 0x%02X\n", chip8->inst.X,
           chip8->V[chip8->inst.X], chip8->inst.NN,
           (uint8_t)(chip8->V[chip8->inst.X] + chip8->inst.NN));
    break;

  case 0x8000:
    switch (chip8->inst.N) {
    case 0x0:
      // 0x8XY0: Set VX = VY
      printf("Set V%X = V%X (0x%02X)\n", chip8->inst.X, chip8->inst.Y,
             chip8->V[chip8->inst.Y]);
      break;

    case 0x1:
      // 0x8XY1: Set VX |= VY
      printf("Set V%X (0x%02X) |= V%X (0x%02X). Result: 0x%02X\n",
             chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
             chip8->V[chip8->inst.Y],
             chip8->V[chip8->inst.X] | chip8->V[chip8->inst.Y]);
      break;

    case 0x2:
      // 0x8XY2: Set VX &= VY
      printf("Set V%X (0x%02X) &= V%X (0x%02X). Result: 0x%02X\n",
             chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
             chip8->V[chip8->inst.Y],
             chip8->V[chip8->inst.X] & chip8->V[chip8->inst.Y]);
      break;

    case 0x3:
      // 0x8XY3: Set VX ^= VY
      printf("Set V%X (0x%02X) ^= V%X (0x%02X). Result: 0x%02X\n",
             chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
             chip8->V[chip8->inst.Y],
             chip8->V[chip8->inst.X] ^ chip8->V[chip8->inst.Y]);
      break;

    case 0x4:
      // 0x8XY4: Set VX += VY, VF = carry
      printf("Set V%X (0x%02X) += V%X (0x%02X), VF = carry. Result: 0x%02X, "
             "VF: %X\n",
             chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
             chip8->V[chip8->inst.Y],
             (uint8_t)(chip8->V[chip8->inst.X] + chip8->V[chip8->inst.Y]),
             (chip8->V[chip8->inst.X] + chip8->V[chip8->inst.Y]) > 0xFF);
      break;

    case 0x5:
      // 0x8XY5: Set VX -= VY, VF = !borrow
      printf("Set V%X (0x%02X) -= V%X (0x%02X), VF = !borrow. Result: 0x%02X, "
             "VF: %X\n",
             chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
             chip8->V[chip8->inst.Y],
             (uint8_t)(chip8->V[chip8->inst.X] - chip8->V[chip8->inst.Y]),
             chip8->V[chip8->inst.X] >= chip8->V[chip8->inst.Y]);
      break;

    case 0x6:
      // 0x8XY6: Set VX >>= 1, VF = shifted off bit
      printf("Set V%X (0x%02X) >>= 1, VF = shifted off bit. Result: 0x%02X, "
             "VF: %X\n",
             chip8->inst.X, chip8->V[chip8->inst.X],
             chip8->V[chip8->inst.X] >> 1, chip8->V[chip8->inst.X] & 0x1);
      break;

    case 0x7:
      // 0x8XY7: Set VX = VY - VX, VF = !borrow
      printf("Set V%X = V%X (0x%02X) - V%X (0x%02X), VF = !borrow. Result: "
             "0x%02X, VF: %X\n",
             chip8->inst.X, chip8->inst.Y, chip8->V[chip8->inst.Y],
             chip8->inst.X, chip8->V[chip8->inst.X],
             (uint8_t)(chip8->V[chip8->inst.Y] - chip8->V[chip8->inst.X]),
             chip8->V[chip8->inst.Y] >= chip8->V[chip8->inst.X]);
      break;

    case 0xE:
      // 0x8XYE: Set VX <<= 1, VF = shifted off bit
      printf("Set V%X (0x%02X) <<= 1, VF = shifted off bit. Result: 0x%02X, "
             "VF: %X\n",
             chip8->inst.X, chip8->V[chip8->inst.X],
             (uint8_t)(chip8->V[chip8->inst.X] << 1),
             (chip8->V[chip8->inst.X] & 0x80) >> 7);
      break;

    default:
      printf("Unimplemented Opcode.\n");
      break;
    }
    break;

  case 0x9000:
    // 0x9XY0: Skip next instruction if VX != VY
    printf("Check if V%X (0x%02X) != V%X (0x%02X), skip next instruction if "
           "true\n",
           chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
           chip8->V[chip8->inst.Y]);
    break;

  case 0xA000:
    // 0xANNN: Set I to NNN
    printf("Set I to NNN (0x%03X)\n", chip8->inst.NNN);
    break;

  case 0xB000:
    // 0xBNNN: Jump to V0 + NNN
    printf("Set PC to V0 (0x%02X) + NNN (0x%03X). Result: 0x%03X\n",
           chip8->V[0x0], chip8->inst.NNN,
           (chip8->V[0x0] + chip8->inst.NNN) & 0x0FFF);
    break;

  case 0xC000:
    // 0xCXNN: Set VX to rand() & NN
    printf("Set V%X = rand() %% 256 & NN (0x%02X)\n", chip8->inst.X,
           chip8->inst.NN);
    break;

  case 0xD000:
    // 0xDXYN: Draw N-height sprite at coords VX, VY from memory location I
    printf("Draw N (%u) height sprite at coords V%X (0x%02X), V%X (0x%02X) "
           "from memory location I (0x%04X). Set VF = 1 if any pixels are "
           "turned off.\n",
           chip8->inst.N, chip8->inst.X, chip8->V[chip8->inst.X], chip8->inst.Y,
           chip8->V[chip8->inst.Y], chip8->I);
    break;

  default:
    printf("Unimplemented Opcode.\n");
    break;
  }
}
#endif
