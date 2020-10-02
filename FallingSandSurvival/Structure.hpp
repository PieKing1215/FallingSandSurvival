
#ifndef INC_Tiles
#include "Tiles.hpp"
#endif // !INC_Tiles

#define INC_Structure

class Structure {
public:
    MaterialInstance* tiles;
    int w;
    int h;

    Structure(int w, int h, MaterialInstance* tiles);
    Structure(SDL_Surface* texture, Material templ);
    Structure() = default;
};
