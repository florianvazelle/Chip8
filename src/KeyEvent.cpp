#include "KeyEvent.hpp"

#include <glfw/glfw3.h>

static int keymap(int keycode) {
    switch (keycode) {
        case GLFW_KEY_1: return 0x1;
        case GLFW_KEY_2: return 0x2;
        case GLFW_KEY_3: return 0x3;
        case GLFW_KEY_4: return 0xc;

        case GLFW_KEY_Q: return 0x4;
        case GLFW_KEY_W: return 0x5;
        case GLFW_KEY_E: return 0x6;
        case GLFW_KEY_R: return 0xd;

        case GLFW_KEY_A: return 0x7;
        case GLFW_KEY_S: return 0x8;
        case GLFW_KEY_D: return 0x9;
        case GLFW_KEY_F: return 0xe;
                  
        case GLFW_KEY_Z: return 0xa;
        case GLFW_KEY_X: return 0x0;
        case GLFW_KEY_C: return 0xb;
        case GLFW_KEY_V: return 0xf;

        default:  return -1;
    }
}

KeyEvent::KeyEvent() {}

KeyEvent::~KeyEvent() {}

void KeyEvent::keyUp(int keycode) {
    int index = keymap(keycode);
    if (index >= 0) { m_keys[index] = true; }
}

void KeyEvent::keyDown(int keycode) {
    int index = keymap(keycode);
    if (index >= 0) { m_keys[index] = false; }
}