#include "Chip8.hpp"

#include "Log.hpp"
#include "Const.hpp"

#include <stdexcept>
#include <fstream>
#include <ios>
#include <cstring>

#include <stdlib.h> /* srand, rand */
#include <time.h>   /* time */

void Chip8::Initialize() {
    pc       = 0x200; // Program counter starts at 0x200
    I        = 0;     // Reset index register
    sp       = 0;     // Reset stack pointer
    drawFlag = true;

    // Clear display
    renderer.Clear();

    // Clear stack
    memset(stack, 0, sizeof(uint16_t) * 16);

    // Clear registers V0-VF
    memset(V, 0, sizeof(uint8_t) * 16);

    // Clear memory
    memset(memory, 0, sizeof(uint8_t) * 4096);

    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory[FONTSET_ADDRESS + i] = FONTSET[i];

    // Reset timers
    delayTimer = 0;
    soundTimer = 0;
}

void Chip8::LoadGame(const std::string& gamePath) {
    const int bufferSize = MAX_GAME_SIZE;
    char buffer[bufferSize];

    std::ifstream gameFile(gamePath, std::ios::in | std::ios::binary);

    if (!gameFile) {
        // An error occurred!
        throw std::runtime_error("Not be able to read game file!");
    }

    gameFile.read(buffer, bufferSize);

    // Start filling the memory at location: 0x200 == 512
    for (int i = 0; i < bufferSize; ++i)
        memory[i + 512] = buffer[i];

    gameFile.close();
}

void Chip8::Idle(Context& context) {
    // Emulate one cycle
    EmulateCycle(context);

    // The delay timer and the sound timer. 
    // They both work the same way; they should be decremented by one 60 times per second (ie. at 60 Hz). 
    // This is independent of the speed of the fetch/decode/execute loop below.
    const constexpr double dt = 1.0 / 60.0;
    
    m_globalTimersDelta += context.dt();
    while (m_globalTimersDelta > dt) {
        m_globalTimersDelta = 0;
        Tick(context);
    }
}

void Chip8::Display(Context& context) {
    // If the draw flag is set, update the screen
    if(drawFlag) {
        renderer.Display();
        drawFlag = false;
    }
}

void Chip8::EmulateCycle(Context& context) {
    // Fetch opcode

    /**
     * Current opcode.
     * The Chip 8 has 35 opcodes which are all two bytes long.
     * To store the current opcode, we need a data type that allows us to store two bytes.
     * An uint16_t has the length of two bytes and therefor fits our needs.
     */
    uint16_t opcode = memory[pc] << 8 | memory[pc + 1];

    // Decode opcode
    uint16_t x   = (opcode & 0x0F00) >> 8;
    uint16_t y   = (opcode & 0x00F0) >> 4;
    uint16_t n   = opcode & 0x000F; // the lowest 4 bits
    uint16_t nn  = opcode & 0x00FF; // the lowest 8 bits
    uint16_t nnn = opcode & 0x0FFF; // the lowest 12 bits

    switch (opcode & 0xF000) {
        case 0x0000:
            switch (opcode) {
                case 0x00E0: // 0x00E0: Clears the screen
                    LOG(LOG_INFO, "Clears the screen");
                    renderer.Clear();
                    drawFlag = true;
                    break;

                case 0x00EE: // 00EE: Returns from subroutine
                    LOG(LOG_INFO, "Returns from subroutine");
                    pc = stack[--sp];
                    break;

                default: // 0NNN: Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.
                    LOG(LOG_ERROR, "Unknown opcode [0x0000]: " << FORMAT_HEX(opcode));
                    exit(2);
            }
            break;

        case 0x1000: // 1NNN: Jumps to address NNN.
            LOG(LOG_INFO, "Jumps to address " << FORMAT_HEX(nnn));
            pc = nnn;
            break;

        case 0x2000: // 2NNN: Calls subroutine at NNN.
            LOG(LOG_INFO, "Call address " << FORMAT_HEX(nnn));
            stack[sp++] = pc + 2;
            pc = nnn;
            break;

        case 0x3000: // 3XNN: Skips the next instruction if VX equals NN.
            LOG(LOG_INFO, "Skips the next instruction if " << FORMAT_HEX((unsigned int)V[x]) << " equals " << FORMAT_HEX(nn));
            pc += (V[x] == nn) ? 4 : 2;
            break;

        case 0x4000: // 4XNN: Skips the next instruction if VX does not equal NN.
            LOG(LOG_INFO, "Skips the next instruction if " << FORMAT_HEX((unsigned int)V[x]) << " NOT equals " << FORMAT_HEX(n));
            pc += (V[x] != nn) ? 4 : 2;
            break;

        case 0x5000: // 5XY0: Skips the next instruction if VX equals VY.
            LOG(LOG_INFO, "Skips the next instruction if " << FORMAT_HEX((unsigned int)V[x]) << " equals " << FORMAT_HEX((unsigned int)V[y]));
            pc += (V[x] == V[y]) ? 4 : 2;
            break;

        case 0x6000: // 6XNN: Sets VX to NN.
            LOG(LOG_INFO, "Sets V[" << FORMAT_HEX(x) << "] to " << FORMAT_HEX(nn));
            V[x] = (uint8_t)nn;
            pc += 2;
            break;

        case 0x7000: // 7XNN: Adds NN to VX.
            LOG(LOG_INFO, "Adds " << FORMAT_HEX(nn) << " to V[" << FORMAT_HEX(x) << "]");
            V[x] += nn;
            pc += 2;
            break;

        case 0x8000: 
            switch (n) {
                case 0x0: // 8XY0: Sets VX to the value of VY.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[y]));
                    V[x] = V[y];
                    pc += 2;
                    break;

                case 0x1: // 8XY1: Sets VX to VX or VY.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] |= V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[y]));
                    V[x] |= V[y];
                    pc += 2;
                    break;

                case 0x2: // 8XY2: Sets VX to VX and VY.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] &= V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[y]));
                    V[x] &= V[y];
                    pc += 2;
                    break;

                case 0x3: // 8XY3: Sets VX to VX xor VY.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] ^= V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[y]));
                    V[x] ^= V[y];
                    pc += 2;
                    break;

                case 0x4: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(x) << "] + V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[x]) << " + " << FORMAT_HEX((unsigned int)V[y]));
                    V[0xF] = ((int)V[x] + (int)V[y]) > 0xFF ? 1 : 0;
                    V[x] += V[y];
                    pc += 2;
                    break;

                case 0x5: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(x) << "] - V[" << FORMAT_HEX(y) << "] = " << FORMAT_HEX((unsigned int)V[x]) << " - " << FORMAT_HEX((unsigned int)V[y]));
                    V[0xF] = (V[x] > V[y]) ? 1 : 0;
                    V[x] -= V[y];
                    pc += 2;
                    break;

                case 0x6: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(x) << "] >> 1 = " << FORMAT_HEX((unsigned int)V[x]) << " >> 1");
                    V[0xF] = V[x] & 0x1;
                    V[x] = (V[x] >> 1);
                    pc += 2;
                    break;

                case 0x7: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(y) << "] - V[" << FORMAT_HEX(x) << "] = " << FORMAT_HEX((unsigned int)V[y]) << " - " << FORMAT_HEX((unsigned int)V[x]));
                    V[0xF] = (V[y] > V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    pc += 2;
                    break;

                case 0xE: // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = V[" << FORMAT_HEX(x) << "] << 1 = " << FORMAT_HEX((unsigned int)V[x]) << " << 1");
                    V[0xF] = (V[x] >> 7) & 0x1;
                    V[x] = (V[x] << 1);
                    pc += 2;
                    break;

                default:
                    LOG(LOG_ERROR, "Unknown opcode [0x8000]: " << FORMAT_HEX(opcode));
                    exit(2);
                }
            break;

        case 0x9000: 
            switch (n) {
                case 0x0: // 9XY0: Skips the next instruction if VX does not equal VY.
                    LOG(LOG_INFO, "Skip next instruction if " << FORMAT_HEX((unsigned int)V[x]) << " != " << FORMAT_HEX((unsigned int)V[y]));
                    pc += (V[x] != V[y]) ? 4 : 2;
                    break;

                default:
                    LOG(LOG_ERROR, "Unknown opcode [0x9000]: " << FORMAT_HEX(opcode));
                    exit(2);
                }
            break;

        case 0xA000: // ANNN: Sets I to the address NNN
            LOG(LOG_INFO, "Set I to " << FORMAT_HEX(nnn));
            I = nnn;
            pc += 2;
            break;

        case 0xB000: // BNNN: Jumps to the address NNN plus V0.
            LOG(LOG_INFO, "Jump to " << FORMAT_HEX(nnn) << " + V[0] (" << FORMAT_HEX((unsigned int)V[0]) << ")");
            pc = V[0] + (nnn);
            break;

        case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = random byte");
            srand((unsigned int)time(NULL));
            V[x] = (rand() % 256) & nn;
            pc += 2;
            break;

        case 0xD000: // DXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. 
                     // Each row of 8 pixels is read as bit-coded starting from memory location I; 
                     // I value does not change after the execution of this instruction. 
                     // As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn,
                     // and to 0 if that does not happen.
            LOG(LOG_INFO, "Draw sprite at (V[" << FORMAT_HEX(x) << "], V[" << FORMAT_HEX(y) << "]) = (" << FORMAT_HEX((unsigned int)V[x]) << ", " << FORMAT_HEX((unsigned int)V[y]) << ") of height " << n);
            uint16_t pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < n; yline++) {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (renderer[(V[x] + xline + ((V[y] + yline) * GFX_COLS))] == 1)
                            V[0xF] = 1;
                        renderer[V[x] + xline + ((V[y] + yline) * GFX_COLS)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            pc += 2;
            break;

        case 0xE000: 
            switch (nn) {
                // Some opcodes //

                case 0x9E: // EX9E: Skips the next instruction if the key stored in VX is pressed.
                    LOG(LOG_INFO, "Skip next instruction if key[" << x << "] is pressed");
                    pc += context.key(V[x]) ? 4 : 2;
                    break;

                case 0xA1: // EXA1: Skips the next instruction if the key stored in VX is not pressed.
                    LOG(LOG_INFO, "Skip next instruction if key[" << x << "] is NOT pressed");
                    pc += (!context.key(V[x])) ? 4 : 2;
                    break;

                default:
                    LOG(LOG_ERROR, "Unknown opcode [0xE000]: " << FORMAT_HEX(opcode));
                    exit(2);
            }
            break;

        case 0xF000:
            switch (nn) {
                // Some opcodes //

                case 0x07: // FX07: Sets VX to the value of the delay timer.
                    LOG(LOG_INFO, "V[" << FORMAT_HEX(x) << "] = delay timer = " << (unsigned int)V[x]);
                    V[x] = delayTimer;
                    pc += 2;
                    break;

                case 0x0A: // FX0A: A key press is awaited, and then stored in VX.
                    LOG(LOG_INFO, "Wait for key instruction");
                    for (int i = 0; i < 16; i++) {
                        if (context.key(i)) {
                            V[x] = i;
                            pc += 2;
                            break;
                        }
                    }
                    break;

                case 0x15: // FX15: Sets the delay timer to VX.
                    LOG(LOG_INFO, "delay timer = V[" << FORMAT_HEX(x) << "] = " << (unsigned int)V[x]);
                    delayTimer = V[x];
                    pc += 2;
                    break;

                case 0x18: // FX18: Sets the sound timer to VX.
                    LOG(LOG_INFO, "sound timer = V[" << FORMAT_HEX(x) << "] = " << (unsigned int)V[x]);
                    soundTimer = V[x];
                    pc += 2;
                    break;

                case 0x1E: // FX1E: Adds VX to I. VF is not affected.
                    LOG(LOG_INFO, "I = I + V[" << FORMAT_HEX(x) << "] = " << FORMAT_HEX(I) << " + " << FORMAT_HEX((unsigned int)V[x]));
                    I += V[x];
                    pc += 2;
                    break;

                case 0x29: // FX29: Sets I to the location of the sprite for the character in VX.
                            // Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                    LOG(LOG_INFO, "I = location of font for character V[" << FORMAT_HEX(x) << "] = " << FORMAT_HEX((unsigned int)V[x]));
                    I = FONTSET_BYTES_PER_CHAR * V[x];
                    pc += 2;
                    break;

                case 0x33: // FX33: Stores the binary-coded decimal representation of VX, with the 
                            // most significant of three digits at the address in I, the middle digit 
                            // at I plus 1, and the least significant digit at I plus 2.
                    LOG(LOG_INFO, "Store BCD for " << (unsigned int)V[x] << " starting at address " << FORMAT_HEX(I));
                    memory[I] = (V[x] % 1000) / 100;   // hundred's digit
                    memory[I + 1] = (V[x] % 100) / 10; // ten's digit
                    memory[I + 2] = (V[x] % 10);       // one's digit
                    pc += 2;
                    break;

                case 0x55: // FX55: Stores V0 to VX (including VX) in memory starting at address I.
                            // The offset from I is increased by 1 for each value written, but I itself is left unmodified
                    LOG(LOG_INFO, "Copy sprite from registers 0 to " << FORMAT_HEX(x) << " into memory at address " << std::hex <<I);
                    for (int i = 0; i <= (x); i++)
                        memory[I + i] = V[i];
                    I += x + 1;
                    pc += 2;
                    break;

                case 0x65: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
                            // The offset from I is increased by 1 for each value written, but I itself is left unmodified.
                    LOG(LOG_INFO, "Copy sprite from memory at address " << FORMAT_HEX(x) << " into registers 0 to " << FORMAT_HEX(I));
                    for (int i = 0; i <= x; i++)
                        V[i] = memory[I + i];
                    I += x + 1;
                    pc += 2;
                    break;

                default:
                    LOG(LOG_ERROR, "Unknown opcode [0xF000]: " << FORMAT_HEX(opcode));
                    exit(2);
                }
            break;

        default:
            LOG(LOG_ERROR, "Unknown opcode: " << FORMAT_HEX(opcode));
            exit(2);
    }
}

void Chip8::Tick(Context& context) {
    // Update timers
    if (delayTimer > 0)
        --delayTimer;

    if (soundTimer > 0) {
        --soundTimer;
        if (soundTimer == 0)
            context.playBeep();
        else
            context.stopAudio();
    }
}