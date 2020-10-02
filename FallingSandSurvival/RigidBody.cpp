
#include "RigidBody.hpp"
#include <iostream>

RigidBody::RigidBody(b2Body* body) {
    this->body = body;
}

RigidBody::~RigidBody() {
    //if (item) delete item;
    //SDL_DestroyTexture(texture);
    //SDL_FreeSurface(surface);
}
