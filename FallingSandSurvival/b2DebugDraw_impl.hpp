#pragma once

#include <box2d/box2d.h>
#include <SDL_gpu.h>

class b2DebugDraw_impl : public b2Draw {
public:
    GPU_Target* target;
    float xOfs = 0;
    float yOfs = 0;
    float scale = 1;

    b2DebugDraw_impl(GPU_Target* target);
    ~b2DebugDraw_impl();

    void Create();
    void Destroy();

    b2Vec2 transform(const b2Vec2& pt);

    SDL_Color convertColor(const b2Color& color);

    void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

    void DrawCircle(const b2Vec2& center, float radius, const b2Color& color);

    void DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color);

    void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

    void DrawTransform(const b2Transform& xf);

    void DrawPoint(const b2Vec2& p, float size, const b2Color& color);

    void DrawString(int x, int y, const char* string, ...);

    void DrawString(const b2Vec2& p, const char* string, ...);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

};
