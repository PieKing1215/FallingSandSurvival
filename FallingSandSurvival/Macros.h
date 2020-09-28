#pragma once


#define PIXEL(surface, x, y) *((Uint32*)(\
(Uint8*)surface->pixels + ((y) * surface->pitch) + ((x) * sizeof(Uint32)))\
)

#define QUOTE(s) #s
