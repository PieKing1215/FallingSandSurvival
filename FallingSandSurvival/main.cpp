
#include "Game.hpp"

#undef main

int main(int argc, char *argv[]) {
    cxxopts::Options options("FallingSandSurvival", "Falling Sand Survival by PieKing1215: github.com/PieKing1215/FallingSandSurvival");
    options.add_options()
        ("h,help", "Print this help message")
        ("game-dir", "Set game directory", cxxopts::value<std::string>()->default_value("gamedir/"))
        ("display-mode", "Set display mode (\"windowed\", \"borderless\", \"fullscreen\")", cxxopts::value<std::string>()->default_value("windowed"))
        ("vsync", "Enable VSync")
        ("server", "Run as dedicated server")
        ("debug", "Open debug UIs by default")
        ("profiler", "Enable easy_profiler")
        ("profiler-dump", "Enable profiler dump to file on exit")
        ;

    try {
        auto result = options.parse(argc, argv);

        if(result.count("help")) {
            std::cout << options.help() << std::endl;
            std::cout << "See more details at: github.com/PieKing1215/FallingSandSurvival/wiki/Command-Line-Arguments" << std::endl;
            return 0;
        }

        Game* game = new Game();

        game->clArgs = new CLArgs(&result);

        return game->init(argc, argv);
    } catch(const cxxopts::option_not_exists_exception& e) {
        std::cerr << "Invalid command line argument: " << e.what() << std::endl;
        std::cerr << "Aborting..." << std::endl;
        return -1;
    }
}
