#include "UIs.hpp"

#include "DiscordIntegration.hpp"

#define timegm _mkgmtime

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

int IngameUI::state = 0;

bool IngameUI::visible = false;
bool IngameUI::setup = false;

void IngameUI::Setup() {

}

void IngameUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    if(!visible) return;

    if(state == 0) {
        DrawIngame(game);
    } else if(state == 1) {
        DrawOptions(game);
    }
}

void IngameUI::DrawIngame(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    if(!setup) {
        Setup();
    }

    ImGui::SetNextWindowSize(ImVec2(400, 300));
    ImGui::SetNextWindowPos(ImVec2(game->WIDTH / 2 - 200, game->HEIGHT / 2 - 250), ImGuiCond_FirstUseEver);
    if(!ImGui::Begin("Pause Menu", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        return;
    }

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("Options").x / 2);
    ImGui::Text("Options");
    ImGui::PopFont();

    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);

    int mainMenuButtonsWidth = 250;
    int mainMenuButtonsYOffset = 50;

    ImVec2 selPos;

    ImGui::SetCursorPos(ImVec2(200 - mainMenuButtonsWidth / 2, 25 + mainMenuButtonsYOffset * 1));
    selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("##continue", ImVec2(mainMenuButtonsWidth, 36))) {
        visible = false;
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + mainMenuButtonsWidth / 2 - ImGui::CalcTextSize("Continue").x / 2, selPos.y));
    ImGui::Text("Continue");
    ImGui::PopFont();

    ImGui::SetCursorPos(ImVec2(200 - mainMenuButtonsWidth / 2, 25 + mainMenuButtonsYOffset * 2));
    selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("##options", ImVec2(mainMenuButtonsWidth, 36))) {
        state = 1;
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + mainMenuButtonsWidth / 2 - ImGui::CalcTextSize("Options").x / 2, selPos.y));
    ImGui::Text("Options");
    ImGui::PopFont();


    ImGui::SetCursorPos(ImVec2(200 - mainMenuButtonsWidth / 2, 25 + mainMenuButtonsYOffset * 4));
    selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("##quit", ImVec2(mainMenuButtonsWidth, 36))) {
        visible = false;
        game->quitToMainMenu();
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + mainMenuButtonsWidth / 2 - ImGui::CalcTextSize("Quit to Main Menu").x / 2, selPos.y));
    ImGui::Text("Quit to Main Menu");
    ImGui::PopFont();

    ImGui::End();
}

void IngameUI::DrawOptions(Game* game) {
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.11f, 0.11f, 0.11f, 0.9f));
    ImGui::SetNextWindowSize(ImVec2(400, 400));
    if(!ImGui::Begin("Pause Menu", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        ImGui::End();
        ImGui::PopStyleColor();
        return;
    }

    OptionsUI::Draw(game);

    ImGui::End();
    ImGui::PopStyleColor();
}
