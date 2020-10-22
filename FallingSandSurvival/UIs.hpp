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
	static GPU_Image** images;

	static Material* selectedMaterial;
	static uint8 brushSize;

	static void Setup();

	static void Draw(Game* game);
};

class MainMenuUI {
public:
	static bool visible;

	static bool setup;

	static GPU_Image* title;

	static bool connectButtonEnabled;

	static ImVec2 pos;

	static std::vector<std::tuple<std::string, WorldMeta>> worlds;

	static long long lastRefresh;

	static void RefreshWorlds(Game* game);

	static void Setup();

	static void Draw(Game* game);

};

class CreateWorldUI {
public:
	static bool visible;
	static bool wasVisible;

	static bool setup;

	static GPU_Image* materialTestWorld;
	static GPU_Image* defaultWorld;

	static bool createWorldButtonEnabled;

	static std::string worldFolderLabel;

	static int selIndex;

	static void Setup();

	static void Draw(Game* game);

	static void inputChanged(std::string text, Game* game);
};