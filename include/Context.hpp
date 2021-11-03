#pragma once

#include "KeyEvent.hpp"
#include "Window.hpp"
#include "Audio.hpp"

class Context {
private:
    // Window
    Window m_window;

    // Input
    KeyEvent m_keyEvent;

    // Audio
    Audio m_audio;

public:
    Context(Window &window);
    ~Context();

    /* Inline getters */

    inline GLFWwindow *window() { return m_window.window(); }
    inline bool key(int i) const { return m_keyEvent.key(i); }
    inline const Audio &audio() const { return m_audio; }

    /* Inline event call */

    inline void keyDown(int keycode) { m_keyEvent.keyDown(keycode); }
    inline void keyUp(int keycode) { m_keyEvent.keyUp(keycode); }
};