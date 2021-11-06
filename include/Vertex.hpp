#pragma once

#include <gl/glew.h>
#include <glm/glm.hpp>

class Vertex {
public:
    alignas(8) glm::vec2 position;
    alignas(8) glm::vec2 texCoord;

    Vertex() : position(), texCoord() {}
    Vertex(const glm::vec2 &position, const glm::vec2 &texCoord) : position(position), texCoord(texCoord) {}

    static void SetAttribute(const GLuint &shader) {
        int loc_position = glGetAttribLocation(shader, "aPosition");
        int loc_texCoord = glGetAttribLocation(shader, "aTexCoord");

        // Indicates that the data is in array form
        glEnableVertexAttribArray(loc_position);
        glEnableVertexAttribArray(loc_texCoord);

        // Specifies the structure of the data sent to the GPU
        glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, position));
        glVertexAttribPointer(loc_texCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, texCoord));
  }
};