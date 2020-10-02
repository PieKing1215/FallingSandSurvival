
#include "Biome.hpp"

Biome::Biome(int id) {
    this->id = id;
}

Biome Biomes::DEFAULT  = Biome(0);

Biome Biomes::TEST_1   = Biome(1);
Biome Biomes::TEST_1_2 = Biome(2);
Biome Biomes::TEST_2   = Biome(3);
Biome Biomes::TEST_2_2 = Biome(4);
Biome Biomes::TEST_3   = Biome(5);
Biome Biomes::TEST_3_2 = Biome(6);
Biome Biomes::TEST_4   = Biome(7);
Biome Biomes::TEST_4_2 = Biome(8);

Biome Biomes::PLAINS = Biome(9);
Biome Biomes::MOUNTAINS = Biome(10);
Biome Biomes::FOREST = Biome(11);
