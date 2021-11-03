#pragma once

#include <stdexcept>

class KeyEvent {
private:
    /**
     * Current key state.
     * The Chip 8 has a HEX based keypad (0x0-0xF), you can use an array to store the current state of the key.
     */
    bool m_keys[16];

public:
    KeyEvent();
    ~KeyEvent();

    /* Inline getters */

    inline bool key(int i) const { 
        if (i >= 16)
            throw std::out_of_range("The Chip 8 has a HEX based keypad (0x0-0xF).");
        return m_keys[i];
    }

    /**
     * @brief Event call when a key is pressed
     */
    void keyUp(int keycode);

    /**
     * @brief Event call when a key is released
     */
    void keyDown(int keycode);
};