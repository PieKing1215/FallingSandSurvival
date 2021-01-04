#pragma once


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL_gpu.h>
#include <box2d/b2_distance_joint.h>
#include <unordered_map>

#include "ProfilerConfig.hpp"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

struct DrawTextParams {
    GPU_Image* t1 = nullptr;
    GPU_Image* t2 = nullptr;
    int w = -1;
    int h = -1;
};

class Drawing {
public:

    static DrawTextParams drawTextParams(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, int align);

    static DrawTextParams drawTextParams(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, bool shadow, int align);

    static void drawText(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, int align);

    static void drawText(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, bool shadow, int align);

    static void drawTextBG(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, SDL_Color bgCol, int align);

    static void drawTextBG(GPU_Target* renderer, const char* string,
        TTF_Font* font, int x, int y,
        uint8_t fR, uint8_t fG, uint8_t fB, SDL_Color bgCol, bool shadow, int align);

    static void drawText(GPU_Target* renderer, DrawTextParams pm, int x, int y, int align);

    static void drawText(GPU_Target* renderer, DrawTextParams pm, int x, int y, bool shadow, int align);

    static b2Vec2 rotate_point(float cx, float cy, float angle, b2Vec2 p);

    static void drawPolygon(GPU_Target* renderer, SDL_Color col, b2Vec2* verts, int x, int y, float scale, int count, float angle, float cx, float cy);

    static uint32 darkenColor(uint32 col, float brightness);

};
