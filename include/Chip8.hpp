#pragma once

#include "Context.hpp"
#include "Audio.hpp"

#include <iostream>

#define MAX_GAME_SIZE (0x1000 - 0x200)

#define FONTSET_ADDRESS 0x00
#define FONTSET_BYTES_PER_CHAR 5

#define PIXEL_SIZE 5

#define CLOCK_HZ 60
#define CLOCK_RATE_MS ((int)((1.0 / CLOCK_HZ) * 1000 + 0.5))

#define GFX_ROWS 32
#define GFX_COLS 64

#define SCREEN_ROWS (GFX_ROWS * PIXEL_SIZE)
#define SCREEN_COLS (GFX_COLS * PIXEL_SIZE)

/**
 * CPU core implementation.
 */
class Chip8 {
public:
    Chip8(const Context &context) : context(context) {}

    const Context& context;
    bool drawFlag = false;

    void Initialize();
    void LoadGame(const std::string &gamePath);
    void EmulateCycle();

    void DrawGraphics() {
        unsigned char screen[SCREEN_ROWS][SCREEN_COLS][3];

        // Clear framebuffer
        glClear(GL_COLOR_BUFFER_BIT);

        // Draw pixels to the buffer
        for (int row = 0; row < GFX_ROWS; row++) {
            for (int col = 0; col < GFX_COLS; col++) {
                unsigned char color = gfx[row * GFX_COLS + col] ? 255 : 0;

                // Paint Cell
                int pixel_row = row * PIXEL_SIZE;
                int pixel_col = col * PIXEL_SIZE;

                for (int drow = 0; drow < PIXEL_SIZE; drow++) {
                    for (int dcol = 0; dcol < PIXEL_SIZE; dcol++) {

                        // Paint pixel
                        int tmpr = SCREEN_ROWS - 1 - (pixel_row + drow);
                        int tmpc = pixel_col + dcol;
                        screen[tmpr][tmpc][0] = screen[tmpr][tmpc][1] = screen[tmpr][tmpc][2] = color;
                    }
                }
            }
        }

        // Update Texture
        glDrawPixels(SCREEN_COLS, SCREEN_ROWS, GL_RGB, GL_UNSIGNED_BYTE, (void *)screen);
    }

    /** 
     * Current opcode.
     * The Chip 8 has 35 opcodes which are all two bytes long. 
     * To store the current opcode, we need a data type that allows us to store two bytes. 
     * An unsigned short has the length of two bytes and therefor fits our needs.
     */
    unsigned short opcode;
    
    /**
     * Memory.
     * The Chip 8 has 4K (= 4096 bytes) memory in total, which we can emulated with a list of 4096 unsigned char.
     */
    unsigned char memory[4096];

    /**
     * Registers.
     * The Chip 8 has 15 8-bit general purpose registers named V0, V1 up to VE. 
     * The 16th register is used  for the "carry flag". 
     * Eight bits is one byte so we can use an unsigned char for this purpose.
     */
    unsigned char V[16];

    /**
     * Index register.
     * Which can have a value from 0x000 to 0xFFF.
     */
    unsigned short I;

    /**
     * Program counter.
     * Which can have a value from 0x000 to 0xFFF.
     */
    unsigned short pc;

    /**
     * Screen.
     * The graphics of the Chip 8 are black and white and the screen has a total of 2048 pixels (64 x 32).
     */
    unsigned char gfx[GFX_ROWS * GFX_COLS];

    unsigned char delayTimer;
    unsigned char soundTimer;

    /**
     * Stack.
     */
    unsigned short stack[16];

    /**
     * Stack pointer.
     */
    unsigned short sp;

    /**
     * Current key state.
     * The Chip 8 has a HEX based keypad (0x0-0xF), you can use an array to store the current state of the key.
     */
    unsigned char key[16];

    /**
     * This is the Chip 8 font set. Each number or character is 4 pixels wide and 5 pixel high.
     */
    const unsigned char FONTSET[80] = {
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
};