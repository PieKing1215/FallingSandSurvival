#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "Game.hpp"

class DebugUI {
public:
	static bool visible;

	static void Draw(Game* game);
};

class DebugCheatsUI {
public:
	static bool visible;
	static std::vector<GPU_Image*> images;

	static void Setup();

	static void Draw(Game* game);
};

class DebugDrawUI {
public:
	static bool visible;
	static int selIndex;
	static std::vector<GPU_Image*> images;

	static Material* selectedMaterial;
	static uint8 brushSize;

	static void Setup();

	static void Draw(Game* game);
};

class MainMenuUI {
public:
	static bool visible;

	static int state;

	static bool setup;

	static GPU_Image* title;

	static bool connectButtonEnabled;

	static ImVec2 pos;

	static std::vector<std::tuple<std::string, WorldMeta>> worlds;

	static long long lastRefresh;

	static void RefreshWorlds(Game* game);

	static void Setup();

	static void Draw(Game* game);

	static void DrawMainMenu(Game* game);

	static void DrawSingleplayer(Game* game);
	static void DrawMultiplayer(Game* game);

	static void DrawCreateWorld(Game* game);

	static void DrawOptions(Game* game);

};

class IngameUI {
public:
	static bool visible;

	static int state;

	static bool setup;

	static void Setup();

	static void Draw(Game* game);

	static void DrawIngame(Game* game);

	static void DrawOptions(Game* game);

};

class CreateWorldUI {
public:
	static bool setup;
	static char worldNameBuf[32];

	static GPU_Image* materialTestWorld;
	static GPU_Image* defaultWorld;

	static bool createWorldButtonEnabled;

	static std::string worldFolderLabel;

	static int selIndex;

	static void Setup();
	static void Reset(Game* game);

	static void Draw(Game* game);

	static void inputChanged(std::string text, Game* game);
};

class OptionsUI {

	static std::map<std::string, FMOD::Studio::Bus*> busMap;

public:
	static int item_current_idx;
	static bool vsync;
	static bool minimizeOnFocus;

	static void Draw(Game* game);
	static void DrawGeneral(Game* game);
	static void DrawVideo(Game* game);
	static void DrawAudio(Game* game);
	static void DrawInput(Game* game);
};