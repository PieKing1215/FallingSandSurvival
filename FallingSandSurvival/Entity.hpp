#define INC_Entity


#ifndef INC_RigidBody
#include "RigidBody.hpp"
#endif
#include <SDL_gpu.h>

class Entity {
public:
    float x = 0;
    float y = 0;
    float vx = 0;
    float vy = 0;
    int hw = 14;
    int hh = 26;
    bool ground = false;
    RigidBody* rb = nullptr;

    virtual void render(GPU_Target* target, int ofsX, int ofsY);
    virtual void renderLQ(GPU_Target* target, int ofsX, int ofsY);
    ~Entity();
};
