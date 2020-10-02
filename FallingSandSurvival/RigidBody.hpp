#pragma once


#define INC_RigidBody
#include <box2d/b2_math.h>
#include <box2d/b2_world.h>
#include <box2d/b2_body.h>
#include <box2d/b2_shape.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_fixture.h>
#include <vector>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL_gpu.h>
#include "MaterialInstance.hpp"

#include "lib/polypartition-master/src/polypartition.h"

class Item;

class RigidBody {
public:
    b2Body* body = nullptr;
    SDL_Surface* surface = nullptr;
    GPU_Image* texture = nullptr;

    int matWidth = 0;
    int matHeight = 0;
    MaterialInstance* tiles = nullptr;

    // hitbox needs update
    bool needsUpdate = false;

    // surface needs to be converted to texture
    bool texNeedsUpdate = false;

    int weldX = -1;
    int weldY = -1;
    bool back = false;
    std::list<TPPLPoly> outline;
    std::list<TPPLPoly> outline2;
    float hover = 0;

    Item* item = nullptr;

    RigidBody(b2Body* body);
    ~RigidBody();
};
