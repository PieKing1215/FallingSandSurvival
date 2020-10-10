#pragma once

#include "Populator.hpp"
#include "Structures.hpp"

#ifndef INC_Textures
#include "Textures.hpp"
#endif // !INC_Textures


class TestPhase1Populator : public Populator {
public:
    int getPhase() {
        return 1;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0xff0000);
            }
        }

        return {};
    }
};

class TestPhase2Populator : public Populator {
public:
    int getPhase() {
        return 2;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0x00ff00);
            }
        }

        return {};
    }
};

class TestPhase3Populator : public Populator {
public:
    int getPhase() {
        return 3;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0x0000ff);
            }
        }

        return {};
    }
};

class TestPhase4Populator : public Populator {
public:
    int getPhase() {
        return 4;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0xffff00);
            }
        }

        return {};
    }
};


class TestPhase5Populator : public Populator {
public:
    int getPhase() {
        return 5;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0xff00ff);
            }
        }

        return {};
    }
};

class TestPhase6Populator : public Populator {
public:
    int getPhase() {
        return 6;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 0; x < 10; x++) {
            for(int y = 0; y < 10; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0x00ffff);
            }
        }

        return {};
    }
};

class TestPhase0Populator : public Populator {
public:
    int getPhase() {
        return 0;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        for(int x = 10; x < 20; x++) {
            for(int y = 10; y < 20; y++) {
                chunk[x + y * CHUNK_W] = MaterialInstance(&Materials::GENERIC_SOLID, 0xffffff);
            }
        }

        return {};
    }
};

class CavePopulator : public Populator {
public:
    int getPhase() {
        return 0;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        if(ch.y < 0) return {};
        for(int x = 0; x < CHUNK_W; x++) {
            for(int y = 0; y < CHUNK_H; y++) {
                int px = x + ch.x * CHUNK_W;
                int py = y + ch.y * CHUNK_H;
                if(chunk[x + y * CHUNK_W].mat->physicsType == PhysicsType::SOLID && chunk[x + y * CHUNK_W].mat->id != Materials::CLOUD.id) {
                    double n = (world->noise.GetPerlin(px * 1.5, py * 1.5, 3802) + 1) / 2;
                    double n2 = (world->noise.GetPerlin(px / 3.0, py / 3.0, 6213) + 1) / 2;
                    double ndetail = (world->noise.GetPerlin(px * 8.0, py * 8.0, 5319) + 1) / 2 * 0.08;

                    if(n2 + n + ndetail < std::fmin(0.95, (py) / 1000.0)) {
                        double nlav = world->noise.GetPerlin(px / 4.0, py / 4.0, 7018);
                        if(nlav > 0.7) {
                            chunk[x + y * CHUNK_W] = rand() % 3 == 0 ? (ch.y > 5 ? Tiles::createLava() : Tiles::createWater()) : Tiles::NOTHING;
                        } else {
                            chunk[x + y * CHUNK_W] = Tiles::NOTHING;
                        }
                    } else {
                        double n3 = world->noise.GetPerlin(px / 64.0, py / 64.0, 9828);
                        if(n3 - 0.25 > py / 1000.0) {
                            chunk[x + y * CHUNK_W] = Tiles::NOTHING;
                        }
                    }
                }
            }
        }
        return {};
    }
};


class CobblePopulator : public Populator {
public:
    int getPhase() {
        return 1;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        if(ch.y < 0) return {};
        for(int x = 0; x < CHUNK_W; x++) {
            for(int y = 0; y < CHUNK_H; y++) {
                int px = x + ch.x * CHUNK_W;
                int py = y + ch.y * CHUNK_H;

                MaterialInstance prop = chunk[x + y * CHUNK_W];
                if(prop.mat->id == Materials::SMOOTH_STONE.id) {
                    int dist = 2 + (world->noise.GetNoise(px * 4, py * 4, 3323) + 1) / 2 * 10;
                    for(int dx = -dist; dx <= dist; dx++) {
                        for(int dy = -dist; dy <= dist; dy++) {
                            int chx = 1;
                            int chy = 1;
                            int dxx = dx;
                            int dyy = dy;
                            if(x + dx < 0) {
                                chx--;
                                dxx += CHUNK_W;
                            } else if(x + dx >= CHUNK_W) {
                                chx++;
                                dxx -= CHUNK_W;
                            }


                            if(y + dy < 0) {
                                chy--;
                                dyy += CHUNK_H;
                            } else if(y + dy >= CHUNK_H) {
                                chy++;
                                dyy -= CHUNK_H;
                            }
                            //if (x + dx >= 0 && x + dx < CHUNK_W && y + dy >= 0 && y + dy < CHUNK_H) {
                            if(area[chx + chy * 3].tiles[(x + dxx) + (y + dyy) * CHUNK_W].mat->physicsType == PhysicsType::AIR || (area[chx + chy * 3].tiles[(x + dxx) + (y + dyy) * CHUNK_W].mat->physicsType == PhysicsType::SAND && area[chx + chy * 3].tiles[(x + dxx) + (y + dyy) * CHUNK_W].mat->id != Materials::SOFT_DIRT.id)) {
                                chunk[x + y * CHUNK_W] = Tiles::createCobbleStone(px, py);
                                goto nextTile;
                            }
                            //}
                        }
                    }
                } else if(prop.mat->id == Materials::SMOOTH_DIRT.id) {
                    int dist = 2 + (world->noise.GetNoise(px * 4, py * 4, 3323) + 1) / 2 * 10;
                    for(int dx = -dist; dx <= dist; dx++) {
                        for(int dy = -dist; dy <= dist; dy++) {
                            int chx = (int)floor((x + dx) / (float)CHUNK_W) + 1;
                            int chy = (int)floor((y + dy) / (float)CHUNK_H) + 1;
                            int dxx = (CHUNK_W + ((x + dx) % CHUNK_W)) % CHUNK_W;
                            int dyy = (CHUNK_H + ((y + dy) % CHUNK_H)) % CHUNK_H;
                            //if (x + dx >= 0 && x + dx < CHUNK_W && y + dy >= 0 && y + dy < CHUNK_H) {
                            if(area[chx + chy * 3].tiles[(dxx)+(dyy)* CHUNK_W].mat->physicsType == PhysicsType::AIR || (area[chx + chy * 3].tiles[(dxx)+(dyy)* CHUNK_W].mat->physicsType == PhysicsType::SAND && area[chx + chy * 3].tiles[(dxx)+(dyy)* CHUNK_W].mat->id != Materials::SOFT_DIRT.id)) {
                                chunk[x + y * CHUNK_W] = Tiles::createCobbleDirt(px, py);
                                goto nextTile;
                            }
                            //}
                        }
                    }
                }

nextTile: {}
            }
        }

        return {};
    }
};

class OrePopulator : public Populator {
public:
    int getPhase() {
        return 0;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        if(ch.y < 0) return {};
        for(int x = 0; x < CHUNK_W; x++) {
            for(int y = 0; y < CHUNK_H; y++) {
                int px = x + ch.x * CHUNK_W;
                int py = y + ch.y * CHUNK_H;

                MaterialInstance prop = chunk[x + y * CHUNK_W];
                if(chunk[x + y * CHUNK_W].mat->id == Materials::SMOOTH_STONE.id) {
                    double n = (world->noise.GetNoise(px * 1.7, py * 1.7, 5124) + 1) / 2;
                    if(n < 0.25) chunk[x + y * CHUNK_W] = Tiles::createIron(px, py);
                }

                if(chunk[x + y * CHUNK_W].mat->id == Materials::SMOOTH_STONE.id) {
                    double n = (world->noise.GetNoise(px * 2, py * 2, 7513) + 1) / 2;
                    if(n < 0.20) chunk[x + y * CHUNK_W] = Tiles::createGold(px, py);
                }

            }
        }

        return {};
    }
};

class TreePopulator : public Populator {
public:
    int getPhase() {
        return 1;
    }

    std::vector<PlacedStructure> apply(MaterialInstance* chunk, MaterialInstance* layer2, Chunk* area, bool* dirty, int tx, int ty, int tw, int th, Chunk ch, World* world) {
        if(ch.y < 0 || ch.y > 3) return {};
        int x = rand() % (CHUNK_W / 2) + (CHUNK_W / 4);
        if(area[1 + 2 * 3].tiles[x + 0 * CHUNK_W].mat->id == Materials::SOFT_DIRT.id) return {};

        for(int y = 0; y < CHUNK_H; y++) {
            if(area[1 + 2 * 3].tiles[x + y * CHUNK_W].mat->id == Materials::SOFT_DIRT.id) {
                int px = x + ch.x * CHUNK_W;
                int py = y + (ch.y + 1) * CHUNK_W;
                /*Structure tree = Structures::makeTree1(*world, px, py);
                px -= tree.w / 2;
                py -= tree.h - 2;*/

                /*for (int tx = 0; tx < tree.w; tx++) {
                    for (int ty = 0; ty < tree.h; ty++) {
                        int chx = (int)floor((tx + px) / (float)CHUNK_W) + 1 - ch.x;
                        int chy = (int)floor((ty + py) / (float)CHUNK_H) + 1 - ch.y;
                        if (chx < 0 || chy < 0 || chx > 2 || chy > 2) continue;
                        int dxx = (CHUNK_W + ((tx + px) % CHUNK_W)) % CHUNK_W;
                        int dyy = (CHUNK_H + ((ty + py) % CHUNK_H)) % CHUNK_H;
                        if (tree.tiles[tx + ty * tree.w].mat->physicsType != PhysicsType::AIR && area[chx + chy * 3].tiles[dxx + dyy * CHUNK_W].mat->physicsType == PhysicsType::AIR && area[chx + chy * 3].layer2[dxx + dyy * CHUNK_W].mat->physicsType == PhysicsType::AIR) {
                            area[chx + chy * 3].layer2[dxx + dyy * CHUNK_W] = tree.tiles[tx + ty * tree.w];
                            dirty[chx + chy * 3] = true;
                        }
                    }
                }*/

                char buff[40];
                snprintf(buff, sizeof(buff), "assets/objects/tree%d.png", rand() % 8 + 1);
                //snprintf(buff, sizeof(buff), "assets/objects/testTree.png");
                std::string buffAsStdStr = buff;
                SDL_Surface* tex = Textures::loadTexture(buffAsStdStr.c_str());

                px -= tex->w / 2;
                py -= tex->h - 2;

                b2PolygonShape s;
                s.SetAsBox(1, 1);
                RigidBody* rb = world->makeRigidBody(b2_dynamicBody, px, py, 0, s, 1, 0.3, tex);
                for(int texX = 0; texX < tex->w; texX++) {
                    b2Filter bf = {};
                    bf.categoryBits = 0x0002;
                    bf.maskBits = 0x0001;
                    rb->body->GetFixtureList()[0].SetFilterData(bf);
                    if(((PIXEL(tex, texX, tex->h - 1) >> 24) & 0xff) != 0x00) {
                        rb->weldX = texX;
                        rb->weldY = tex->h - 1;
                        break;
                    }
                }
                world->rigidBodies.push_back(rb);
                world->updateRigidBodyHitbox(rb);

                return {};
            }
        }
        return {};
    }
};
