
#include "Controls.hpp"

std::vector<KeyControl*> Controls::keyControls = {};
//KeyControl** Controls::keyControls = new KeyControl*[NUM_CONTROLS];
bool Controls::initted = false;

Control* Controls::STATS_DISPLAY          = add(new KeyControl(SDLK_F3, RISING));
Control* Controls::STATS_DISPLAY_DETAILED = add(new KeyControl(SDLK_LSHIFT, MOMENTARY));

Control* Controls::DEBUG_UI                = add(new KeyControl(SDLK_F4, RISING));
Control* Controls::DEBUG_REFRESH           = add(new KeyControl(SDLK_KP_0, RISING));
Control* Controls::DEBUG_UPDATE_WORLD_MESH = add(new KeyControl(SDLK_KP_1, RISING));
Control* Controls::DEBUG_TICK			   = add(new KeyControl(SDLK_KP_2, RISING));
Control* Controls::DEBUG_EXPLODE           = add(new KeyControl(SDLK_e, RISING));
Control* Controls::DEBUG_CARVE             = add(new KeyControl(SDLK_c, RISING));
Control* Controls::DEBUG_RIGID             = add(new KeyControl(SDLK_r, RISING));

Control* Controls::DEBUG_DRAW = new MultiControl(ControlCombine::AND, {add(new KeyControl(SDLK_SPACE, MOMENTARY)), add(new KeyControl(SDLK_LCTRL, MOMENTARY))});
Control* Controls::DEBUG_BRUSHSIZE_INC = add(new KeyControl(']', TYPE));
Control* Controls::DEBUG_BRUSHSIZE_DEC = add(new KeyControl('[', TYPE));

Control* Controls::DEBUG_TOGGLE_PLAYER = add(new KeyControl(SDLK_p, RISING));

Control* Controls::PLAYER_UP = new MultiControl(ControlCombine::OR, {add(new KeyControl(SDLK_w, MOMENTARY)), add(new KeyControl(SDLK_SPACE, MOMENTARY))});
Control* Controls::PLAYER_LEFT  = add(new KeyControl(SDLK_a, MOMENTARY));
Control* Controls::PLAYER_DOWN  = add(new KeyControl(SDLK_s, MOMENTARY));
Control* Controls::PLAYER_RIGHT = add(new KeyControl(SDLK_d, MOMENTARY));

Control* Controls::PAUSE = add(new KeyControl(SDLK_ESCAPE, RISING));

bool Controls::lmouse = false;
bool Controls::mmouse = false;
bool Controls::rmouse = false;

void Controls::keyEvent(SDL_KeyboardEvent event) {
    //if (keyControls[event.keysym.sym]) {}

    logDebug("SDL_KEYEV {0:c} {0:d} {0:d}", event.keysym.sym, event.repeat, event.keysym.sym);

    for(auto& v : keyControls) {
        if(v->key == event.keysym.sym) {
            logDebug("match KEY {0:d}", event.keysym.sym);

            bool newState = false;
            switch(event.type) {
            case SDL_KEYDOWN:
                //logDebug("SDL_KEYDOWN {0:c} {0:d} {0:d}", event.keysym.sym, event.repeat, event.keysym.sym);
                newState = true;
                break;
            case SDL_KEYUP:
                //logDebug("SDL_KEYUP {0:c}", event.keysym.sym);
                newState = false;
                break;
            }

            if(event.repeat == 0 || v->mode == TYPE) {
                //logDebug("write raw");
                v->raw = newState;
            }
        }
    }
}

KeyControl* Controls::add(KeyControl* c) {

    if(!initted) {
        /*for (int i = 0; i < NUM_CONTROLS; i++) {
            keyControls[i] = NULL;
        }*/
        keyControls = {};
        initted = true;
    }

    //return keyControls[c->key] = c;
    keyControls.push_back(c);
    return c;
}

bool KeyControl::get() {

    bool ret = false;
    switch(mode) {
    case MOMENTARY:
        ret = raw;
        break;
    case RISING:
        ret = raw && !lastRaw;
        break;
    case FALLING:
        ret = !raw && lastRaw;
        break;
    case TOGGLE:
        if(raw && !lastRaw) lastState ^= true;
        ret = lastState;
        break;
    case TYPE:
        ret = raw;
        raw = false;
        break;
    }

    lastRaw = raw;
    return ret;
}

bool MultiControl::get() {
    if(this->combine == ControlCombine::OR) {
        for(auto& v : this->controls) {
            if(v->get()) return true;
        }
        return false;
    } else if(this->combine == ControlCombine::AND) {
        for(auto& v : this->controls) {
            if(!v->get()) return false;
        }
        return true;
    }

    return false;
}
