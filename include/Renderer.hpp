#pragma once

#include "Shader.hpp"

#define GFX_ROWS 32
#define GFX_COLS 64

class Renderer {
public:
    Renderer();
    ~Renderer();

    /* Inline getters */

    inline uint8_t& operator[](int i) { return gfx[i]; }
    inline uint8_t operator[](int i) const { return gfx[i]; }

    void Display() const;
    void Clear();

private:
    /**
     * Screen.
     * The graphics of the Chip 8 are black and white and the screen has a total of 2048 pixels (64 x 32).
     */
    uint8_t gfx[GFX_ROWS * GFX_COLS];

    GLuint m_texture, m_vao, m_vbo, m_ibo;
    Shader m_program;
};