#pragma once

#include "KeyEvent.hpp"
#include "Window.hpp"
#include "Audio.hpp"

class Context {
private:
    // Timer
    double m_deltaTime;

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

    inline double dt() const { return m_deltaTime; }
    inline GLFWwindow *window() { return m_window.window(); }
    inline bool key(int i) const { return m_keyEvent.key(i); }

    /* Inline event call */

    inline void keyDown(int keycode) { m_keyEvent.keyDown(keycode); }
    inline void keyUp(int keycode) { m_keyEvent.keyUp(keycode); }

    inline void playBeep() { m_audio.playBeep(); }
    inline void stopAudio() { m_audio.stopAudio(); }

    /**
     * @brief Update the delta time
     */
    void ComputeDeltaTime();
};