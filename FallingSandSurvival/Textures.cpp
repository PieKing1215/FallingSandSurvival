
#include "Textures.hpp"

SDL_Surface* Textures::testTexture = Textures::loadTexture("assets/textures/test.png");
SDL_Surface* Textures::dirt1Texture = Textures::loadTexture("assets/textures/testDirt.png");
SDL_Surface* Textures::stone1Texture = Textures::loadTexture("assets/textures/testStone.png");

SDL_Surface* Textures::smoothStone = Textures::loadTexture("assets/textures/smooth_stone_128x.png");
SDL_Surface* Textures::cobbleStone = Textures::loadTexture("assets/textures/cobble_stone_128x.png");
SDL_Surface* Textures::flatCobbleStone = Textures::loadTexture("assets/textures/flat_cobble_stone_128x.png");
SDL_Surface* Textures::smoothDirt = Textures::loadTexture("assets/textures/smooth_dirt_128x.png");
SDL_Surface* Textures::cobbleDirt = Textures::loadTexture("assets/textures/cobble_dirt_128x.png");
SDL_Surface* Textures::flatCobbleDirt = Textures::loadTexture("assets/textures/flat_cobble_dirt_128x.png");
SDL_Surface* Textures::softDirt = Textures::loadTexture("assets/textures/soft_dirt.png");
SDL_Surface* Textures::cloud = Textures::loadTexture("assets/textures/cloud.png");
SDL_Surface* Textures::gold = Textures::loadTexture("assets/textures/gold.png");
SDL_Surface* Textures::goldMolten = Textures::loadTexture("assets/textures/moltenGold.png");
SDL_Surface* Textures::goldSolid = Textures::loadTexture("assets/textures/solidGold.png");
SDL_Surface* Textures::iron = Textures::loadTexture("assets/textures/iron.png");
SDL_Surface* Textures::obsidian = Textures::loadTexture("assets/textures/obsidian.png");

SDL_Surface* Textures::caveBG = Textures::loadTexture("assets/backgrounds/testCave.png");

SDL_Surface* Textures::loadTexture(std::string path) {
    return loadTexture(path, SDL_PIXELFORMAT_ARGB8888);
}

SDL_Surface* Textures::loadTexture(std::string path, Uint32 pixelFormat) {
    SDL_Surface* loadedSurface = SDL_ConvertSurfaceFormat(IMG_Load(path.c_str()), pixelFormat, 0);
    if(loadedSurface == NULL) {
        logError("Unable to load image {}! SDL_image Error: {}\n", path.c_str(), IMG_GetError());
    }

    return loadedSurface;
}

SDL_Surface* Textures::scaleTexture(SDL_Surface* src, float x, float y) {
    SDL_Surface* dest = SDL_CreateRGBSurface(src->flags, src->w * x, src->h * y, src->format->BitsPerPixel, src->format->Rmask, src->format->Gmask, src->format->Bmask, src->format->Amask);

    SDL_Rect* srcR = new SDL_Rect();
    srcR->w = src->w;
    srcR->h = src->h;

    SDL_Rect* dstR = new SDL_Rect();
    dstR->w = dest->w;
    dstR->h = dest->h;

    SDL_FillRect(dest, dstR, 0x00000000);

    SDL_SetSurfaceBlendMode(src, SDL_BLENDMODE_NONE); // override instead of overlap (prevents transparent things darkening)

    SDL_BlitScaled(src, srcR, dest, dstR);

    delete srcR;
    delete dstR;

    return dest;
}
