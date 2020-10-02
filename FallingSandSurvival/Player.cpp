
#include "Player.hpp"
#include "UTime.hpp"

void Player::render(GPU_Target* target, int ofsX, int ofsY) {
    Entity::render(target, ofsX, ofsY);

    if(heldItem != NULL) {
        GPU_Rect* ir = new GPU_Rect {(float)(ofsX + x + hw / 2.0 - heldItem->surface->w), (float)(ofsY + y + hh / 2.0 - heldItem->surface->h / 2), (float)heldItem->surface->w, (float)heldItem->surface->h};
        SDL_FPoint* fp = new SDL_FPoint {(float)(-ir->x + ofsX + x + hw / 2.0), (float)(-ir->y + ofsY + y + hh / 2.0)};
        GPU_SetShapeBlendMode(GPU_BlendPresetEnum::GPU_BLEND_ADD);
        //GPU_BlitTransformX(heldItem->texture, NULL, target, ir->x, ir->y, fp->x, fp->y, holdAngle, 1, 1);
        //SDL_RenderCopyExF(renderer, heldItem->texture, NULL, ir, holdAngle, fp, abs(holdAngle) > 90 ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
        GPU_BlitRectX(heldItem->texture, NULL, target, ir, holdAngle, fp->x, fp->y, abs(holdAngle) > 90 ? GPU_FLIP_VERTICAL : GPU_FLIP_NONE);
        delete ir;
        delete fp;
    }
}

b2Vec2 rotate_point2(float cx, float cy, float angle, b2Vec2 p);

void Player::setItemInHand(Item* item, World* world) {
    RigidBody* r;
    if(heldItem != NULL) {
        b2PolygonShape ps;
        ps.SetAsBox(1, 1);

        float angle = holdAngle;

        b2Vec2 pt = rotate_point2(0, 0, angle * 3.1415 / 180.0, {(float)(heldItem->surface->w / 2.0), (float)(heldItem->surface->h / 2.0)});

        r = world->makeRigidBody(b2_dynamicBody, x + hw / 2 + world->loadZone.x - pt.x + 16 * cos((holdAngle + 180) * 3.1415f / 180.0f), y + hh / 2 + world->loadZone.y - pt.y + 16 * sin((holdAngle + 180) * 3.1415f / 180.0f), angle, ps, 1, 0.3, heldItem->surface);

        //  0 -> -w/2 -h/2
        // 90 ->  w/2 -h/2
        //180 ->  w/2  h/2
        //270 -> -w/2  h/2

        float strength = 10;
        int time = Time::millis() - startThrow;

        if(time > 1000) time = 1000;

        strength += time / 1000.0 * 30;

        r->body->SetLinearVelocity({
            (float)(strength * (float)cos((holdAngle + 180) * 3.1415f / 180.0f)),
            (float)(strength * (float)sin((holdAngle + 180) * 3.1415f / 180.0f)) - 10
            });

        b2Filter bf = {};
        bf.categoryBits = 0x0001;
        //bf.maskBits = 0x0000;
        r->body->GetFixtureList()[0].SetFilterData(bf);

        r->item = heldItem;
        world->rigidBodies.push_back(r);
        world->updateRigidBodyHitbox(r);
        //SDL_DestroyTexture(heldItem->texture);
    }
    auto a = 7;
    heldItem = item;
}

Player::~Player() {
    if(heldItem) delete heldItem;
}

b2Vec2 rotate_point2(float cx, float cy, float angle, b2Vec2 p) {
    float s = sin(angle);
    float c = cos(angle);

    // translate to origin
    p.x -= cx;
    p.y -= cy;

    // rotate
    float xn = p.x * c - p.y * s;
    float yn = p.x * s + p.y * c;

    // translate back
    return b2Vec2(xn + cx, yn + cy);
}
