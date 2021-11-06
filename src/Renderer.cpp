#include "Renderer.hpp"

#include "Vertex.hpp"

static GLchar texture_vert_shader[] = {
    #include "Texture.vert.h"
};

static GLchar texture_frag_shader[] = {
    #include "Texture.frag.h"
};

Renderer::Renderer() {
    /* Load shaders */

    m_program.LoadShader(GL_VERTEX_SHADER, "C:\\Users\\ponyt\\Documents\\GitHub\\chip8\\shaders\\Texture.vert");
    m_program.LoadShader(GL_FRAGMENT_SHADER, "C:\\Users\\ponyt\\Documents\\GitHub\\chip8\\shaders\\Texture.frag");
    m_program.Create();

    GLuint shader = m_program.GetProgram();
    glUseProgram(shader);

    // Texture
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        // Create a texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, GFX_COLS, GFX_ROWS, 0, GL_RED, GL_UNSIGNED_BYTE, gfx);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // VAO, VBO and IBO for a fullscreen quad
    {
        const Vertex vertices[4] = {
            Vertex(glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)),  // Top-left
            Vertex(glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)),   // Top-right
            Vertex(glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f)),  // Bottom-right
            Vertex(glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 1.0f)), // Bottom-left
        };

        const GLuint elements[6] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
        };

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, &vertices[0], GL_STATIC_DRAW);

        glGenBuffers(1, &m_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &elements[0], GL_STATIC_DRAW);

        glUniform1i(glGetUniformLocation(shader, "uTexSampler"), 0);

        Vertex::SetAttribute(shader);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    glUseProgram(0);
}

Renderer::~Renderer() {
    glDeleteTextures(1, &m_texture);
}

void Renderer::Display() const {
    GLuint shader = m_program.GetProgram();
    glUseProgram(shader);

    {
        // Clear the screen
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // Update texture
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GFX_COLS, GFX_ROWS, GL_RED, GL_UNSIGNED_BYTE, gfx);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Draw quad
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(0);
}

void Renderer::Clear() {
    memset(gfx, 0, sizeof(uint8_t) * GFX_ROWS * GFX_COLS);
}