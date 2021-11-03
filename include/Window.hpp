#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <stdexcept>

class Window {
 private:
  GLFWwindow* m_window;

  std::function<void(void)> m_drawFrameFunc;

 public:
  Window();
  Window(int width, int height, const char* title);
  ~Window();

  /* Inline getters */

  inline GLFWwindow* window() { return m_window; }
  inline const GLFWwindow* window() const { return m_window; }

  /* Inline setters */
  
  inline void SetDrawFrameFunc(const std::function<void(void)>& func) { m_drawFrameFunc = func; }
  inline void SetWindowUserPointer(void* pointer) { glfwSetWindowUserPointer(m_window, pointer); }

  /**
   * @brief Key event callback
   */
  static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);

  void mainLoop();
};