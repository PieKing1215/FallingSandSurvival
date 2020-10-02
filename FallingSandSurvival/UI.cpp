
#include "UI.h"
#include "Settings.h"

#include "Macros.h"
#include "UTime.h"
#include "ProfilerConfig.h"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

void UI::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    background->draw(t, bounds);

    UINode::draw(t, transformX, transformY);
}

bool UI::onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return false;
    if(ev.type == SDL_MOUSEMOTION && ev.motion.state & SDL_BUTTON_LMASK) {
        bounds->x += ev.motion.xrel;
        bounds->y += ev.motion.yrel;

        return true;
    } else if(ev.type == SDL_MOUSEBUTTONDOWN) {
        return true;
    }
    return false;
}

void SolidBackground::draw(GPU_Target* t, SDL_Rect* bounds) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    GPU_RectangleFilled(t, bounds->x, bounds->y, bounds->x + bounds->w, bounds->y + bounds->h, {(color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, (color >> 24) & 0xff});
}

void UINode::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    for(auto& c : children) {
        c->parent = this;
        c->draw(t, bounds->x + transformX, bounds->y + transformY);
    }

    if(Settings::draw_uinode_bounds || drawBorder) {
        GPU_Rectangle(t, bounds->x + transformX, bounds->y + transformY, bounds->x + transformX + bounds->w, bounds->y + transformY + bounds->h, {0xcc, 0xcc, 0xcc, 0xff});
    }
}

bool UINode::checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return false;

    bool ok = false;

    switch(ev.type) {
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        ok = ev.button.x >= bounds->x + transformX && ev.button.x <= bounds->x + transformX + bounds->w &&
            ev.button.y >= bounds->y + transformY && ev.button.y <= bounds->y + transformY + bounds->h;
        break;
    case SDL_MOUSEMOTION:
        ok = (ev.motion.x >= bounds->x + transformX && ev.button.x <= bounds->x + transformX + bounds->w &&
            ev.button.y >= bounds->y + transformY && ev.button.y <= bounds->y + transformY + bounds->h) ||
            (ev.motion.x - ev.motion.xrel >= bounds->x + transformX && ev.button.x - ev.motion.xrel <= bounds->x + transformX + bounds->w &&
                ev.button.y - ev.motion.yrel >= bounds->y + transformY && ev.button.y - ev.motion.yrel <= bounds->y + transformY + bounds->h);
        break;
    case SDL_KEYDOWN:
        ok = true;
        break;
    case SDL_TEXTINPUT:
        ok = true;
        break;
    case SDL_TEXTEDITING:
        ok = true;
        break;
    }

    if(ok) {
        bool stopPropagation = false;
        for(auto& c : children) {
            stopPropagation = stopPropagation || c->checkEvent(ev, t, world, bounds->x + transformX, bounds->y + transformY);
        }

        if(!stopPropagation) {
            return onEvent(ev, t, world, bounds->x + transformX, bounds->y + transformY);
        } else {
            return true;
        }
    }

    return false;
}

void UILabel::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    //drawText(r, text.c_str(), font, bounds->x + transformX, bounds->y + transformY, (textColor >> 16) & 0xff, (textColor >> 8) & 0xff, (textColor >> 0) & 0xff, 0, 0, 0, align);

    if(texture == NULL) {
        texture = GPU_CopyImageFromSurface(surface);
        GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    }

    GPU_Blit(texture, NULL, t, bounds->x + transformX + 1 - align * surface->w / 2 + surface->w * 0.5, bounds->y + transformY + 1 + surface->h * 0.5);

    UINode::draw(t, transformX, transformY);
}

bool UIButton::checkEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return false;
    if(disabled) return false;

    bool ok = false;

    if(ev.type == SDL_MOUSEMOTION) {
        return onEvent(ev, t, world, transformX, transformY);
    }

    return UINode::checkEvent(ev, t, world, transformX, transformY);
}

bool UIButton::onEvent(SDL_Event ev, GPU_Target* t, World * world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(ev.type == SDL_MOUSEBUTTONUP) {
        selectCallback();
        return true;
    } else if(ev.type == SDL_MOUSEMOTION) {

        hovered = (ev.motion.x >= bounds->x + transformX && ev.button.x <= bounds->x + transformX + bounds->w &&
            ev.button.y >= bounds->y + transformY && ev.button.y <= bounds->y + transformY + bounds->h) ||
            (ev.motion.x - ev.motion.xrel >= bounds->x + transformX && ev.button.x - ev.motion.xrel <= bounds->x + transformX + bounds->w &&
                ev.button.y - ev.motion.yrel >= bounds->y + transformY && ev.button.y - ev.motion.yrel <= bounds->y + transformY + bounds->h);
        //hovered = true;
        //printf("hovering = %s\n", hovering ? "true" : "false");

        //hoverCallback();
        return hovered;
    }
    return false;
}

void UIButton::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    if(Settings::draw_uinode_bounds || drawBorder) {
        EASY_BLOCK("draw border", UI_PROFILER_COLOR);
        GPU_Rect tb {(float)(bounds->x + transformX), (float)(bounds->y + transformY), (float)(bounds->w), (float)(bounds->h)};
        if(disabled) {
            GPU_RectangleFilled2(t, tb, {0x48, 0x48, 0x48, 0x80});
            GPU_Rectangle2(t, tb, {0x77, 0x77, 0x77, 0xff});
        } else {
            GPU_RectangleFilled2(t, tb, hovered ? SDL_Color{0xAA, 0xAA, 0xAA, 0x80} : SDL_Color{0x66, 0x66, 0x66, 0x80});
            GPU_Rectangle2(t, tb, hovered ? SDL_Color{0xDD, 0xDD, 0xDD, 0xFF} : SDL_Color{0x88, 0x88, 0x88, 0xFF});
        }
        EASY_END_BLOCK;
    }

    if(textureDisabled == NULL) {
        textureDisabled = GPU_CopyImageFromSurface(surfaceDisabled);
        GPU_SetImageFilter(textureDisabled, GPU_FILTER_NEAREST);
    }

    if(texture == NULL) {
        texture = GPU_CopyImageFromSurface(surface);
        GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    }

    if(surface != NULL) {
        EASY_BLOCK("draw text", UI_PROFILER_COLOR);
        GPU_Image* tex = disabled ? textureDisabled : texture;

        GPU_Blit(tex, NULL, t, bounds->x + transformX + 1 - align * surface->w / 2 + bounds->w / 2 + surface->w * 0.5, bounds->y + transformY + 1 + surface->h * 0.5);
        EASY_END_BLOCK;
    }

    for(auto& c : children) {
        c->parent = this;
        c->draw(t, bounds->x + transformX, bounds->y + transformY);
    }
}

void UICheckbox::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    GPU_Rect tb = {(float)(bounds->x + transformX), (float)(bounds->y + transformY), (float)(bounds->w), (float)(bounds->h)};
    SDL_Color col;
    if(checked) {
        col = {0x00, 0xff, 0x00, 0xff};
    } else {
        col = {0xff, 0x00, 0x00, 0xff};
    }
    GPU_RectangleFilled2(t, tb, col);

    UINode::draw(t, transformX, transformY);
}

bool UICheckbox::onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(ev.type == SDL_MOUSEBUTTONDOWN) {
        checked = !checked;
        callback(checked);
        return true;
    } else if(ev.type == SDL_MOUSEMOTION) {
        return true;
    }
    return false;
}


void UITextArea::draw(GPU_Target * t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return;

    GPU_Rect tb = {(float)(bounds->x + transformX), (float)(bounds->y + transformY), (float)(bounds->w), (float)(bounds->h)};
    GPU_RectangleFilled2(t, tb, focused ? SDL_Color {0x80, 0x80, 0x80, 0xA0} : SDL_Color {0x60, 0x60, 0x60, 0xA0});
    GPU_Rectangle2(t, tb, {0xff, 0xff, 0xff, 0xff});

    int cursorX = 0;
    if(cursorIndex > text.size()) cursorIndex = (int)text.size();
    if(cursorIndex < 0) cursorIndex = 0;
    if(text.size() > 0) {
        if(maxLength != -1) {
            if(text.size() > maxLength) {
                text = text.substr(0, maxLength);
                if(cursorIndex > text.size()) cursorIndex = (int)text.size();
                if(cursorIndex < 0) cursorIndex = 0;
            }
        }

        if(dirty) {
            textParams = Drawing::drawTextParams(t, text.c_str(), font, (int)tb.x + 1, (int)tb.y + 2, 0xff, 0xff, 0xff, ALIGN_LEFT);
            dirty = false;
        }

        Drawing::drawText(t, textParams, (int)tb.x + 1, (int)tb.y + 1, ALIGN_LEFT);

        std::string precursor = text.substr(0, cursorIndex);
        int w, h;
        if(TTF_SizeText(font, precursor.c_str(), &w, &h)) {
            logError("TTF_SizeText failed: {}", TTF_GetError());
        } else {
            cursorX += w;
        }
    } else if(!focused) {

        if(dirty) {
            textParams = Drawing::drawTextParams(t, placeholder.c_str(), font, (int)tb.x + 1, (int)tb.y + 2, 0xbb, 0xbb, 0xbb, false, ALIGN_LEFT);
            dirty = false;
        }

        Drawing::drawText(t, textParams, (int)tb.x + 1, (int)tb.y + 2, false, ALIGN_LEFT);
    }

    if(focused && (Time::millis() - lastCursorTimer) % 1000 < 500) {
        GPU_Line(t, tb.x + 3 + cursorX - 1, tb.y + 2 + 2, tb.x + 2 + cursorX - 1, tb.y + 2 + TTF_FontHeight(font), {0xff, 0xff, 0xff, 0xff});
        GPU_Line(t, tb.x + 3 + cursorX, tb.y + 2 + 2, tb.x + 2 + cursorX, tb.y + 2 + TTF_FontHeight(font), {0xaa, 0xaa, 0xaa, 0x80});
    }

    UINode::draw(t, transformX, transformY);
}

bool UITextArea::checkEvent(SDL_Event ev, GPU_Target * t, World * world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(!visible) return false;

    bool ok = false;

    if(ev.type == SDL_MOUSEBUTTONDOWN) {
        return onEvent(ev, t, world, transformX, transformY);
    }

    return UINode::checkEvent(ev, t, world, transformX, transformY);
}

bool UITextArea::onEvent(SDL_Event ev, GPU_Target * t, World * world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(ev.type == SDL_MOUSEBUTTONDOWN) {

        bool hovered = (ev.motion.x >= bounds->x + transformX && ev.button.x <= bounds->x + transformX + bounds->w &&
            ev.button.y >= bounds->y + transformY && ev.button.y <= bounds->y + transformY + bounds->h) ||
            (ev.motion.x - ev.motion.xrel >= bounds->x + transformX && ev.button.x - ev.motion.xrel <= bounds->x + transformX + bounds->w &&
                ev.button.y - ev.motion.yrel >= bounds->y + transformY && ev.button.y - ev.motion.yrel <= bounds->y + transformY + bounds->h);

        bool wasFocused = focused;
        focused = hovered;

        if(focused) {
            if(text.size() > 0) {
                bool found = false;
                int accW = 0;
                for(int i = 0; i < text.size(); i++) {
                    int w, h;
                    if(TTF_SizeText(font, std::string(1, text.at(i)).c_str(), &w, &h)) {
                        logError("TTF_SizeText failed: {}", TTF_GetError());
                    } else {
                        accW += w;
                        if(ev.button.x + 3 < accW + (bounds->x + transformX) + 2) {
                            cursorIndex = i;
                            found = true;
                            break;
                        }
                    }
                }
                if(!found) {
                    cursorIndex = (int)text.size();
                }
            } else {
                cursorIndex = 0;
            }
            lastCursorTimer = Time::millis();
            callback(text);
        }

        if(focused && !wasFocused) {
            SDL_StartTextInput();
        } else if(!focused && wasFocused) {
            SDL_StopTextInput();
        }

        return focused;
    } else if(ev.type == SDL_MOUSEMOTION) {
        return true;
    } else if(ev.type == SDL_KEYDOWN && focused) {
        SDL_Keycode ch = ev.key.keysym.sym;
        if(ch == SDLK_BACKSPACE && text.size() > 0 && cursorIndex > 0) {
            text = text.erase(cursorIndex - 1, 1);
            if(cursorIndex > 0) cursorIndex--;
            lastCursorTimer = Time::millis();
            dirty = true;
            callback(text);
        } else if(ch == SDLK_DELETE && text.size() > 0 && cursorIndex < text.size()) {
            text = text.erase(cursorIndex, 1);
            lastCursorTimer = Time::millis();
            dirty = true;
            callback(text);
        } else if(ch == SDLK_HOME) {
            cursorIndex = 0;
            lastCursorTimer = Time::millis();
        } else if(ch == SDLK_END) {
            cursorIndex = (int)text.size();
            lastCursorTimer = Time::millis();
        } else if(ch == SDLK_LEFT) {
            if((ev.key.keysym.mod & SDL_Keymod::KMOD_LCTRL) || (ev.key.keysym.mod & SDL_Keymod::KMOD_RCTRL)) {
                bool foundAlpha = false;
                for(int i = cursorIndex; i > 0; i--) {
                    char curCh = text.at(i - 1);
                    if(!isalnum((unsigned char)curCh)) {
                        if(foundAlpha) {
                            cursorIndex++;
                            break;
                        }
                    } else {
                        foundAlpha = true;
                    }
                    cursorIndex = i - 1;
                }
                if(cursorIndex > 0) cursorIndex--;
            } else {
                if(cursorIndex > 0) cursorIndex--;
            }
            lastCursorTimer = Time::millis();
        } else if(ch == SDLK_RIGHT) {
            if((ev.key.keysym.mod & SDL_Keymod::KMOD_LCTRL) || (ev.key.keysym.mod & SDL_Keymod::KMOD_RCTRL)) {
                bool foundNonAlpha = false;
                for(int i = cursorIndex; i < text.size(); i++) {
                    char curCh = text.at(i);
                    if(!isalnum((unsigned char)curCh)) {
                        foundNonAlpha = true;
                    } else {
                        if(foundNonAlpha) {

                            break;
                        }
                    }
                    cursorIndex = i;
                }
                if(cursorIndex < text.size()) cursorIndex++;
            } else {
                if(cursorIndex < text.size()) cursorIndex++;
            }
            lastCursorTimer = Time::millis();
        }
        return true;
    } else if(ev.type == SDL_TEXTINPUT && focused) {
        text = text.insert(cursorIndex, ev.text.text, strlen(ev.text.text));
        dirty = true;
        //text += ev.text.text;
        if(cursorIndex < text.size()) cursorIndex++;
        callback(text);
        return true;
    } else if(ev.type == SDL_TEXTEDITING && focused) {
        auto composition = ev.edit.text;
        auto cursor = ev.edit.start;
        auto selection_len = ev.edit.length;
        logDebug("SDL_TEXTEDITING");
        return true;
    }
    return false;
}

void ChiselNode::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    int scale = 4;

    GPU_Rect dst = {(float)(bounds->x + transformX), (float)(bounds->y + transformY), (float)(bounds->w), (float)(bounds->h)};
    GPU_BlitRect(texture, NULL, t, &dst);

    for(int x = 1; x < surface->w; x++) {
        GPU_Line(t, dst.x + x * scale, dst.y, dst.x + x * scale, dst.y + dst.h, {0, 0, 0, 0x80});
    }

    for(int y = 1; y < surface->h; y++) {
        GPU_Line(t, dst.x, dst.y + y * scale, dst.x + dst.w, dst.y + y * scale, {0, 0, 0, 0x80});
    }

    UINode::draw(t, transformX, transformY);
}

bool ChiselNode::onEvent(SDL_Event ev, GPU_Target* t, World* world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if((ev.type == SDL_MOUSEMOTION && ev.motion.state & SDL_BUTTON_LMASK) || (ev.type == SDL_MOUSEBUTTONDOWN && ev.button.button & SDL_BUTTON_LMASK)) {
        int lx = ev.motion.x - transformX;
        int ly = ev.motion.y - transformY;

        int px = lx / 4;
        int py = ly / 4;

        PIXEL(surface, px, py) = 0x00000000;

        GPU_FreeImage(texture);
        texture = GPU_CopyImageFromSurface(surface);
        GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
        return true;
    } else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {

        SDL_FreeSurface(rb->surface);
        GPU_FreeImage(rb->texture);

        rb->surface = surface;
        rb->texture = texture;
        world->updateRigidBodyHitbox(rb);
        parent->visible = false;
        return true;
    }
    return false;
}

bool ChiselNode::checkEvent(SDL_Event ev, GPU_Target* t, World * world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE) {
        UINode::checkEvent(ev, t, world, transformX, transformY);
        return true;
    }
    return UINode::checkEvent(ev, t, world, transformX, transformY);
}

void MaterialNode::draw(GPU_Target* t, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(texture == NULL) {
        texture = GPU_CopyImageFromSurface(surface);
        GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    }

    EASY_BLOCK("GPU_BlitScale", GPU_PROFILER_COLOR);
    GPU_BlitScale(texture, NULL, t, bounds->x + transformX + bounds->w/2, bounds->y + transformY + bounds->h / 2, bounds->w / texture->w, bounds->h / texture->h);
    EASY_END_BLOCK;

    UINode::draw(t, transformX, transformY);
}

bool MaterialNode::onEvent(SDL_Event ev, GPU_Target* t, World * world, int transformX, int transformY) {
    EASY_FUNCTION(UI_PROFILER_COLOR);
    if(ev.type == SDL_MOUSEBUTTONDOWN) {
        selectCallback(mat);
        return true;
    } else if(ev.type == SDL_MOUSEMOTION) {
        hoverCallback(mat);
        return true;
    }
    return false;
}

