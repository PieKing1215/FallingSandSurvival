
#include <SDL2/SDL.h>
#include <stdio.h>
#include <vector>

enum ControlMode {
    MOMENTARY,
    RISING,
    FALLING,
    TOGGLE,
    TYPE
};

enum ControlCombine {
    AND,
    OR
};

class Control {
public:
    virtual bool get() = 0;
};

class KeyControl : public Control {
public:
    SDL_Keycode key;
    ControlMode mode;

    bool raw;
    bool lastRaw;
    bool lastState;

    KeyControl(SDL_Keycode key, ControlMode mode) {
        this->key = key;
        this->mode = mode;

        raw = false;
        lastRaw = false;
        lastState = false;
    }

    virtual bool get();

};

class MultiControl : public Control {
public:
    ControlCombine combine;

    std::vector<Control*> controls;

    MultiControl(ControlCombine combine, std::vector<Control*> controls) {
        this->combine = combine;
        this->controls = controls;
    }

    virtual bool get();
};

class Controls {
public:
    static const int NUM_CONTROLS = 65535;

    static bool lmouse;
    static bool mmouse;
    static bool rmouse;

    static Control* STATS_DISPLAY;
    static Control* STATS_DISPLAY_DETAILED;

    static Control* DEBUG_UI;
    static Control* DEBUG_REFRESH;
    static Control* DEBUG_UPDATE_WORLD_MESH;
    static Control* DEBUG_TICK;

    static Control* DEBUG_EXPLODE;
    static Control* DEBUG_CARVE;

    static Control* DEBUG_RIGID;

    static Control* DEBUG_DRAW;
    static Control* DEBUG_BRUSHSIZE_INC;
    static Control* DEBUG_BRUSHSIZE_DEC;

    static Control* DEBUG_TOGGLE_PLAYER;

    static Control* PLAYER_UP;
    static Control* PLAYER_DOWN;
    static Control* PLAYER_LEFT;
    static Control* PLAYER_RIGHT;

    static Control* PAUSE;

    static std::vector<KeyControl*> keyControls;
    //static KeyControl** keyControls;
    static bool initted;

    static void keyEvent(SDL_KeyboardEvent event);

    static KeyControl* add(KeyControl* c);
};
