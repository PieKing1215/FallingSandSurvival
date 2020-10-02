

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>
#include <functional>
#ifndef INC_RigidBody
#include "RigidBody.h"
#endif

#ifndef INC_World
#include "world.h"
#endif

#include "Drawing.h"

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

class UIBackground {
public:
    virtual void draw(GPU_Target* t, SDL_Rect* bounds) = 0;
};

class SolidBackground : public UIBackground {
public:
    Uint32 color;
    void draw(GPU_Target* t, SDL_Rect* bounds);

    SolidBackground(Uint32 col) {
        this->color = col;
    }
};

class UINode {
public:
    bool visible = true;

    SDL_Rect* bounds;
    bool drawBorder = false;

    UINode* parent = NULL;
    std::vector<UINode*> children = {};

    virtual void draw(GPU_Target* t, int transformX, int transformY);

    virtual bool checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    virtual bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
        return false;
    };

    UINode(SDL_Rect* bounds) {
        this->bounds = bounds;
    }
};

class UI : public UINode {
public:
    UIBackground* background = nullptr;
    void draw(GPU_Target* t, int transformX, int transformY);

    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    UI(SDL_Rect* bounds) : UINode(bounds) {};
};

class UILabel : public UINode {
public:
    std::string text;
    TTF_Font* font;
    Uint32 textColor;
    int align;

    SDL_Surface* surface;
    GPU_Image* texture = NULL;

    void draw(GPU_Target* t, int transformX, int transformY);

    void updateTexture() {
        surface = TTF_RenderText_Solid(font, text.c_str(), {(textColor >> 16) & 0xff, (textColor >> 8) & 0xff, (textColor >> 0) & 0xff});
        texture = NULL;
    }

    UILabel(SDL_Rect* bounds, std::string text, TTF_Font* font, Uint32 textColor, int align) : UINode(bounds) {
        this->text = text;
        this->font = font;
        this->textColor = textColor;
        this->align = align;

        surface = TTF_RenderText_Solid(font, text.c_str(), {(textColor >> 16) & 0xff, (textColor >> 8) & 0xff, (textColor >> 0) & 0xff});
    }
};

class UIButton : public UILabel {
public:
    bool hovered = false;
    bool disabled = false;
    std::function<void()> selectCallback = []() {};
    std::function<void()> hoverCallback  = []() {};

    bool checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);
    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);
    void draw(GPU_Target* t, int transformX, int transformY);

    SDL_Surface* surfaceDisabled;
    GPU_Image* textureDisabled = NULL;

    void updateTexture() {
        surfaceDisabled = TTF_RenderText_Solid(font, text.c_str(), {(textColor >> 16) & 0xff, (textColor >> 8) & 0xff, (textColor >> 0) & 0xff, 0x80});
        textureDisabled = NULL;
    }

    UIButton(SDL_Rect* bounds, std::string text, TTF_Font* font, Uint32 textColor, int align) : UILabel(bounds, text, font, textColor, align) {
        surfaceDisabled = TTF_RenderText_Solid(font, text.c_str(), {(textColor >> 16) & 0xff, (textColor >> 8) & 0xff, (textColor >> 0) & 0xff, 0x80});
    };
};

class UICheckbox : public UINode {
public:
    bool checked = false;
    std::function<void(bool)> callback = [](bool checked) {};

    void draw(GPU_Target* t, int transformX, int transformY);

    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    UICheckbox(SDL_Rect* bounds, bool checked) : UINode(bounds) {
        this->checked = checked;
    }
};

class UITextArea : public UINode {
public:
    TTF_Font* font;
    std::string text;
    std::string placeholder;
    int maxLength = -1;
    std::function<void(std::string)> callback = [](std::string checked) {};
    std::function<bool(char)> isValidChar = [](char ch) { return true; };
    bool focused = false;
    bool dirty = true;
    DrawTextParams textParams;
    int cursorIndex = 0;
    long long lastCursorTimer = 0;

    void draw(GPU_Target* t, int transformX, int transformY);

    bool checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);
    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    UITextArea(SDL_Rect* bounds, std::string defaultText, std::string placeholder, TTF_Font* font) : UINode(bounds) {
        this->text = defaultText;
        this->placeholder = placeholder;
        this->font = font;
    }
};

class ChiselNode : public UINode {
public:
    RigidBody* rb = nullptr;
    SDL_Surface* surface = nullptr;
    GPU_Image* texture = nullptr;

    void draw(GPU_Target* t, int transformX, int transformY);

    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    bool checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    ChiselNode(SDL_Rect* bounds) : UINode(bounds) {};
};

class MaterialNode : public UINode {
public:
    Material* mat;
    SDL_Surface* surface;
    GPU_Image* texture = NULL;
    std::function<void(Material*)> selectCallback = [](Material* mat) {};
    std::function<void(Material*)> hoverCallback = [](Material* mat) {};

    void draw(GPU_Target* t, int transformX, int transformY);

    bool onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY);

    MaterialNode(SDL_Rect* bounds, Material* mat) : UINode(bounds) {
        this->mat = mat;
        surface = SDL_CreateRGBSurfaceWithFormat(0, bounds->w, bounds->h, 32, SDL_PIXELFORMAT_ARGB8888);
        for(int x = 0; x < bounds->w; x++) {
            for(int y = 0; y < bounds->h; y++) {
                MaterialInstance m = Tiles::create(mat, x, y);
                PIXEL(surface, x, y) = m.color + (m.mat->alpha << 24);
            }
        }
    }
};
