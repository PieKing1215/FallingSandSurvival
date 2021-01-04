#pragma once

#include <vector>
#ifndef INC_World
#include "world.hpp"
#endif // !INC_World


#define INC_Populator

class World;

class Populator {
public:
    virtual int getPhase() = 0;
    virtual std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk** area, bool* dirty, int tx, int ty, int tw, int th, Chunk* ch, World* world) = 0;
};
