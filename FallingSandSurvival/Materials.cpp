
#include "Materials.hpp"

int Materials::nMaterials = 0;
Material Materials::GENERIC_AIR        = Material(nMaterials++, "_AIR", PhysicsType::AIR, 0, 255, 0, 0, 16, 0);
Material Materials::GENERIC_SOLID      = Material(nMaterials++, "_SOLID", PhysicsType::SOLID, 0, 255, 1, 0, 0, 0);
Material Materials::GENERIC_SAND       = Material(nMaterials++, "_SAND", PhysicsType::SAND, 20, 255, 10, 2, 0, 0);
Material Materials::GENERIC_LIQUID     = Material(nMaterials++, "_LIQUID", PhysicsType::SOUP, 0, 255, 1.5, 3, 0, 0);
Material Materials::GENERIC_GAS        = Material(nMaterials++, "_GAS", PhysicsType::GAS, 0, 255, -1, 1, 0, 0);
Material Materials::GENERIC_PASSABLE   = Material(nMaterials++, "_PASSABLE", PhysicsType::PASSABLE, 0, 255, 0, 0, 0, 0);
Material Materials::GENERIC_OBJECT     = Material(nMaterials++, "_OBJECT", PhysicsType::OBJECT, 0, 255, 1000.0, 0, 0, 0);

Material Materials::TEST_SAND          = Material(nMaterials++, "Test Sand", PhysicsType::SAND, 20, 255, 10, 2, 0, 0);
Material Materials::TEST_TEXTURED_SAND = Material(nMaterials++, "Test Textured Sand", PhysicsType::SAND, 20, 255, 10, 2, 0, 0);
Material Materials::TEST_LIQUID        = Material(nMaterials++, "Test Liquid", PhysicsType::SOUP, 0, 255, 1.5, 4, 0, 0);

Material Materials::STONE              = Material(nMaterials++, "Stone", PhysicsType::SOLID, 0, 1, 0);
Material Materials::GRASS              = Material(nMaterials++, "Grass", PhysicsType::SAND, 20, 12, 1);
Material Materials::DIRT               = Material(nMaterials++, "Dirt", PhysicsType::SAND, 8, 15, 1);

Material Materials::SMOOTH_STONE       = Material(nMaterials++, "Stone", PhysicsType::SOLID, 0, 1, 0);
Material Materials::COBBLE_STONE       = Material(nMaterials++, "Cobblestone", PhysicsType::SOLID, 0, 1, 0);
Material Materials::SMOOTH_DIRT        = Material(nMaterials++, "Ground", PhysicsType::SOLID, 0, 1, 0);
Material Materials::COBBLE_DIRT        = Material(nMaterials++, "Hard Ground", PhysicsType::SOLID, 0, 1, 0);
Material Materials::SOFT_DIRT          = Material(nMaterials++, "Dirt", PhysicsType::SOLID, 0, 15, 2);

Material Materials::WATER              = Material(nMaterials++, "Water", PhysicsType::SOUP, 0, 0x80, 1.5, 6, 40, 0x3000AFB5);
Material Materials::LAVA               = Material(nMaterials++, "Lava", PhysicsType::SOUP, 0, 0xC0, 2, 1, 40, 0xFFFF6900);

Material Materials::CLOUD              = Material(nMaterials++, "Cloud", PhysicsType::SOLID, 0, 127, 1, 0);

Material Materials::GOLD_ORE           = Material(nMaterials++, "Gold Ore", PhysicsType::SAND, 20, 255, 20, 2, 8, 0x804000);
Material Materials::GOLD_MOLTEN        = Material(nMaterials++, "Molten Gold", PhysicsType::SOUP, 0, 255, 20, 2, 8, 0x6FFF9B40);
Material Materials::GOLD_SOLID         = Material(nMaterials++, "Solid Gold", PhysicsType::SOLID, 0, 255, 20, 2, 8, 0);

Material Materials::IRON_ORE           = Material(nMaterials++, "Iron Ore", PhysicsType::SAND, 20, 255, 20, 2, 8, 0x7F442F);

Material Materials::OBSIDIAN           = Material(nMaterials++, "Obsidian", PhysicsType::SOLID, 0, 255, 1, 0, 0, 0);
Material Materials::STEAM              = Material(nMaterials++, "Steam", PhysicsType::GAS, 0, 255, -1, 1, 0, 0);

Material Materials::SOFT_DIRT_SAND     = Material(nMaterials++, "Soft Dirt Sand", PhysicsType::SAND, 8, 15, 2);

Material Materials::FIRE               = Material(nMaterials++, "Fire", PhysicsType::PASSABLE, 0, 255, 20, 1, 0, 0);

Material Materials::FLAT_COBBLE_STONE  = Material(nMaterials++, "Flat Cobblestone", PhysicsType::SOLID, 0, 1, 0);
Material Materials::FLAT_COBBLE_DIRT   = Material(nMaterials++, "Flat Hard Ground", PhysicsType::SOLID, 0, 1, 0);

std::vector<Material*> Materials::MATERIALS;
Material** Materials::MATERIALS_ARRAY;
void Materials::init() {

    Materials::GENERIC_AIR.conductionSelf = 0.8;
    Materials::GENERIC_AIR.conductionOther = 0.8;

    Materials::LAVA.conductionSelf = 0.5;
    Materials::LAVA.conductionOther = 0.7;
    Materials::LAVA.addTemp = 2;

    Materials::COBBLE_STONE.conductionSelf = 0.01;
    Materials::COBBLE_STONE.conductionOther = 0.4;

    Materials::COBBLE_DIRT.conductionSelf = 1.0;
    Materials::COBBLE_DIRT.conductionOther = 1.0;

    Materials::GOLD_ORE.conductionSelf = 1.0;
    Materials::GOLD_ORE.conductionOther = 1.0;

    Materials::GOLD_MOLTEN.conductionSelf = 1.0;
    Materials::GOLD_MOLTEN.conductionOther = 1.0;

    #define REGISTER(material) MATERIALS.insert(MATERIALS.begin() + material.id, &material);
    REGISTER(GENERIC_AIR);
    REGISTER(GENERIC_SOLID);
    REGISTER(GENERIC_SAND);
    REGISTER(GENERIC_LIQUID);
    REGISTER(GENERIC_GAS);
    REGISTER(GENERIC_PASSABLE);
    REGISTER(GENERIC_OBJECT);
    REGISTER(TEST_SAND);
    REGISTER(TEST_TEXTURED_SAND);
    REGISTER(TEST_LIQUID);
    REGISTER(STONE);
    REGISTER(GRASS);
    REGISTER(DIRT);
    REGISTER(SMOOTH_STONE);
    REGISTER(COBBLE_STONE);
    REGISTER(SMOOTH_DIRT);
    REGISTER(COBBLE_DIRT);
    REGISTER(SOFT_DIRT);
    REGISTER(WATER);
    REGISTER(LAVA);
    REGISTER(CLOUD);
    REGISTER(GOLD_ORE);
    REGISTER(GOLD_MOLTEN);
    REGISTER(GOLD_SOLID);
    REGISTER(IRON_ORE);
    REGISTER(OBSIDIAN);
    REGISTER(STEAM);
    REGISTER(SOFT_DIRT_SAND);
    REGISTER(FIRE);
    REGISTER(FLAT_COBBLE_STONE);
    REGISTER(FLAT_COBBLE_DIRT);

    Material* randMats = new Material[10];
    for(int i = 0; i < 10; i++) {
        char buff[10];
        snprintf(buff, sizeof(buff), "Mat_%d", i);
        std::string buffAsStdStr = buff;

        Uint32 rgb = rand() % 255;
        rgb = (rgb << 8) + rand() % 255;
        rgb = (rgb << 8) + rand() % 255;

        int type = rand() % 2 == 0 ? (rand() % 2 == 0 ? PhysicsType::SAND : PhysicsType::GAS) : PhysicsType::SOUP;
        float dens = 0;
        if(type == PhysicsType::SAND) {
            dens = 5 + (rand() % 1000) / 1000.0;
        } else if(type == PhysicsType::SOUP) {
            dens = 4 + (rand() % 1000) / 1000.0;
        } else if(type == PhysicsType::GAS) {
            dens = 3 + (rand() % 1000) / 1000.0;
        }
        randMats[i] = Material(nMaterials++, buff, type, 10, type == PhysicsType::SAND ? 255 : (rand() % 192 + 63), dens, rand() % 4 + 1, 0, 0, rgb);
        REGISTER(randMats[i]);
    }

    for(int j = 0; j < MATERIALS.size(); j++) {
        MATERIALS[j]->interact = false;
        MATERIALS[j]->interactions = new std::vector<MaterialInteraction>[MATERIALS.size()];
        MATERIALS[j]->nInteractions = new int[MATERIALS.size()];

        for(int k = 0; k < MATERIALS.size(); k++) {
            MATERIALS[j]->interactions[k] = {};
            MATERIALS[j]->nInteractions[k] = 0;
        }

        MATERIALS[j]->react = false;
        MATERIALS[j]->nReactions = 0;
        MATERIALS[j]->reactions = {};
    }

    for(int i = 0; i < 10; i++) {
        Material* mat = MATERIALS[randMats[i].id];
        mat->interact = false;
        int interactions = rand() % 3 + 1;
        for(int j = 0; j < interactions; j++) {
            while(true) {
                Material imat = randMats[rand() % 10];
                if(imat.id != mat->id) {
                    MATERIALS[mat->id]->nInteractions[imat.id]++;

                    MaterialInteraction inter;
                    inter.type = rand() % 2 + 1;

                    if(inter.type == INTERACT_TRANSFORM_MATERIAL) {
                        inter.data1 = randMats[rand() % 10].id;
                        inter.data2 = rand() % 4;
                        inter.ofsX = rand() % 5 - 2;
                        inter.ofsY = rand() % 5 - 2;
                    } else if(inter.type == INTERACT_SPAWN_MATERIAL) {
                        inter.data1 = randMats[rand() % 10].id;
                        inter.data2 = rand() % 4;
                        inter.ofsX = rand() % 5 - 2;
                        inter.ofsY = rand() % 5 - 2;
                    }

                    MATERIALS[mat->id]->interactions[imat.id].push_back(inter);

                    break;
                }
            }
        }
    }

    /*MATERIALS[WATER.id]->interact = true;
    MATERIALS[WATER.id]->nInteractions[LAVA.id] = 2;
    MATERIALS[WATER.id]->interactions[LAVA.id].push_back({ INTERACT_TRANSFORM_MATERIAL, OBSIDIAN.id, 3, 0, 0 });
    MATERIALS[WATER.id]->interactions[LAVA.id].push_back(MaterialInteraction{ INTERACT_SPAWN_MATERIAL, STEAM.id, 0, 0, 0 });*/

    MATERIALS[LAVA.id]->react = true;
    MATERIALS[LAVA.id]->nReactions = 1;
    MATERIALS[LAVA.id]->reactions.push_back({REACT_TEMPERATURE_BELOW, 512, OBSIDIAN.id});

    MATERIALS[WATER.id]->react = true;
    MATERIALS[WATER.id]->nReactions = 1;
    MATERIALS[WATER.id]->reactions.push_back({REACT_TEMPERATURE_ABOVE, 128, STEAM.id});

    MATERIALS[GOLD_ORE.id]->react = true;
    MATERIALS[GOLD_ORE.id]->nReactions = 1;
    MATERIALS[GOLD_ORE.id]->reactions.push_back({REACT_TEMPERATURE_ABOVE, 512, GOLD_MOLTEN.id});

    MATERIALS[GOLD_MOLTEN.id]->react = true;
    MATERIALS[GOLD_MOLTEN.id]->nReactions = 1;
    MATERIALS[GOLD_MOLTEN.id]->reactions.push_back({REACT_TEMPERATURE_BELOW, 128, GOLD_SOLID.id});

    MATERIALS_ARRAY = MATERIALS.data();

    #undef REGISTER

}
