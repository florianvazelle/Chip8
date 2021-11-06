#version 450 core

/* Variables */

uniform sampler2D uTexSampler;

layout(location = 0) in vec2 iTexCoord;

layout(location = 0) out vec4 oColour;

/* Functions */

void main(void) {
  float color = texture2D(uTexSampler, iTexCoord).r * 255;
  oColour = vec4(color, color, color, 1.0);
}