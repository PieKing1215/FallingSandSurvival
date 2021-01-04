#include "GameDir.hpp"

std::string GameDir::getPath(std::string filePathRel) {
    return this->gameDir + filePathRel;
}

std::string GameDir::getWorldPath(std::string worldName) {
    return this->getPath("worlds/" + worldName);
}
