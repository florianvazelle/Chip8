#pragma once

#ifdef __CHIP8_DEBUG__

#include <cassert>
#include <iostream>
#include <ostream>
#include <string>

namespace ansi {
  template <class CharT, class Traits>
  constexpr std::basic_ostream<CharT, Traits>& reset(std::basic_ostream<CharT, Traits> &os ) {
    return os << "\033[0m";
  }

  template <class CharT, class Traits>
  constexpr std::basic_ostream<CharT, Traits>& blue(std::basic_ostream<CharT, Traits> &os) {
    return os << "\033[34m";
  }

  template <class CharT, class Traits>
  constexpr std::basic_ostream<CharT, Traits> &yellow(std::basic_ostream<CharT, Traits> &os) {
    return os << "\033[33m";
  }

  template <class CharT, class Traits>
  constexpr std::basic_ostream<CharT, Traits>& red(std::basic_ostream< CharT, Traits > &os) {
    return os << "\033[31m";
  }
}

/**
 * Example usage:
 * LOG(LOG_ERROR, "Woops.");
 */

// Log something to console.
#define LOG(level, msg) level << msg << ansi::reset << std::endl

// Log an error message.
#define LOG_ERROR std::cout << ansi::red << "  [ERROR] : "

// Log a warning.
#define LOG_WARNING std::cout << ansi::yellow << "[WARNING] : "

// Log some information.
#define LOG_INFO std::cout << ansi::blue << "   [INFO] : "

// Format a hexadecimal value
#define FORMAT_HEX(value) "0x" << std::hex << std::uppercase << value

#else

#define LOG(level, msg)

#define LOG_ERROR

#define LOG_WARNING

#define LOG_INFO

#define FORMAT_HEX(value)

#endif