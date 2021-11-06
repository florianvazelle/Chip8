#pragma once

#include "Context.hpp"
#include "Audio.hpp"
#include "Renderer.hpp"

#include <iostream>

/**
 * CPU core implementation.
 */
class Chip8 {
public:
    Chip8(Context &context) : renderer() {}

    void Initialize();
    void LoadGame(const std::string &gamePath);

    void Idle(Context &context);
    void Display(Context &context);

private:
    void EmulateCycle(Context &context);
    void Tick(Context &context);

    Renderer renderer;
    double m_globalTimersDelta = 0;
    bool drawFlag = false;

    /**
     * Memory.
     * The Chip 8 has 4K (= 4096 bytes) memory in total, which we can emulated with a list of 4096 uint8_t.
     */
    uint8_t memory[4096];

    /**
     * Registers.
     * The Chip 8 has 15 8-bit general purpose registers named V0, V1 up to VE. 
     * The 16th register is used  for the "carry flag". 
     * Eight bits is one byte so we can use an uint8_t for this purpose.
     */
    uint8_t V[16];

    /**
     * Index register.
     * Which can have a value from 0x000 to 0xFFF.
     */
    uint16_t I;

    /**
     * Program counter.
     * Which can have a value from 0x000 to 0xFFF.
     */
    uint16_t pc;

    /**
     * Delay timer.
     */
    uint8_t delayTimer;

    /**
     * Sound timer.
     * The sound timer is special in that it should make the computer “beep” as long as it’s above 0.
     */
    uint8_t soundTimer;

    /**
     * Stack.
     */
    uint16_t stack[16];

    /**
     * Stack pointer.
     */
    uint16_t sp;
};