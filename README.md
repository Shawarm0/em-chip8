# CHIP-8 Emulator

A CHIP-8 interpreter written in C17, rendered with SDL2.

CHIP-8 is an interpreted virtual machine from the mid-1970s, originally built to
make game development on 8-bit microcomputers practical. This project implements
the full standard instruction set — 35 opcodes — together with the 64×32
monochrome display, the 16-key hexadecimal keypad, and the two 60 Hz timers.

## Features

- **Complete opcode coverage** — all 35 standard CHIP-8 instructions.
- **Cycle pacing** — a configurable instruction budget per frame (700 IPS by
  default) with the timers ticking once per frame at 60 Hz.
- **Scaled rendering** — the 64×32 framebuffer is scaled 20× to a 1280×640
  window, with optional per-pixel outlines for a CRT-like grid.
- **Pause / resume** — freeze execution without tearing down the machine state.
- **Debug tracing** — a `debug` build disassembles every executed instruction to
  stdout with live register and memory values.
- **Defensive execution** — stack overflow and underflow are detected and
  reported rather than corrupting memory, and oversized ROMs are rejected at
  load time.

## Requirements

- A C17-capable compiler (`gcc` or `clang`)
- `make`
- SDL2, with `sdl2-config` available on `PATH`

Install SDL2:

```sh
# macOS
brew install sdl2

# Debian / Ubuntu
sudo apt install libsdl2-dev

# Arch
sudo pacman -S sdl2
```

## Building

```sh
make          # optimised build  -> out/chip8
make debug    # instruction trace + debug symbols
make clean    # remove the binary
```

The build treats all warnings as errors (`-Wall -Wextra -Werror`).

## Usage

```sh
./out/chip8 <path-to-rom>
```

For example:

```sh
./out/chip8 roms/"IBM Logo.ch8"
```

`make run` builds and launches the binary, but passes no ROM path, so invoke
`out/chip8` directly for normal use.

ROMs are not distributed with this repository (`roms/` is gitignored). Public
domain CHIP-8 programs and test suites are widely available — the
[chip8-test-suite](https://github.com/Timendus/chip8-test-suite) is a good place
to verify an implementation.

## Controls

The original CHIP-8 keypad is mapped to the left-hand block of a QWERTY
keyboard, in sequential order:

| Keyboard |   |   |   |     | CHIP-8 |   |   |   |
|---|---|---|---|---|---|---|---|---|
| `1` | `2` | `3` | `4` | → | `0` | `1` | `2` | `3` |
| `Q` | `W` | `E` | `R` | → | `4` | `5` | `6` | `7` |
| `A` | `S` | `D` | `F` | → | `8` | `9` | `A` | `B` |
| `Z` | `X` | `C` | `V` | → | `C` | `D` | `E` | `F` |

| Key | Action |
|---|---|
| `Space` | Toggle pause / resume |
| `Esc` | Quit |

## Architecture

| File | Responsibility |
|---|---|
| `src/chip8.c` | Entry point; the frame loop that drives input, execution, timers, and rendering |
| `src/CPU.c` | Machine initialisation, ROM loading, fetch/decode/execute, timers, disassembler |
| `src/sdl.c` | Framebuffer rendering, keyboard event handling, window title |
| `src/utils.c` | SDL lifecycle and emulator configuration |
| `src/utils.h` | Shared types: `chip8_t`, `config_t`, `instruction_t`, `sdl_t` |

The emulated machine is a single `chip8_t`: 4 KB of memory, a 64×32 boolean
display, sixteen 8-bit data registers, a 12-level call stack, index register,
program counter, stack pointer, and the delay and sound timers. The built-in
hexadecimal font is loaded at address `0x000` and ROMs are loaded at the
conventional entry point `0x200`.

Each frame the loop drains the SDL event queue, executes
`insts_per_second / 60` instructions, decrements the timers once, redraws the
display, and sleeps for the remainder of the ~16.67 ms budget.

## Implementation notes

CHIP-8 has no single authoritative specification, and interpreters differ on
several instructions. This implementation makes the following choices:

- **`8XY4`, `8XY5`, `8XY7`, `8XY6`, `8XYE`** write `VF` *after* the result, so
  using `VF` as the destination register leaves the flag intact.
- **`8XY6` / `8XYE`** shift `VX` in place rather than shifting `VY` into `VX`
  (the CHIP-48 / SUPER-CHIP behaviour that most modern ROMs expect).
- **`FX55` / `FX65`** do not modify `I` after the transfer.
- **`BNNN`** jumps to `NNN + V0`, the original COSMAC VIP behaviour.
- **`DXYN`** wraps the starting coordinates modulo the screen dimensions, then
  clips sprites at the right and bottom edges instead of wrapping them.
- **`0NNN`** (call host machine code) is decoded and ignored, as on every
  modern interpreter.

## Known limitations

- The sound timer decrements correctly but no audio is emitted; `SDL_INIT_AUDIO`
  is requested in anticipation of it.
- Command-line arguments beyond the ROM path are accepted but not yet parsed —
  colours, scale factor, and clock rate are compile-time defaults in
  `set_config_from_args`.
- SUPER-CHIP and XO-CHIP extensions are not implemented.

## Licence

No licence has been declared for this project yet.
