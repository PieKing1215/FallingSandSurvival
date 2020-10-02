#include "Structure.hpp"

#include "Macros.hpp"

Structure::Structure(int w, int h, MaterialInstance* tiles) {
    this->w = w;
    this->h = h;
    this->tiles = tiles;
}

Structure::Structure(SDL_Surface* texture, Material mat) {
    MaterialInstance* tiles = new MaterialInstance[texture->w * texture->h];
    for(int x = 0; x < texture->w; x++) {
        for(int y = 0; y < texture->h; y++) {
            Uint32 color = PIXEL(texture, x, y);
            int alpha = 255;
            if(texture->format->format == SDL_PIXELFORMAT_ARGB8888) {
                alpha = (color >> 24) & 0xff;
                if(alpha == 0) {
                    tiles[x + y * texture->w] = Tiles::NOTHING;
                    continue;
                }
            }
            MaterialInstance prop = MaterialInstance(&mat, color);
            tiles[x + y * texture->w] = prop;
        }
    }
    this->w = texture->w;
    this->h = texture->h;
    this->tiles = tiles;
}
