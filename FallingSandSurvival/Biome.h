
#define INC_Biome

class Biome {
public:
    int id;
    Biome(int id);
};

class Biomes {
public:
    static Biome DEFAULT;
    static Biome TEST_1;
    static Biome TEST_1_2;
    static Biome TEST_2;
    static Biome TEST_2_2;
    static Biome TEST_3;
    static Biome TEST_3_2;
    static Biome TEST_4;
    static Biome TEST_4_2;

    static Biome PLAINS;
    static Biome MOUNTAINS;
    static Biome FOREST;
};
