#include "UIs.hpp"

#include "DiscordIntegration.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

#include <map>

std::map<std::string, FMOD::Studio::Bus*> OptionsUI::busMap = {};
int OptionsUI::item_current_idx = 0;
bool OptionsUI::vsync = false;
bool OptionsUI::minimizeOnFocus = false;

void OptionsUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    int createWorldWidth = 350;

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("Options").x / 2);
    ImGui::Text("Options");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    static int prevTab = 0;
    int tab = 0;
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if(ImGui::BeginTabBar("OptionsTabs", tab_bar_flags)) {

        if(ImGui::BeginTabItem("General")) {
            tab = 0;
            ImGui::BeginChild("OptionsTabsCh", ImVec2(0, 250), false);

            DrawGeneral(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Video")) {
            tab = 1;
            ImGui::BeginChild("OptionsTabsCh", ImVec2(0, 250), false);

            DrawVideo(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Audio")) {
            tab = 2;
            ImGui::BeginChild("OptionsTabsCh", ImVec2(0, 250), false);

            DrawAudio(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }
        if(ImGui::BeginTabItem("Input")) {
            tab = 3;
            ImGui::BeginChild("OptionsTabsCh", ImVec2(0, 250), false);

            DrawInput(game);

            ImGui::EndChild();
            ImGui::EndTabItem();
        }


        ImGui::EndTabBar();
    }

    if(tab != prevTab) {
        game->audioEngine.PlayEvent("event:/GUI/GUI_Tab");
        prevTab = tab;
    }

    ImGui::Separator();

    ImGui::SetCursorPos(ImVec2(20, ImGui::GetCursorPosY() + 5));
    ImVec2 selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("  ", ImVec2(150, 36))) {
        MainMenuUI::state = 0;
        IngameUI::state = 0;
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + 150 / 2 - ImGui::CalcTextSize("Back").x / 2, selPos.y));
    ImGui::Text("Back");
    ImGui::PopFont();

}

void OptionsUI::DrawGeneral(Game* game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Gameplay");
    ImGui::Indent(4);

    ImGui::Checkbox("Material Tooltips", &Settings::draw_material_info);

    ImGui::Unindent(4);

    #if BUILD_WITH_DISCORD
    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Discord");
    ImGui::Indent(4);

    if(DiscordIntegration::discordAPI) {
        ImGui::TextColored(ImVec4(0.7, 1.0, 0.7, 1.0), "%s", "Discord Integration is running");
        if(ImGui::Button("Stop Discord Integration")) {
            DiscordIntegration::shutdown();
        }

        if(ImGui::Checkbox("Show Details", &DiscordIntegration::showDetails)) {
            DiscordIntegration::flushActivity();
        }

        if(ImGui::Checkbox("Show Play Time", &DiscordIntegration::showPlaytime)) {
            DiscordIntegration::flushActivity();
        }

    } else {
        ImGui::TextColored(ImVec4(1.0, 0.7, 0.7, 1.0), "%s", "Discord Integration is not running");
        if(ImGui::Button("Start Discord Integration")) {
            if(DiscordIntegration::init()) {
                DiscordIntegration::flushActivity();
            }
        }
    }

    ImGui::Unindent(4);
    #endif
}

void OptionsUI::DrawVideo(Game* game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Window");
    ImGui::Indent(4);

    const char* items[] = {"Windowed", "Fullscreen Borderless", "Fullscreen"};
    const char* combo_label = items[item_current_idx];  // Label to preview before opening the combo (technically it could be anything)
    ImGui::SetNextItemWidth(190);
    if(ImGui::BeginCombo("Display Mode", combo_label, 0)) {
        for(int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if(ImGui::Selectable(items[n], is_selected)) {

                switch(n) {
                case 0:
                    game->setDisplayMode(DisplayMode::WINDOWED);
                    break;
                case 1:
                    game->setDisplayMode(DisplayMode::BORDERLESS);
                    break;
                case 2:
                    game->setDisplayMode(DisplayMode::FULLSCREEN);
                    break;
                }

                item_current_idx = n;
            }
                

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if(is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    if(ImGui::Checkbox("VSync", &vsync)) {
        game->setVSync(vsync);
    }

    if(ImGui::Checkbox("Minimize on lost focus", &minimizeOnFocus)) {
        game->setMinimizeOnLostFocus(minimizeOnFocus);
    }


    ImGui::Unindent(4);
    ImGui::Separator();

    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Rendering");
    ImGui::Indent(4);

    if(ImGui::Checkbox("HD Objects", &Settings::hd_objects)) {
        GPU_FreeTarget(game->textureObjects->target);
        GPU_FreeImage(game->textureObjects);
        GPU_FreeTarget(game->textureObjectsBack->target);
        GPU_FreeImage(game->textureObjectsBack);
        GPU_FreeTarget(game->textureEntities->target);
        GPU_FreeImage(game->textureEntities);

        game->textureObjects = GPU_CreateImage(
            game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
            GPU_FormatEnum::GPU_FORMAT_RGBA
        );
        GPU_SetImageFilter(game->textureObjects, GPU_FILTER_NEAREST);

        game->textureObjectsBack = GPU_CreateImage(
            game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
            GPU_FormatEnum::GPU_FORMAT_RGBA
        );
        GPU_SetImageFilter(game->textureObjectsBack, GPU_FILTER_NEAREST);

        GPU_LoadTarget(game->textureObjects);
        GPU_LoadTarget(game->textureObjectsBack);

        game->textureEntities = GPU_CreateImage(
            game->world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), game->world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
            GPU_FormatEnum::GPU_FORMAT_RGBA
        );
        GPU_SetImageFilter(game->textureEntities, GPU_FILTER_NEAREST);

        GPU_LoadTarget(game->textureEntities);
    }

    ImGui::SetNextItemWidth(100);
    ImGui::SliderFloat("Lighting Quality", &Settings::lightingQuality, 0.0, 1.0, "", 0);
    ImGui::Checkbox("Simple Lighting", &Settings::simpleLighting);
    ImGui::Checkbox("Dither Lighting", &Settings::lightingDithering);

    ImGui::Unindent(4);
}

void OptionsUI::DrawAudio(Game* game) {
    ImGui::TextColored(ImVec4(1.0, 1.0, 0.8, 1.0), "%s", "Volume");
    ImGui::Indent(4);

    if(busMap.size() == 0) {
        FMOD::Studio::Bus* busses[20];
        int busCt = 0;
        game->audioEngine.GetBank("assets/audio/fmod/Build/Desktop/Master.bank")->getBusList(busses, 20, &busCt);

        busMap = {};

        for(int i = 0; i < busCt; i++) {
            FMOD::Studio::Bus* b = busses[i];
            char path[100];
            int ctPath = 0;
            b->getPath(path, 100, &ctPath);

            busMap[std::string(path)] = b;
        }
    }

    std::vector<std::vector<std::string>> disp = {
        {"bus:/Master", "Master"},
        {"bus:/Master/Underwater/Music", "Music"},
        {"bus:/Master/GUI", "GUI"},
        {"bus:/Master/Underwater/Player", "Player"},
        {"bus:/Master/Underwater/World", "World"}
    };

    for(auto& v : disp) {
        float volume = 0;
        busMap[v[0]]->getVolume(&volume);
        volume *= 100;
        if(ImGui::SliderFloat(v[1].c_str(), &volume, 0.0f, 100.0f, "%0.0f%%")) {
            volume = max(0.0f, min(volume, 100.0f));
            busMap[v[0]]->setVolume(volume / 100.0f);
        }
    }

    ImGui::Unindent(4);
}

void OptionsUI::DrawInput(Game* game) {

}
