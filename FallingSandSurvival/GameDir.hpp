#pragma once

class GameDir {
    std::string gameDir;

public:

    GameDir(std::string gameDir) {
        this->gameDir = gameDir;
    }

    GameDir() {
        gameDir = "";
    }

    std::string getPath(std::string filePathRel);
    std::string getWorldPath(std::string worldName);
};