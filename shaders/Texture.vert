#version 450 core

/* Variables */

layout(location = 0) in vec2 aPosition;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec2 oTexCoord;

/* Functions */

void main(void) {
    oTexCoord = aTexCoord;
    gl_Position = vec4(aPosition, 0.0, 1.0);
}