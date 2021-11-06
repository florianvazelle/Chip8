#include "Context.hpp"

Context::Context(Window& window) : m_deltaTime(0), m_window(window), m_keyEvent() {}

Context::~Context() {}

void Context::ComputeDeltaTime() {
  static double t0 = 0, t;

  m_deltaTime = ((t = glfwGetTime()) - t0);
  t0 = t;
}