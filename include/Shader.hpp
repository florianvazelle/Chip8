#pragma once

#include <cstdint>
#include <stdexcept>
#include <vector>

#include <GL/glew.h>

class Shader {
private:
    // Va contenir la liste de nos shaders :
    // - Un Vertex Shader est execute pour chaque sommet (vertex)
    // - Un Geometry Shader est execute pour chaque primitive
    // - Un Fragment Shader est execute pour chaque "pixel" lors de la rasterization/remplissage de la
    // primitive
    std::vector<GLuint> m_shaders;

    // un programme fait le liens entre Vertex Shader et Fragment Shader
    GLuint m_program;

    void ValidateShader(GLenum status, GLuint type) const;
    void ValidateProgram(GLenum status, GLuint type) const;

public:
    Shader();
    ~Shader();

    /* Inline getters */

    inline GLuint &GetProgram() { return m_program; }
    inline const GLuint &GetProgram() const { return m_program; }

    /**
     * @brief Load, compile and validate any shader
     */
    void LoadShader(GLenum type, const char *filename);

    /**
     * @brief Create a program and attach all shaders to it
     */
    void Create();
};