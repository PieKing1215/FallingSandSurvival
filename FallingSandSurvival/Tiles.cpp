
#include "Tiles.hpp"
#include "Textures.hpp"

#include "Macros.hpp"

const MaterialInstance Tiles::NOTHING       = MaterialInstance(&Materials::GENERIC_AIR   , 0x000000);
const MaterialInstance Tiles::TEST_SOLID    = MaterialInstance(&Materials::GENERIC_SOLID , 0xff0000);
const MaterialInstance Tiles::TEST_SAND     = MaterialInstance(&Materials::GENERIC_SAND  , 0xffff00);
const MaterialInstance Tiles::TEST_LIQUID   = MaterialInstance(&Materials::GENERIC_LIQUID, 0x0000ff);
const MaterialInstance Tiles::TEST_GAS      = MaterialInstance(&Materials::GENERIC_GAS   , 0x800080);
const MaterialInstance Tiles::OBJECT		= MaterialInstance(&Materials::GENERIC_OBJECT, 0x00ff00);

MaterialInstance Tiles::createTestSand() {
    Uint32 rgb = 220;
    rgb = (rgb << 8) + 155 + rand() % 30;
    rgb = (rgb << 8) + 100;
    return MaterialInstance(&Materials::TEST_SAND, rgb);
}

MaterialInstance Tiles::createTestTexturedSand(int x, int y) {
    SDL_Surface* tex = Textures::testTexture;

    int tx = x % tex->w;
    int ty = y % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);
    return MaterialInstance(&Materials::TEST_TEXTURED_SAND, rgb);
}

MaterialInstance Tiles::createTestLiquid() {
    Uint32 rgb = 0;
    rgb = (rgb << 8) + 0;
    rgb = (rgb << 8) + 255;
    return MaterialInstance(&Materials::TEST_LIQUID, rgb);
}

MaterialInstance Tiles::createStone(int x, int y) {
    SDL_Surface* tex = Textures::cobbleStone;

    int tx = x % tex->w;
    int ty = y % tex->h;

    Uint8* pixel = (Uint8*)tex->pixels;

    pixel += ((ty)* tex->pitch) + ((tx) * sizeof(Uint32));
    Uint32 rgb = *((Uint32*)pixel);

    return MaterialInstance(&Materials::STONE, rgb);
}

MaterialInstance Tiles::createGrass() {
    Uint32 rgb = 40;
    rgb = (rgb << 8) + 120 + rand() % 20;
    rgb = (rgb << 8) + 20;
    return MaterialInstance(&Materials::GRASS, rgb);
}

MaterialInstance Tiles::createDirt() {
    Uint32 rgb = 60 + rand() % 10;
    rgb = (rgb << 8) + 40;
    rgb = (rgb << 8) + 20;
    return MaterialInstance(&Materials::DIRT, rgb);
}

MaterialInstance Tiles::createSmoothStone(int x, int y) {
    SDL_Surface* tex = Textures::smoothStone;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::SMOOTH_STONE, rgb);
}

MaterialInstance Tiles::createCobbleStone(int x, int y) {
    SDL_Surface* tex = Textures::cobbleStone;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::COBBLE_STONE, rgb);
}

MaterialInstance Tiles::createSmoothDirt(int x, int y) {
    SDL_Surface* tex = Textures::smoothDirt;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::SMOOTH_DIRT, rgb);
}

MaterialInstance Tiles::createCobbleDirt(int x, int y) {
    SDL_Surface* tex = Textures::cobbleDirt;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::COBBLE_DIRT, rgb);
}

MaterialInstance Tiles::createSoftDirt(int x, int y) {
    SDL_Surface* tex = Textures::softDirt;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::SOFT_DIRT, rgb);
}

MaterialInstance Tiles::createWater() {
    Uint32 rgb = 0x00B69F;

    return MaterialInstance(&Materials::WATER, rgb, -1023);
}

MaterialInstance Tiles::createLava() {
    Uint32 rgb = 0xFF7C00;

    return MaterialInstance(&Materials::LAVA, rgb, 1024);
}

MaterialInstance Tiles::createCloud(int x, int y) {
    SDL_Surface* tex = Textures::cloud;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::CLOUD, rgb);
}

MaterialInstance Tiles::createGold(int x, int y) {
    SDL_Surface* tex = Textures::gold;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::GOLD_ORE, rgb);
}

MaterialInstance Tiles::createIron(int x, int y) {
    SDL_Surface* tex = Textures::iron;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::IRON_ORE, rgb);
}

MaterialInstance Tiles::createObsidian(int x, int y) {
    SDL_Surface* tex = Textures::obsidian;

    int tx = (tex->w + (x % tex->w)) % tex->w;
    int ty = (tex->h + (y % tex->h)) % tex->h;

    Uint32 rgb = PIXEL(tex, tx, ty);

    return MaterialInstance(&Materials::OBSIDIAN, rgb);
}

MaterialInstance Tiles::createSteam() {
    return MaterialInstance(&Materials::STEAM, 0x666666);
}

MaterialInstance Tiles::createFire() {

    Uint32 rgb = 255;
    rgb = (rgb << 8) + 100 + rand() % 50;
    rgb = (rgb << 8) + 50;

    return MaterialInstance(&Materials::FIRE, rgb);
}

MaterialInstance Tiles::create(Material* mat, int x, int y) {
    if(mat->id == Materials::TEST_SAND.id) {
        return createTestSand();
    } else if(mat->id == Materials::TEST_TEXTURED_SAND.id) {
        return createTestTexturedSand(x, y);
    } else if(mat->id == Materials::TEST_LIQUID.id) {
        return createTestLiquid();
    } else if(mat->id == Materials::STONE.id) {
        return createStone(x, y);
    } else if(mat->id == Materials::GRASS.id) {
        return createGrass();
    } else if(mat->id == Materials::DIRT.id) {
        return createDirt();
    } else if(mat->id == Materials::SMOOTH_STONE.id) {
        return createSmoothStone(x, y);
    } else if(mat->id == Materials::COBBLE_STONE.id) {
        return createCobbleStone(x, y);
    } else if(mat->id == Materials::SMOOTH_DIRT.id) {
        return createSmoothDirt(x, y);
    } else if(mat->id == Materials::COBBLE_DIRT.id) {
        return createCobbleDirt(x, y);
    } else if(mat->id == Materials::SOFT_DIRT.id) {
        return createSoftDirt(x, y);
    } else if(mat->id == Materials::WATER.id) {
        return createWater();
    } else if(mat->id == Materials::LAVA.id) {
        return createLava();
    } else if(mat->id == Materials::CLOUD.id) {
        return createCloud(x, y);
    } else if(mat->id == Materials::GOLD_ORE.id) {
        return createGold(x, y);
    } else if(mat->id == Materials::GOLD_MOLTEN.id) {
        SDL_Surface* tex = Textures::goldMolten;

        int tx = (tex->w + (x % tex->w)) % tex->w;
        int ty = (tex->h + (y % tex->h)) % tex->h;

        Uint32 rgb = PIXEL(tex, tx, ty);

        return MaterialInstance(&Materials::GOLD_MOLTEN, rgb);
    } else if(mat->id == Materials::GOLD_SOLID.id) {
        SDL_Surface* tex = Textures::goldSolid;

        int tx = (tex->w + (x % tex->w)) % tex->w;
        int ty = (tex->h + (y % tex->h)) % tex->h;

        Uint32 rgb = PIXEL(tex, tx, ty);

        return MaterialInstance(&Materials::GOLD_SOLID, rgb);
    } else if(mat->id == Materials::IRON_ORE.id) {
        return createIron(x, y);
    } else if(mat->id == Materials::OBSIDIAN.id) {
        return createObsidian(x, y);
    } else if(mat->id == Materials::STEAM.id) {
        return createSteam();
    } else if(mat->id == Materials::FIRE.id) {
        return createFire();
    } else if(mat->id == Materials::FLAT_COBBLE_STONE.id) {
        SDL_Surface* tex = Textures::flatCobbleStone;

        int tx = (tex->w + (x % tex->w)) % tex->w;
        int ty = (tex->h + (y % tex->h)) % tex->h;

        Uint32 rgb = PIXEL(tex, tx, ty);

        return MaterialInstance(&Materials::FLAT_COBBLE_STONE, rgb);
    } else if(mat->id == Materials::FLAT_COBBLE_DIRT.id) {
        SDL_Surface* tex = Textures::flatCobbleDirt;

        int tx = (tex->w + (x % tex->w)) % tex->w;
        int ty = (tex->h + (y % tex->h)) % tex->h;

        Uint32 rgb = PIXEL(tex, tx, ty);

        return MaterialInstance(&Materials::FLAT_COBBLE_DIRT, rgb);
    }

    return MaterialInstance(mat, mat->color);
}
