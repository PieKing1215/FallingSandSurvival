#pragma once


#include <SDL2/SDL.h>
#ifndef INC_Tiles
#include "Tiles.hpp"
#endif // !INC_Tiles

#include <functional>

class Particle {
public:
    MaterialInstance tile {};
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    float ax = 0;
    float ay = 0;
    float targetX = 0;
    float targetY = 0;
    float targetForce = 0;
    bool phase = false;
    bool temporary = false;
    int lifetime = 0;
    int fadeTime = 60;
    unsigned short inObjectState = 0;
    std::function<void()> killCallback = []() {};
    Particle(MaterialInstance tile, float x, float y, float vx, float vy, float ax, float ay);
    Particle(const Particle &part);
};
