#include <iostream>
#include <exception>

#include <cxxopts.hpp>

#include "Window.hpp"
#include "Chip8.hpp"
#include "Context.hpp"

#define PIXEL_SIZE 5

#define SCREEN_ROWS (GFX_ROWS * PIXEL_SIZE)
#define SCREEN_COLS (GFX_COLS * PIXEL_SIZE)

int main(int argc, char **argv) try {
  /* Command-line */

  cxxopts::Options options("chip8", "A Chip8 emulator");
  options.positional_help("GAME").show_positional_help();

  // clang-format off
  options.add_options()
      ("h,help", "Show help")
      ("g,game", "Path to a chip8 game", cxxopts::value<std::string>(), "GAME");
  ;
  // clang-format on

  options.parse_positional({"game"});

  auto result = options.parse(argc, argv);

  if (result["help"].as<bool>()) {
      std::cout << options.help();
      return 0;
  }

  std::string gamePath = "";
  if (result.count("game")) {
      gamePath = result["game"].as<std::string>();
  } else {
      std::cout << "You must specify a game to load.\n"
                << "Use « chip8 --help » for more information.\n";
      return 0;
  }

  /* Application */

  Window window(SCREEN_COLS, SCREEN_ROWS, "Chip8 Emulator");

  Context context(window);

  Chip8 app(context);

  window.SetWindowUserPointer(&context);

  app.Initialize();
  app.LoadGame(gamePath);

  window.SetDrawFrameFunc([&]() {
    context.ComputeDeltaTime();

    app.Idle(context);
    app.Display(context); 
  });

  window.mainLoop();

  return 0;
} catch (const std::exception &e) {
  std::cout << e.what() << std::endl;
}
