#pragma once
#include <SDL2/SDL.h>
#include <iostream>
#include <SDL2/SDL_image.h>

#define INC_Textures

class Textures {

public:
    static SDL_Surface* testTexture;
    static SDL_Surface* dirt1Texture;
    static SDL_Surface* stone1Texture;

    static SDL_Surface* smoothStone;
    static SDL_Surface* cobbleStone;
    static SDL_Surface* flatCobbleStone;
    static SDL_Surface* smoothDirt;
    static SDL_Surface* cobbleDirt;
    static SDL_Surface* flatCobbleDirt;
    static SDL_Surface* softDirt;
    static SDL_Surface* cloud;
    static SDL_Surface* gold;
    static SDL_Surface* goldMolten;
    static SDL_Surface* goldSolid;
    static SDL_Surface* iron;
    static SDL_Surface* obsidian;

    static SDL_Surface* caveBG;

    static SDL_Surface* loadTexture(std::string path);
    static SDL_Surface* loadTexture(std::string path, Uint32 pixelFormat);

    static SDL_Surface* scaleTexture(SDL_Surface*, float x, float y);
};
