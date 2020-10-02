

#define INC_WorldGenerator

#ifndef INC_World
#include "world.h"
#endif

class World;

class WorldGenerator {
public:
    virtual void generateChunk(World* world, Chunk* ch) = 0;
};
