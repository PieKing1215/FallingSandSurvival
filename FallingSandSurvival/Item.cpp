
#ifndef INC_Item
#include "Item.hpp"
#endif // !INC_Item

#include "Macros.hpp"

Item::Item() {}

Item::~Item() {
    GPU_FreeImage(texture);
    SDL_FreeSurface(surface);
}

Item* Item::makeItem(uint8_t flags, RigidBody* rb) {
    Item* i;

    if(rb->item != NULL) {
        i = rb->item;
        i->surface = rb->surface;
        i->texture = rb->texture;
    } else {
        i = new Item();
        i->flags = flags;
        i->surface = rb->surface;
        i->texture = rb->texture;
    }

    return i;
}

void Item::loadFillTexture(SDL_Surface* tex) {
    fill.resize(capacity);
    uint32_t maxN = 0;
    for(uint16_t x = 0; x < tex->w; x++) {
        for(uint16_t y = 0; y < tex->h; y++) {
            Uint32 col = PIXEL(tex, x, y);
            if(((col >> 24) & 0xff) > 0) {
                uint32_t n = col & 0x00ffffff;
                fill[n - 1] = {x, y};
                if(n - 1 > maxN) maxN = n - 1;
            }
        }
    }
    fill.resize(maxN);
}
