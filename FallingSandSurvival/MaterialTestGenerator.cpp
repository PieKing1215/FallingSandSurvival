#pragma once

#ifndef INC_WorldGenerator
#include "WorldGenerator.hpp"
#endif

#ifndef INC_Textures
#include "Textures.hpp"
#endif

#include "Populators.cpp"

class MaterialTestGenerator : public WorldGenerator {
    void generateChunk(World* world, Chunk* ch) override {
        MaterialInstance* prop = new MaterialInstance[CHUNK_W * CHUNK_H];
        MaterialInstance* layer2 = new MaterialInstance[CHUNK_W * CHUNK_H];
        Uint32* background = new Uint32[CHUNK_W * CHUNK_H];
        Material* mat;

        while(true) {
            mat = Materials::MATERIALS[rand() % Materials::MATERIALS.size()];
            if(mat->id >= 31 && (mat->physicsType == PhysicsType::SAND || mat->physicsType == PhysicsType::SOUP)) break;
        }

        for(int x = 0; x < CHUNK_W; x++) {
            int px = x + ch->x * CHUNK_W;

            for(int y = 0; y < CHUNK_H; y++) {
                background[x + y * CHUNK_W] = 0x00000000;
                int py = y + ch->y * CHUNK_W;

                if(py > 400 && py <= 450) {
                    prop[x + y * CHUNK_W] = Tiles::createCobbleStone(px, py);
                } else if(ch->y == 1 && ch->x >= 1 && ch->x <= 4) {
                    if(x < 8 || y < 8 || x >= CHUNK_H - 8 || (y >= CHUNK_W - 8 && (x < 60 || x >= 68))) {
                        prop[x + y * CHUNK_W] = Tiles::createCobbleDirt(px, py);
                    } else if(y > CHUNK_H * 0.75) {
                        prop[x + y * CHUNK_W] = Tiles::create(mat, px, py);
                    } else {
                        prop[x + y * CHUNK_W] = Tiles::NOTHING;
                    }
                } else if(ch->x == 1 && py <= 400 && py > 300 && x < (py - 300)) {
                    prop[x + y * CHUNK_W] = Tiles::createCobbleStone(px, py);
                } else if(ch->x == 4 && py <= 400 && py > 300 && (CHUNK_W - x) < (py - 300)) {
                    prop[x + y * CHUNK_W] = Tiles::createCobbleStone(px, py);
                } else {
                    prop[x + y * CHUNK_W] = Tiles::NOTHING;
                }

                layer2[x + y * CHUNK_W] = Tiles::NOTHING;
            }
        }

        ch->tiles = prop;
        ch->layer2 = layer2;
        ch->background = background;
    }

    std::vector<Populator*> getPopulators() override {
        return {};
    }
};
