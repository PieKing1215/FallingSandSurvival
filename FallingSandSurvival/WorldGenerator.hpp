

#define INC_WorldGenerator

#ifndef INC_World
#include "world.hpp"
#endif

class World;

class WorldGenerator {
public:
    virtual void generateChunk(World* world, Chunk* ch) = 0;
};
