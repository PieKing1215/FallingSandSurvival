
#include "Particle.hpp"
#include <iostream>

Particle::Particle(MaterialInstance tile, float x, float y, float vx, float vy, float ax, float ay) {
    this->tile = tile;
    this->x = x;
    this->y = y;
    this->vx = vx;
    this->vy = vy;
    this->ax = ax;
    this->ay = ay;
}

Particle::Particle(const Particle & part) {
    tile = part.tile;
    x = part.x;
    y = part.y;
    vx = part.vx;
    vy = part.vy;
    ax = part.ax;
    ay = part.ay;
}
