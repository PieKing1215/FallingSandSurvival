
#include "MaterialInstance.hpp"

int MaterialInstance::_curID = 1;

MaterialInstance::MaterialInstance(Material* mat, Uint32 color, int32_t temperature) {
    this->id = _curID++;
    this->mat = mat;
    this->color = color;
    this->temperature = temperature;
}

bool MaterialInstance::operator==(const MaterialInstance & other) {
    return this->id == other.id;
}
