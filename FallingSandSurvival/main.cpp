
#include "Game.h"

#undef main

int main(int argc, char *argv[]) {
    Game* game = new Game();
    return game->init(argc, argv);
}
