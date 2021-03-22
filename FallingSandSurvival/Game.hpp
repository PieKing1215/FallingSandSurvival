#pragma once

//#define b2_maxTranslation 10.0f
//#define b2_maxTranslationSquared (b2_maxTranslation * b2_maxTranslation)

#include "Macros.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL_gpu.h>
#include <iostream>
#include <algorithm> 
#include <unordered_map>

#include "Networking.hpp"

#include "lib/sparsehash/sparse_hash_map.h"
#ifndef INC_World
#include "world.hpp"
#endif
#include "UTime.hpp"
#include "Textures.hpp"
#include "Background.hpp"
#include <box2d/b2_distance_joint.h>
#include "Settings.hpp"
#include "Controls.hpp"
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <io.h>
#else
#include <sys/io.h>
#endif
#include <fcntl.h>
#include <codecvt>
#include "Drawing.hpp"
#include "Shaders.hpp"

#include "b2DebugDraw_impl.hpp"

#if BUILD_WITH_STEAM
#include "steam_api.h"
#endif

#if BUILD_WITH_DISCORD
#include "discord.h"
#endif

#include "ProfilerConfig.hpp"

#include "GameDir.hpp"

#include "CLArgs.hpp"

enum GameState {
    MAIN_MENU,
    LOADING,
    INGAME
};

enum DisplayMode {
    WINDOWED,
    BORDERLESS,
    FULLSCREEN
};

enum WindowProgressState {
    NONE,
    INDETERMINATE,
    NORMAL,
    PAUSED,
    ERROR
};

enum WindowFlashAction {
    START,
    START_COUNT,
    START_UNTIL_FG,
    STOP
};

class Game {
public:

    static const int MAX_WIDTH = 1920;
    static const int MAX_HEIGHT = 1080;

    CLArgs* clArgs;

    GameState state = LOADING;
    GameState stateAfterLoad = MAIN_MENU;
    int networkMode = -1;
    Client* client = nullptr;
    Server* server = nullptr;

    bool steamAPI = false;
    #if BUILD_WITH_STEAM
    void SteamHookMessages();
    #endif

    CAudioEngine audioEngine;

    int WIDTH = 1200;
    int HEIGHT = 800;

    void handleWindowSizeChange(int newWidth, int newHeight);

    int scale = 4;

    int ofsX = 0;
    int ofsY = 0;

    float plPosX = 0;
    float plPosY = 0;

    float camX = 0;
    float camY = 0;

    float desCamX = 0;
    float desCamY = 0;

    float freeCamX = 0;
    float freeCamY = 0;

    #define frameTimeNum 100
    uint16_t* frameTime = new uint16_t[frameTimeNum];

    TTF_Font* font64 = nullptr;
    TTF_Font* font16 = nullptr;
    TTF_Font* font14 = nullptr;

    SDL_Window* window = nullptr;
    #ifdef _WIN32
    HWND hwnd = NULL;
    ITaskbarList3* win_taskbar = NULL;
    #endif
    void setWindowProgress(WindowProgressState state, float value);
    void setWindowFlash(WindowFlashAction action, int count, int period);

    GPU_Target* realTarget = nullptr;
    GPU_Target* target = nullptr;

    void setDisplayMode(DisplayMode mode);
    void setVSync(bool vsync);
    void setMinimizeOnLostFocus(bool minimize);

    GPU_Image* backgroundImage = nullptr;

    GPU_Image* loadingTexture = nullptr;
    vector< unsigned char > pixelsLoading;
    unsigned char* pixelsLoading_ar = nullptr;
    int loadingScreenW = 0;
    int loadingScreenH = 0;

    GPU_Image* worldTexture = nullptr;
    GPU_Image* lightingTexture = nullptr;

    GPU_Image* emissionTexture = nullptr;
    vector< unsigned char > pixelsEmission;
    unsigned char* pixelsEmission_ar = nullptr;

    GPU_Image* texture = nullptr;
    vector< unsigned char > pixels;
    unsigned char* pixels_ar = nullptr;
    GPU_Image* textureLayer2 = nullptr;
    vector< unsigned char > pixelsLayer2;
    unsigned char* pixelsLayer2_ar = nullptr;
    GPU_Image* textureBackground = nullptr;
    vector< unsigned char > pixelsBackground;
    unsigned char* pixelsBackground_ar = nullptr;
    GPU_Image* textureObjects = nullptr;
    GPU_Image* textureObjectsLQ = nullptr;
    vector< unsigned char > pixelsObjects;
    unsigned char* pixelsObjects_ar = nullptr;
    GPU_Image* textureObjectsBack = nullptr;
    GPU_Image* textureParticles = nullptr;
    vector< unsigned char > pixelsParticles;
    unsigned char* pixelsParticles_ar = nullptr;
    GPU_Image* textureEntities = nullptr;
    GPU_Image* textureEntitiesLQ = nullptr;

    GPU_Image* textureFire = nullptr;
    GPU_Image* texture2Fire = nullptr;
    vector< unsigned char > pixelsFire;
    unsigned char* pixelsFire_ar = nullptr;

    GPU_Image* textureFlowSpead = nullptr;
    GPU_Image* textureFlow = nullptr;
    vector< unsigned char > pixelsFlow;
    unsigned char* pixelsFlow_ar = nullptr;

    GPU_Image* temperatureMap = nullptr;
    vector< unsigned char > pixelsTemp;
    unsigned char* pixelsTemp_ar = nullptr;

    b2DebugDraw_impl* b2DebugDraw;

    int ent_prevLoadZoneX = 0;
    int ent_prevLoadZoneY = 0;
    ctpl::thread_pool* updateDirtyPool = nullptr;
    ctpl::thread_pool* rotateVectorsPool = nullptr;

    uint16_t* movingTiles;
    void updateMaterialSounds();

    int tickTime = 0;

    bool running = true;

    World* world = nullptr;

    float accLoadX = 0;
    float accLoadY = 0;

    int mx = 0;
    int my = 0;
    int lastDrawMX = 0;
    int lastDrawMY = 0;
    int lastEraseMX = 0;
    int lastEraseMY = 0;

    bool* objectDelete = nullptr;

    WaterShader* waterShader = nullptr;
    WaterFlowPassShader* waterFlowPassShader = nullptr;
    NewLightingShader* newLightingShader = nullptr;
    float newLightingShader_insideDes = 0.0f;
    float newLightingShader_insideCur = 0.0f;
    FireShader* fireShader = nullptr;
    Fire2Shader* fire2Shader = nullptr;
    void loadShaders();

    int fps = 0;
    int feelsLikeFps = 0;
    long long lastTime = 0;
    long long lastTick = 0;
    long long lastLoadingTick = 0;
    long long now = 0;
    long long startTime = 0;
    long long deltaTime;
    long mspt = 0;
    uint32 loadingOnColor = 0;
    uint32 loadingOffColor = 0;

    DrawTextParams dt_versionInfo1;
    DrawTextParams dt_versionInfo2;
    DrawTextParams dt_versionInfo3;

    DrawTextParams dt_fps;
    DrawTextParams dt_feelsLikeFps;

    DrawTextParams dt_frameGraph[5];
    DrawTextParams dt_loading;

    long long fadeInStart = 0;
    long long fadeInLength = 0;
    int fadeInWaitFrames = 0;

    long long fadeOutStart = 0;
    long long fadeOutLength = 0;
    int fadeOutWaitFrames = 0;
    std::function<void()> fadeOutCallback = []() {};

    GameDir gameDir;

    int init(int argc, char* argv[]);

    int run(int argc, char *argv[]);

    void updateFrameEarly();
    void tick();
    void tickChunkLoading();
    void tickPlayer();
    void updateFrameLate();
    void renderOverlays();

    void renderEarly();
    void renderLate();

    void renderTemperatureMap(World* world);

    int getAimSolidSurface(int dist);
    int getAimSurface(int dist);

    void quitToMainMenu();

};
