
#ifndef INC_Entity
#include "Entity.h"
#endif

void Entity::render(GPU_Target* target, int ofsX, int ofsY) {
    GPU_Rectangle(target, x + ofsX, y + ofsY, x + ofsX + hw, y + ofsY + hh, {0xff, 0, 0, 0xff});
}

Entity::~Entity() {
    //delete rb;
}
