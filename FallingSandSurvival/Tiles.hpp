
#ifndef INC_MaterialInstance
#include "MaterialInstance.hpp"
#endif // !INC_MaterialInstance

#include <iostream>

#define INC_Tiles

class Tiles {
public:
    static const MaterialInstance NOTHING;
    static const MaterialInstance TEST_SOLID;
    static const MaterialInstance TEST_SAND;
    static const MaterialInstance TEST_LIQUID;
    static const MaterialInstance TEST_GAS;
    static const MaterialInstance OBJECT;

    static MaterialInstance createTestSand();
    static MaterialInstance createTestTexturedSand(int x, int y);
    static MaterialInstance createTestLiquid();

    static MaterialInstance createStone(int x, int y);
    static MaterialInstance createGrass();
    static MaterialInstance createDirt();


    static MaterialInstance createSmoothStone(int x, int y);
    static MaterialInstance createCobbleStone(int x, int y);
    static MaterialInstance createSmoothDirt(int x, int y);
    static MaterialInstance createCobbleDirt(int x, int y);

    static MaterialInstance createSoftDirt(int x, int y);

    static MaterialInstance createWater();
    static MaterialInstance createLava();

    static MaterialInstance createCloud(int x, int y);
    static MaterialInstance createGold(int x, int y);
    static MaterialInstance createIron(int x, int y);

    static MaterialInstance createObsidian(int x, int y);

    static MaterialInstance createSteam();

    static MaterialInstance createFire();

    static MaterialInstance create(Material* mat, int x, int y);

};
