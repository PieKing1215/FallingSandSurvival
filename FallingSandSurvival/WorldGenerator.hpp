

#define INC_WorldGenerator

#ifndef INC_World
#include "world.hpp"
#endif

#include "Populator.hpp"

class World;
class Populator;

class WorldGenerator {
public:
    virtual void generateChunk(World* world, Chunk* ch) = 0;
    virtual std::vector<Populator*> getPopulators() = 0;
};
