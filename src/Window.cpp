#include "Window.hpp"

#include "Context.hpp"

#include <iostream>
#include <map>

Window::Window() : m_window(nullptr) {}

Window::Window(int width, int height, const char *title) : Window() {
  // Initialize the library
  if (!glfwInit())
    throw std::runtime_error("We cannot initialize GLFW!");

  // Create a windowed mode window and its OpenGL context
  m_window = glfwCreateWindow(width, height, title, NULL, NULL);
  if (!m_window) {
    throw std::runtime_error("We cannot create GLFW Window!");
  }

  // Make the window's context current
  glfwMakeContextCurrent(m_window);

  /* Initialize OpenGL */

  {
    GLenum error = glewInit();

    if (error != GLEW_OK) {
      throw std::runtime_error("We cannot initialize GLEW!");
    }

    std::cout << "  Version : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "   Vendor : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << " Renderer : " << glGetString(GL_RENDERER) << std::endl;
  }

  // Set key event callback
  glfwSetKeyCallback(m_window, Window::OnKey);
}

Window::~Window() {
  if (m_window)
    glfwTerminate();
}

void Window::OnKey(GLFWwindow *window, int key, int scancode, int action, int mods) {
  Context *context = static_cast<Context *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      glfwSetWindowShouldClose(window, 1);
    }

    context->keyDown(key);
  } else if (action == GLFW_RELEASE) {
    context->keyUp(key);
  }
}

void Window::mainLoop() {
  while (!glfwWindowShouldClose(m_window)) {
    m_drawFrameFunc();

    // Swap front and back buffers
    glfwSwapBuffers(m_window);

    // Poll for and process events
    glfwPollEvents();
  }
}