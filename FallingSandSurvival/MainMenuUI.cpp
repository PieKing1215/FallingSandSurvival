#include "UIs.hpp"

#define timegm _mkgmtime

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>
#include "ProfilerConfig.hpp"

bool MainMenuUI::visible = true;
bool MainMenuUI::setup = false;
GPU_Image* MainMenuUI::title = nullptr;
bool MainMenuUI::connectButtonEnabled = false;
ImVec2 MainMenuUI::pos = ImVec2(0, 0);
std::vector<std::tuple<std::string, WorldMeta>> MainMenuUI::worlds = {};
long long MainMenuUI::lastRefresh = 0;

void MainMenuUI::RefreshWorlds(Game* game) {
	EASY_FUNCTION(UI_PROFILER_COLOR);

	worlds = {};

	for(auto& p : experimental::filesystem::directory_iterator(game->getFileInGameDir("worlds/"))) {
		string worldName = p.path().filename().generic_string();

		WorldMeta meta = WorldMeta::loadWorldMeta((char*)game->getWorldDir(worldName).c_str());

		worlds.push_back(std::make_tuple(worldName, meta));
	}
}

void MainMenuUI::Setup() {
	EASY_FUNCTION(UI_PROFILER_COLOR);

	SDL_Surface* logoSfc = Textures::loadTexture("assets/ui/temp_logo.png");
	title = GPU_CopyImageFromSurface(logoSfc);
	GPU_SetImageFilter(title, GPU_FILTER_NEAREST);
	SDL_FreeSurface(logoSfc);

	setup = true;
}

void MainMenuUI::Draw(Game* game) {
	EASY_FUNCTION(UI_PROFILER_COLOR);

	if(!setup) {
		Setup();
	}
	long long now = Time::millis();
	if(now - lastRefresh > 3000) {
		RefreshWorlds(game);
		lastRefresh = now;
	}
	if(!visible) return;

    ImGui::SetNextWindowSize(ImVec2(400, 500));
	ImGui::SetNextWindowPos(ImVec2(game->WIDTH / 2 - 200, game->HEIGHT / 2 - 250), ImGuiCond_FirstUseEver);
	if(!ImGui::Begin("Main Menu", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
		ImGui::End();
		return;
	}
	pos = ImGui::GetWindowPos();

	ImTextureID texId = (ImTextureID)GPU_GetTextureHandle(title);

	ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
	ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
	ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
	ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);

	ImGui::SetCursorPos(ImVec2((ImGui::GetWindowSize().x - title->w / 2) * 0.5f, ImGui::GetCursorPosY() + 10));
	ImGui::Image(texId, ImVec2(title->w/2, title->h/2), uv_min, uv_max, tint_col, border_col);

	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	static char connectBuf[128] = "";
	//200 - connectWidth / 2 - 60 / 2, 60, connectWidth, 20
	ImGui::PushItemWidth(200);
	ImGui::SetCursorPos(ImVec2(200 - 200 / 2 - 60 / 2, 60));
	if(ImGui::InputTextWithHint("", "ip:port", connectBuf, IM_ARRAYSIZE(connectBuf))) {
		regex connectInputRegex("([^:]+):(\\d+)");
		std::string str = std::string(connectBuf);
		connectButtonEnabled = regex_match(str, connectInputRegex);
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleVar();
	ImGui::SameLine();

	if(!connectButtonEnabled) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if(ImGui::Button("Connect")) {
		logInfo("connectButton select");
		if(game->client->connect("172.23.16.150", 1337)) {
			game->networkMode = NetworkMode::CLIENT;
			visible = false;
			game->state = LOADING;
			game->stateAfterLoad = INGAME;
		}
	}

	if(!connectButtonEnabled) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	int mainMenuButtonsWidth = 250;
	int mainMenuButtonsYOffset = 50;
	int mainMenuNewButtonWidth = 150;
	ImGui::SetCursorPos(ImVec2(200 - mainMenuNewButtonWidth / 2, 50 + mainMenuButtonsYOffset));
	ImVec2 selPos = ImGui::GetCursorPos();
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
	if(ImGui::Button(" ", ImVec2(mainMenuNewButtonWidth, 36))) {
		visible = false;
		CreateWorldUI::visible = true;
	}
	ImGui::PopStyleVar();
	ImGui::SetCursorPos(ImVec2(selPos.x + mainMenuNewButtonWidth / 2 - ImGui::CalcTextSize("New World").x / 2, selPos.y));
	ImGui::Text("New World");
	ImGui::PopFont();

	int nMainMenuButtons = 1;
	for(auto& t : worlds) {
		string worldName = std::get<0>(t);

		WorldMeta meta = std::get<1>(t);

		ImGui::PushID(nMainMenuButtons);

		ImGui::SetCursorPos(ImVec2(200 - mainMenuButtonsWidth / 2, 60 + mainMenuButtonsYOffset + (nMainMenuButtons++ * 60)));
		selPos = ImGui::GetCursorScreenPos();
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
		if(ImGui::Button("", ImVec2(mainMenuButtonsWidth, 50))) {
			logInfo("Selected world: {}", worldName.c_str());
			visible = false;
			CreateWorldUI::visible = false;

			game->fadeOutStart = game->now;
			game->fadeOutLength = 250;
			game->fadeOutCallback = [&, game, worldName]() {
				game->state = LOADING;
				game->stateAfterLoad = INGAME;

				EASY_BLOCK("Close world");
				delete game->world;
				game->world = nullptr;
				EASY_END_BLOCK;

				//std::thread loadWorldThread([&] () {
				EASY_BLOCK("Load world");
				World* w = new World();
				w->init((char*)game->getWorldDir(worldName).c_str(), (int)ceil(game->WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(game->HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, game->target, &game->audioEngine, game->networkMode);

				EASY_BLOCK("Queue chunk loading");
				logInfo("Queueing chunk loading...");
				for(int x = -CHUNK_W * 4; x < w->width + CHUNK_W * 4; x += CHUNK_W) {
					for(int y = -CHUNK_H * 3; y < w->height + CHUNK_H * 8; y += CHUNK_H) {
						w->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
					}
				}
				EASY_END_BLOCK;
				EASY_END_BLOCK;

				game->world = w;
				//});

				game->fadeInStart = game->now;
				game->fadeInLength = 250;
				game->fadeInWaitFrames = 4;
			};
		}
		ImGui::PopStyleVar();

		ImVec2 prevPos = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(ImVec2(selPos.x, selPos.y));

		tm* tm_utc = gmtime(&meta.lastOpenedTime);

		// convert to local time
		time_t time_utc = timegm(tm_utc);
		time_t time_local = mktime(tm_utc);
		time_local += time_utc - time_local;
		tm* tm_local = localtime(&time_local);

		char* formattedTime = new char[100];
		strftime(formattedTime, 100, "%#m/%#d/%y %#I:%M%p", tm_local);

		char* filenameAndTimestamp = new char[200];
		snprintf(filenameAndTimestamp, 100, "%s (%s)", worldName.c_str(), formattedTime);

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::SetCursorScreenPos(ImVec2(selPos.x + mainMenuButtonsWidth/2 - ImGui::CalcTextSize(meta.worldName.c_str()).x/2, selPos.y));
		ImGui::Text("%s", meta.worldName.c_str());
		ImGui::PopFont();

		ImGui::SetCursorScreenPos(ImVec2(selPos.x + mainMenuButtonsWidth / 2 - ImGui::CalcTextSize(filenameAndTimestamp).x / 2, selPos.y + 32));
		ImGui::Text("%s", filenameAndTimestamp);

		ImGui::SetCursorScreenPos(prevPos);
		ImGui::PopID();

	}

	ImGui::End();
}
