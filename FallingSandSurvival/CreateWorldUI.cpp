#include "UIs.hpp"

#include "DefaultGenerator.cpp"
#include "MaterialTestGenerator.cpp"
#define timegm _mkgmtime

#include "DiscordIntegration.hpp"

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

char CreateWorldUI::worldNameBuf[32] = "";
bool CreateWorldUI::setup   = false;
GPU_Image* CreateWorldUI::materialTestWorld = nullptr;
GPU_Image* CreateWorldUI::defaultWorld = nullptr;
bool CreateWorldUI::createWorldButtonEnabled = false;
std::string CreateWorldUI::worldFolderLabel = "";
int CreateWorldUI::selIndex = 0;

void CreateWorldUI::Setup() {
    EASY_FUNCTION(UI_PROFILER_COLOR);

	SDL_Surface* logoMT = Textures::loadTexture("assets/ui/prev_materialtest.png");
	materialTestWorld = GPU_CopyImageFromSurface(logoMT);
	GPU_SetImageFilter(materialTestWorld, GPU_FILTER_NEAREST);
	SDL_FreeSurface(logoMT);

	SDL_Surface* logoDef = Textures::loadTexture("assets/ui/prev_default.png");
	defaultWorld = GPU_CopyImageFromSurface(logoDef);
	GPU_SetImageFilter(defaultWorld, GPU_FILTER_NEAREST);
	SDL_FreeSurface(logoDef);

	setup = true;
}

void CreateWorldUI::Draw(Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

	if(!setup) Setup();

    int createWorldWidth = 350;

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - ImGui::CalcTextSize("Create World").x / 2);
    ImGui::Text("Create World");
    ImGui::PopFont();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushItemWidth(createWorldWidth);
    ImGui::SetCursorPos(ImVec2(200 - createWorldWidth / 2, 70));
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    if(ImGui::InputTextWithHint("", "", worldNameBuf, IM_ARRAYSIZE(worldNameBuf))) {
        std::string text = std::string(worldNameBuf);
        inputChanged(text, game);
    }
    ImGui::PopFont();
    ImGui::PopItemWidth();
    ImGui::PopStyleVar();

    ImGui::SetCursorPos(ImVec2(200 - createWorldWidth / 2, 70 - 16));
    ImGui::Text("World Name");

    ImGui::SetCursorPos(ImVec2(200 - createWorldWidth / 2, 70 + 40 + 2));
    ImGui::Text("%s", worldFolderLabel.c_str());
        
    // generator selection

    ImGui::PushID(0);

    ImGui::SetCursorPos(ImVec2(400 / 2 - 100 - 24 - 4, 170));
    ImVec2 selPos = ImGui::GetCursorPos();
    if(ImGui::Selectable("", selIndex == 0, 0, ImVec2(111 - 4, 111))) {
        selIndex = 0;
    }

    if(ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Material Test World");
        ImGui::EndTooltip();
    }

    ImVec2 prevPos = ImGui::GetCursorPos();
    ImGuiStyle& style = ImGui::GetStyle();
    ImGui::SetCursorPos(ImVec2(selPos.x - 1 + 4, selPos.y + 4));

    // imgui_impl_opengl3.cpp implements ImTextureID as GLuint 
    ImTextureID texId = (ImTextureID)GPU_GetTextureHandle(materialTestWorld);

    ImVec2 pos = ImGui::GetCursorPos();
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

    ImGui::Image(texId, ImVec2(100, 100), uv_min, uv_max, tint_col, border_col);

    ImGui::SetCursorPos(prevPos);

    ImGui::PopID();


    ImGui::PushID(1);

    ImGui::SetCursorPos(ImVec2(400 / 2 + 24 - 4, 170));
    selPos = ImGui::GetCursorPos();
    if(ImGui::Selectable("", selIndex == 1, 0, ImVec2(111 - 4, 111))) {
        selIndex = 1;
    }

    if(ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Default World (WIP)");
        ImGui::EndTooltip();
    }

    prevPos = ImGui::GetCursorPos();
    style = ImGui::GetStyle();
    ImGui::SetCursorPos(ImVec2(selPos.x - 1 + 4, selPos.y + 4));

    // imgui_impl_opengl3.cpp implements ImTextureID as GLuint 
    texId = (ImTextureID)GPU_GetTextureHandle(defaultWorld);

    pos = ImGui::GetCursorPos();
    uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
    border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);

    ImGui::Image(texId, ImVec2(100, 100), uv_min, uv_max, tint_col, border_col);

    ImGui::SetCursorPos(prevPos);

    ImGui::PopID();

    ImGui::SetCursorPos(ImVec2(20, 360 - 52));
    selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("##back", ImVec2(150, 36))) {
        MainMenuUI::state = 2;
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + 150 / 2 - ImGui::CalcTextSize("Back").x / 2, selPos.y));
    ImGui::Text("Back");
    ImGui::PopFont();

    if(!createWorldButtonEnabled) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    }

    ImGui::SetCursorPos(ImVec2(400 - 170, 360 - 52));
    selPos = ImGui::GetCursorPos();
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    if(ImGui::Button("##create", ImVec2(150, 36))) {
        std::string pref = "Saved in: ";

        std::string worldName = worldFolderLabel.substr(pref.length());
        char* wn = (char*)worldName.c_str();

        std::string worldTitle = std::string(worldNameBuf);
        regex trimWhitespaceRegex("^ *(.+?) *$");
        worldTitle = regex_replace(worldTitle, trimWhitespaceRegex, "$1");

        logInfo("Creating world named \"{}\" at \"{}\"", worldTitle, game->gameDir.getWorldPath(wn));
        MainMenuUI::visible = false;
        game->state = LOADING;
        game->stateAfterLoad = INGAME;

        EASY_BLOCK("Close world");
        delete game->world;
        game->world = nullptr;
        EASY_END_BLOCK;

        WorldGenerator* generator;

        if(selIndex == 0) {
            generator = new MaterialTestGenerator();
        } else if(selIndex == 1) {
            generator = new DefaultGenerator();
        } else {
            // create world UI is in invalid state
            generator = new MaterialTestGenerator();
        }

        std::string wpStr = game->gameDir.getWorldPath(wn);

        EASY_BLOCK("Load world");
        game->world = new World();
        game->world->init(wpStr, (int)ceil(Game::MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(Game::MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, game->target, &game->audioEngine, game->networkMode, generator);
        game->world->metadata.worldName = std::string(worldNameBuf);
        game->world->metadata.lastOpenedTime = Time::millis() / 1000;
        game->world->metadata.lastOpenedVersion = std::string(VERSION);
        game->world->metadata.save(wpStr);

        EASY_BLOCK("Queue chunk loading");
        logInfo("Queueing chunk loading...");
        for(int x = -CHUNK_W * 4; x < game->world->width + CHUNK_W * 4; x += CHUNK_W) {
            for(int y = -CHUNK_H * 3; y < game->world->height + CHUNK_H * 8; y += CHUNK_H) {
                game->world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
            }
        }
        EASY_END_BLOCK;
        EASY_END_BLOCK;

        #if BUILD_WITH_DISCORD
        DiscordIntegration::setStart(Time::millis());
        DiscordIntegration::setActivityState("Playing Singleplayer");
        DiscordIntegration::flushActivity();
        #endif
    }
    ImGui::PopStyleVar();
    ImGui::SetCursorPos(ImVec2(selPos.x + 150 / 2 - ImGui::CalcTextSize("Create").x / 2, selPos.y));
    ImGui::Text("Create");
    ImGui::PopFont();

    if(!createWorldButtonEnabled) {
        ImGui::PopItemFlag();
        ImGui::PopStyleVar();
    }
}

void CreateWorldUI::inputChanged(std::string text, Game* game) {
    EASY_FUNCTION(UI_PROFILER_COLOR);

    regex trimWhitespaceRegex("^ *(.+?) *$");
    text = regex_replace(text, trimWhitespaceRegex, "$1");
    if(text.length() == 0 || text == " ") {
        worldFolderLabel = "Saved in: ";
        createWorldButtonEnabled = false;
        return;
    }

    regex worldNameInputRegex("^[\\x20-\\x7E]+$");
    createWorldButtonEnabled = regex_match(text, worldNameInputRegex);

    regex worldFolderRegex("[\\/\\\\:*?\"<>|.]");

    std::string worldFolderName = regex_replace(text, worldFolderRegex, "_");
    std::string folder = game->gameDir.getWorldPath(worldFolderName);
    struct stat buffer;
    bool exists = (stat(folder.c_str(), &buffer) == 0);

    std::string newWorldFolderName = worldFolderName;
    int i = 2;
    while(exists) {
        newWorldFolderName = worldFolderName + " (" + std::to_string(i) + ")";
        folder = game->gameDir.getWorldPath(newWorldFolderName);

        exists = (stat(folder.c_str(), &buffer) == 0);

        i++;
    }


    worldFolderLabel = "Saved in: " + newWorldFolderName;
}

void CreateWorldUI::Reset(Game* game) {
    #ifdef _WIN32
    strcpy_s(worldNameBuf, "New World");
    #else
    strcpy(worldNameBuf, "New World");
    #endif
    inputChanged(std::string(worldNameBuf), game);
}
