
#include <SDL2/SDL.h>
#ifndef INC_PhysicsType
#include "PhysicsType.hpp"
#endif // !INC_PhysicsType

#include <unordered_map>

#include <string>

#define INC_Material

#define INTERACT_NONE 0
#define INTERACT_TRANSFORM_MATERIAL 1 // id, radius
#define INTERACT_SPAWN_MATERIAL 2 // id, radius
#define EXPLODE 3 // radius

#define REACT_TEMPERATURE_BELOW 4 // temperature, id
#define REACT_TEMPERATURE_ABOVE 5 // temperature, id

struct MaterialInteraction {
    int type = INTERACT_NONE;
    int data1 = 0;
    int data2 = 0;
    int ofsX = 0;
    int ofsY = 0;
};

class Material {
public:
    std::string name;
    int id = 0;
    int physicsType = 0;
    Uint8 alpha = 0;
    float density = 0;
    int iterations = 0;
    int emit = 0;
    Uint32 emitColor = 0;
    Uint32 color = 0;
    Uint32 addTemp = 0;
    float conductionSelf = 1.0;
    float conductionOther = 1.0;

    bool interact = false;
    int* nInteractions = nullptr;
    std::vector<MaterialInteraction>* interactions = nullptr;
    bool react = false;
    int nReactions = 0;
    std::vector<MaterialInteraction> reactions;

    int slipperyness = 1;

    Material(int id, std::string name, int physicsType, int slipperyness, Uint8 alpha, float density, int iterations, int emit, Uint32 emitColor, Uint32 color);
    Material(int id, std::string name, int physicsType, int slipperyness, Uint8 alpha, float density, int iterations, int emit, Uint32 emitColor) : Material(id, name, physicsType, slipperyness, alpha, density, iterations, emit, emitColor, 0xffffffff) {};
    Material(int id, std::string name, int physicsType, int slipperyness, Uint8 alpha, float density, int iterations) : Material(id, name, physicsType, slipperyness, alpha, density, iterations, 0, 0) {};
    Material(int id, std::string name, int physicsType, int slipperyness, float density, int iterations) : Material(id, name, physicsType, slipperyness, 0xff, density, iterations) {};
    Material() : Material(0, "Air", PhysicsType::AIR, 4, 0, 0) {};

};
