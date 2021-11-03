#include <iostream>
#include <exception>

#include <cxxopts.hpp>

#include "Window.hpp"
#include "Chip8.hpp"
#include "Context.hpp"

int main(int argc, char** argv) try {
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

    Window window(SCREEN_COLS, SCREEN_ROWS, "Chip8 Emulator");
    Context context(window);
    window.SetWindowUserPointer(&context);

    Chip8 chip(context);
    chip.Initialize();
    chip.LoadGame(gamePath);

    window.SetDrawFrameFunc([&]() {
        // Emulate one cycle
        chip.EmulateCycle();
    
        // If the draw flag is set, update the screen
        if(chip.drawFlag) {
            chip.DrawGraphics();
            chip.drawFlag = false;
        }
    });

    window.mainLoop();

    return 0;
} catch (const std::exception& e) {
  std::cout << e.what() << std::endl;
}
