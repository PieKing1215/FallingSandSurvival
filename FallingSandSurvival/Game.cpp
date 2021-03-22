
#include "Game.hpp"
#include <iterator>
#include <regex>

#include "DefaultGenerator.cpp"
#include "MaterialTestGenerator.cpp"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "UIs.hpp"

#include <GL/gl3w.h>

#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

#include "DiscordIntegration.hpp"

#define timegm _mkgmtime

#define W_PI 3.14159265358979323846

void Game::updateMaterialSounds() {
    uint16_t waterCt = min(movingTiles[Materials::WATER.id], (uint16_t)5000);
    float water = (float)waterCt / 3000;
    //logDebug("{} / {} = {}", waterCt, 3000, water);
    audioEngine.SetEventParameter("event:/World/WaterFlow", "FlowIntensity", water);
}

#ifdef _WIN32
/**
 Speeds up future calls to SDL_CreateWindow with SDL_WINDOW_OPENGL
 See https://hero.handmade.network/forums/code-discussion/t/2503-%5Bday_235%5D_opengls_pixel_format_takes_a_long_time
 On my machine this saves about 100ms during init
 */
DWORD WINAPI glSpeedUp(LPVOID param) {
    HDC dc = GetDC(NULL);
    DescribePixelFormat(dc, 0, 0, NULL);
    ReleaseDC(NULL, dc);
    return 0;
};
#endif

int Game::init(int argc, char *argv[]) {
    if(clArgs->getBool("profiler")) {
        EASY_PROFILER_ENABLE;
        profiler::startListen();
    }
    EASY_MAIN_THREAD;
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    EASY_EVENT("Start Loading", profiler::colors::Magenta);

    networkMode = clArgs->getBool("server") ? NetworkMode::SERVER : NetworkMode::HOST;

    EASY_BLOCK("warm up opengl");
    if(networkMode != NetworkMode::SERVER) {
        #ifdef _WIN32
        // see comment on glSpeedUp
        CreateThread(NULL, 3 * 64 * 1024, &glSpeedUp, NULL, STACK_SIZE_PARAM_IS_A_RESERVATION, NULL);
        #endif
    }
    EASY_END_BLOCK;

    // init console & print title
    #pragma region
    EASY_BLOCK("init console");
    #ifdef _WIN32
    if(_setmode(_fileno(stdout), _O_WTEXT) == -1) {
        std::wcout << "Error initting console: " << std::strerror(errno) << std::endl;
    }
    #endif

    EASY_BLOCK("print ascii title");
    std::locale ulocale(locale(), new codecvt_utf8<wchar_t>);
    std::wifstream ifs("assets/title.txt");
    std::locale prevlocale = ifs.imbue(ulocale);
    #ifndef _WIN32
    std::wcout.sync_with_stdio(false);
    std::wcout.imbue(std::locale("en_US.utf8"));
    #endif
    std::wcout << ifs.rdbuf() << std::endl;
    EASY_END_BLOCK;

    EASY_BLOCK("start spdlog");
    spdlog::set_level(spdlog::level::trace);
    spdlog::set_pattern("%D %T (%s:%#) [%^%l%$] \t%v");
    EASY_END_BLOCK;
    EASY_END_BLOCK;
    #pragma endregion

    logInfo("Starting game...");

    bool openDebugUIs = clArgs->getBool("debug");
    DebugUI::visible = openDebugUIs;
    DebugCheatsUI::visible = openDebugUIs;
    DebugDrawUI::visible = openDebugUIs;
    Settings::draw_frame_graph = openDebugUIs;
    if(!openDebugUIs) {
        Settings::draw_background = true;
        Settings::draw_background_grid = false;
        Settings::draw_load_zones = false;
        Settings::draw_physics_debug = false;
        Settings::draw_chunk_state = false;
        Settings::draw_debug_stats = false;
        Settings::draw_detailed_material_info = false;
        Settings::draw_temperature_map = false;

        //TODO: load video settings from settings file

    }

    this->gameDir = GameDir(clArgs->getString("game-dir"));

    Networking::init();
    if(networkMode == NetworkMode::SERVER) {
        int port = 1337;
        if(argc >= 3) {
            port = atoi(argv[2]);
        }
        server = Server::start(port);
        //SDL_SetWindowTitle(window, "Falling Sand Survival (Server)");

        /*while (true) {
            logDebug("[SERVER] tick {0:d}", server->server->connectedPeers);
            server->tick();
            Sleep(500);
        }
        return 0;*/

    } else {
        client = Client::start();
        //client->connect("172.23.16.150", 1337);

        //while (true) {
        //	logDebug("[CLIENT] tick");

        //	/* Create a reliable packet of size 7 containing "packet\0" */
        //	ENetPacket * packet = enet_packet_create("keepalive",
        //		strlen("keepalive") + 1, 0);
        //	/* Send the packet to the peer over channel id 0. */
        //	/* One could also broadcast the packet by         */
        //	/* enet_host_broadcast (host, 0, packet);         */
        //	enet_peer_send(client->peer, 0, packet);

        //	for (int i = 0; i < 1000; i++) {
        //		client->tick();
        //		Sleep(1);
        //	}
        //}
        //return 0;

        //SDL_SetWindowTitle(window, "Falling Sand Survival (Client)");
    }

    ctpl::thread_pool* initThreadPool = new ctpl::thread_pool(1);
    std::future<void> initThread;
    ctpl::thread_pool* worldInitThreadPool = new ctpl::thread_pool(1);
    std::future<void> worldInitThread;
    if(networkMode != NetworkMode::SERVER) {
        #if BUILD_WITH_STEAM
        // SteamAPI_RestartAppIfNecessary
        #pragma region
        logInfo("SteamAPI_RestartAppIfNecessary...");
        EASY_BLOCK("SteamAPI_RestartAppIfNecessary", STEAM_PROFILER_COLOR);
        if(SteamAPI_RestartAppIfNecessary(STEAM_APPID)) {
            return 1; // restart game through steam (if ran directly)
        }
        EASY_END_BLOCK;
        #pragma endregion
        #endif

        // init fmod
        #pragma region

        initThread = initThreadPool->push([&](int id) {
            EASY_THREAD("Async init");
            EASY_BLOCK("init fmod");
            logInfo("Initializing audio engine...");

            EASY_BLOCK("init audioEngine");
            audioEngine.Init();
            EASY_END_BLOCK;

            EASY_BLOCK("load banks");
            audioEngine.LoadBank("assets/audio/fmod/Build/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
            audioEngine.LoadBank("assets/audio/fmod/Build/Desktop/Master.strings.bank", FMOD_STUDIO_LOAD_BANK_NORMAL);
            EASY_END_BLOCK;

            EASY_BLOCK("load events");
            audioEngine.LoadEvent("event:/Music/Title");

            audioEngine.LoadEvent("event:/Player/Jump");
            audioEngine.LoadEvent("event:/Player/Fly");
            audioEngine.LoadEvent("event:/Player/Wind");
            audioEngine.LoadEvent("event:/Player/Impact");

            audioEngine.LoadEvent("event:/World/Sand");
            audioEngine.LoadEvent("event:/World/WaterFlow");

            audioEngine.LoadEvent("event:/GUI/GUI_Hover");
            audioEngine.LoadEvent("event:/GUI/GUI_Slider");
            EASY_END_BLOCK;

            EASY_BLOCK("play events");
            audioEngine.PlayEvent("event:/Player/Fly");
            audioEngine.PlayEvent("event:/Player/Wind");
            audioEngine.PlayEvent("event:/World/Sand");
            audioEngine.PlayEvent("event:/World/WaterFlow");
            EASY_END_BLOCK;

            EASY_END_BLOCK; // init fmod
        });
        #pragma endregion
    }

    // init sdl
    #pragma region
    EASY_BLOCK("init SDL");
    EASY_BLOCK("SDL_Init", SDL_PROFILER_COLOR);
    logInfo("Initializing SDL...");
    uint32 sdl_init_flags = SDL_INIT_VIDEO | SDL_INIT_EVENTS;
    if(SDL_Init(sdl_init_flags) < 0) {
        logCritical("SDL_Init failed: {}", SDL_GetError());
        return EXIT_FAILURE;
    }

    //SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "direct3d11", SDL_HINT_OVERRIDE);
    SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_OVERRIDE);
    EASY_END_BLOCK;

    EASY_BLOCK("TTF_Init");
    logInfo("Initializing SDL_TTF...");
    if(TTF_Init() != 0) {
        logCritical("TTF_Init failed: {}", TTF_GetError());
        return EXIT_FAILURE;
    }
    EASY_END_BLOCK;

    if(networkMode != NetworkMode::SERVER) {
        EASY_BLOCK("load fonts");
        font64 = TTF_OpenFont("assets/fonts/pixel_operator/PixelOperator.ttf", 64);
        font16 = TTF_OpenFont("assets/fonts/pixel_operator/PixelOperator.ttf", 16);
        font14 = TTF_OpenFont("assets/fonts/pixel_operator/PixelOperator.ttf", 14);
        EASY_END_BLOCK;
    }

    EASY_BLOCK("IMG_Init");
    logInfo("Initializing SDL_IMG...");
    int imgFlags = IMG_INIT_PNG;
    if(!(IMG_Init(imgFlags) & imgFlags)) {
        logCritical("IMG_Init failed: {}", IMG_GetError());
        return EXIT_FAILURE;
    }
    EASY_END_BLOCK;
    EASY_END_BLOCK;
    #pragma endregion

    if(networkMode != NetworkMode::SERVER) {
        // create the window
        #pragma region
        EASY_BLOCK("create window");
        logInfo("Creating game window...");
        EASY_BLOCK("SDL_CreateWindow", SDL_PROFILER_COLOR);
        window = SDL_CreateWindow("Falling Sand Survival", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
        EASY_END_BLOCK;

        if(window == nullptr) {
            logCritical("Could not create SDL_Window: {}", SDL_GetError());
            return EXIT_FAILURE;
        }

        #ifdef _WIN32
        // get windows specific handle
        SDL_SysWMinfo wmInfo;
        SDL_VERSION(&wmInfo.version);
        SDL_GetWindowWMInfo(window, &wmInfo);
        this->hwnd = wmInfo.info.win.window;

        // get windows taskbar handler
        HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&win_taskbar));
        if(SUCCEEDED(hr)) {
            hr = win_taskbar->HrInit();
            if(FAILED(hr)) {
                win_taskbar->Release();
                win_taskbar = NULL;
            }
        }
        #endif

        setWindowProgress(WindowProgressState::INDETERMINATE, 0.0f);

        EASY_BLOCK("SDL_SetWindowIcon", SDL_PROFILER_COLOR);
        SDL_SetWindowIcon(window, Textures::loadTexture("assets/Icon_32x.png"));
        EASY_END_BLOCK;
        EASY_END_BLOCK;
        #pragma endregion

        // create gpu target
        #pragma region
        EASY_BLOCK("create gpu target");
        logInfo("Creating gpu target...");
        EASY_BLOCK("GPU_SetPreInitFlags", GPU_PROFILER_COLOR);
        GPU_SetDebugLevel(GPU_DEBUG_LEVEL_MAX);
        GPU_SetPreInitFlags(GPU_INIT_DISABLE_VSYNC);
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_SetInitWindow", GPU_PROFILER_COLOR);
        GPU_SetInitWindow(SDL_GetWindowID(window));
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_Init", GPU_PROFILER_COLOR);
        target = GPU_Init(WIDTH, HEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
        EASY_END_BLOCK;

        if(target == NULL) {
            logCritical("Could not create GPU_Target: {}", SDL_GetError());
            return EXIT_FAILURE;
        }
        realTarget = target;
        EASY_END_BLOCK;
        #pragma endregion

        SDL_GLContext& gl_context = target->context->context;

        SDL_GL_MakeCurrent(window, gl_context);

        bool err = gl3wInit() != 0;

        if(err) {
            fprintf(stderr, "Failed to initialize OpenGL loader!\n");
            return EXIT_FAILURE;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::GetIO().Fonts->AddFontDefault();
        ImGui::GetIO().Fonts->AddFontFromFileTTF("assets/fonts/pixel_operator/PixelOperator.ttf", 32);

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);

        const char* glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);

        ImGui::StyleColorsClassic();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
        colors[ImGuiCol_TextDisabled]           = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.72f);
        colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.11f, 0.14f, 0.92f);
        colors[ImGuiCol_Border]                 = ImVec4(1.00f, 1.00f, 1.00f, 0.64f);
        colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.43f, 0.43f, 0.43f, 0.43f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.47f, 0.47f, 0.69f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.42f, 0.41f, 0.64f, 0.69f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.41f, 0.47f, 0.60f, 0.83f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.29f, 0.43f, 0.73f, 0.87f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.37f, 0.48f, 0.69f, 0.46f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.27f, 0.32f, 0.44f, 0.87f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.44f, 0.60f, 0.86f, 0.44f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.54f, 0.67f, 0.87f, 0.44f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.64f, 0.74f, 0.90f, 0.44f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.76f, 0.91f, 0.75f, 0.58f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.41f, 0.39f, 0.80f, 0.60f);
        colors[ImGuiCol_Button]                 = ImVec4(0.35f, 0.40f, 0.61f, 0.62f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.40f, 0.48f, 0.71f, 0.79f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.46f, 0.54f, 0.80f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.40f, 0.44f, 0.90f, 0.45f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.53f, 0.53f, 0.87f, 0.80f);
        colors[ImGuiCol_Separator]              = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.60f, 0.60f, 0.70f, 1.00f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.70f, 0.70f, 0.90f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(1.00f, 1.00f, 1.00f, 0.16f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.78f, 0.82f, 1.00f, 0.60f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.78f, 0.82f, 1.00f, 0.90f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.35f, 0.41f, 0.57f, 0.77f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.45f, 0.56f, 0.90f, 0.80f);
        colors[ImGuiCol_TabActive]              = ImVec4(0.31f, 0.41f, 0.86f, 0.84f);
        colors[ImGuiCol_TabUnfocused]           = ImVec4(0.28f, 0.28f, 0.57f, 0.82f);
        colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.35f, 0.35f, 0.65f, 0.84f);
        colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.66f, 0.77f, 0.99f, 0.35f);
        colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight]           = ImVec4(0.45f, 0.45f, 0.90f, 0.80f);
        colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

        // load splash screen
        #pragma region
        EASY_BLOCK("load splash screen");
        logInfo("Loading splash screen...");
        EASY_BLOCK("GPU_Clear", GPU_PROFILER_COLOR);
        GPU_Clear(target);
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_Flip", GPU_PROFILER_COLOR);
        GPU_Flip(target);
        EASY_END_BLOCK;

        SDL_Surface* splashSurf = Textures::loadTexture("assets/title/splash.png");
        EASY_BLOCK("GPU_CopyImageFromSurface", GPU_PROFILER_COLOR);
        GPU_Image* splashImg = GPU_CopyImageFromSurface(splashSurf);
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
        GPU_SetImageFilter(splashImg, GPU_FILTER_NEAREST);
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_BlitRect", GPU_PROFILER_COLOR);
        GPU_BlitRect(splashImg, NULL, target, NULL);
        EASY_END_BLOCK;

        EASY_BLOCK("GPU_FreeImage", GPU_PROFILER_COLOR);
        GPU_FreeImage(splashImg);
        EASY_END_BLOCK;
        EASY_BLOCK("SDL_FreeSurface", SDL_PROFILER_COLOR);
        SDL_FreeSurface(splashSurf);
        EASY_END_BLOCK;
        EASY_BLOCK("GPU_Flip", GPU_PROFILER_COLOR);
        GPU_Flip(target);
        EASY_END_BLOCK;
        EASY_EVENT("splash screen visible", profiler::colors::Green);

        EASY_BLOCK("wait for Fmod", THREAD_WAIT_PROFILER_COLOR);
        initThread.get();
        EASY_END_BLOCK;
        EASY_BLOCK("play audio");
        audioEngine.PlayEvent("event:/Music/Title");
        audioEngine.Update();
        EASY_END_BLOCK;
        EASY_END_BLOCK;
        #pragma endregion
    }

    // init the world
    #pragma region
    // calls IMG_Load and SDL_ConvertSurfaceFormat which I think are thread safe but not actually sure
    worldInitThread = worldInitThreadPool->push([&](int id) {
        EASY_THREAD("World init");
        EASY_BLOCK("init world");
        logInfo("Initializing world...");
        world = new World();
        world->noSaveLoad = true;
        world->init(gameDir.getWorldPath("mainMenu"), (int)ceil(MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, target, &audioEngine, networkMode);
        EASY_END_BLOCK;
    });
    #pragma endregion

    if(networkMode != NetworkMode::SERVER) {
        // init steam and discord
        #pragma region
        initThread = initThreadPool->push([&](int id) {
            EASY_THREAD("Async init");

            #if BUILD_WITH_STEAM
            // load SteamAPI
            #pragma region
            logInfo("Initializing SteamAPI...");
            EASY_BLOCK("Init SteamAPI", STEAM_PROFILER_COLOR);
            EASY_BLOCK("SteamAPI_Init", STEAM_PROFILER_COLOR);
            steamAPI = SteamAPI_Init();
            EASY_END_BLOCK;
            if(!steamAPI) {
                logError("SteamAPI_Init failed.");
            } else {
                logInfo("SteamAPI_Init successful.");
                EASY_BLOCK("SteamHookMessages", STEAM_PROFILER_COLOR);
                SteamHookMessages();
                EASY_END_BLOCK;
            }
            EASY_END_BLOCK;
            #pragma endregion
            #endif

            #if BUILD_WITH_DISCORD
            // init discord
            #pragma region
            logInfo("Initializing Discord Game SDK...");
            EASY_BLOCK("init discord", DISCORD_PROFILER_COLOR);
            if(DiscordIntegration::init()) {
                EASY_BLOCK("set activity", DISCORD_PROFILER_COLOR);
                DiscordIntegration::setActivityState("On Main Menu");
                DiscordIntegration::flushActivity();
                EASY_END_BLOCK;
            }
            EASY_END_BLOCK;
            #pragma endregion
            #endif
        });
    
        // init the background
        #pragma region
        EASY_BLOCK("load backgrounds");
        logInfo("Loading backgrounds...");
        Backgrounds::TEST_OVERWORLD.init();
        EASY_END_BLOCK;
        #pragma endregion
    }

    // init the rng
    #pragma region
    EASY_BLOCK("seed RNG");
    logInfo("Seeding RNG...");
    unsigned int seed = (unsigned int)Time::millis();
    srand(seed);
    EASY_END_BLOCK;
    #pragma endregion

    // register & set up materials
    #pragma region
    EASY_BLOCK("init materials");
    logInfo("Setting up materials...");
    Materials::init();
    EASY_END_BLOCK;
    #pragma endregion

    movingTiles = new uint16_t[Materials::nMaterials];

    b2DebugDraw = new b2DebugDraw_impl(target);

    EASY_BLOCK("wait for worldInitThread", THREAD_WAIT_PROFILER_COLOR);
    worldInitThread.get();
    EASY_END_BLOCK;

    if(networkMode != NetworkMode::SERVER) {
        // set up main menu ui
        #pragma region
        EASY_BLOCK("init main menu UI");
        logInfo("Setting up main menu...");
        TTF_Font* labelFont = TTF_OpenFont("assets/fonts/pixel_operator/PixelOperator.ttf", 32);
        TTF_Font* uiFont = TTF_OpenFont("assets/fonts/pixel_operator/PixelOperator.ttf", 16);
        EASY_END_BLOCK;


        std::string displayMode = clArgs->getString("display-mode");

        if(displayMode == "windowed") {
            setDisplayMode(DisplayMode::WINDOWED);
        } else if(displayMode == "borderless") {
            setDisplayMode(DisplayMode::BORDERLESS);
        } else if(displayMode == "fullscreen") {
            setDisplayMode(DisplayMode::FULLSCREEN);
        }

        setVSync(clArgs->getBool("vsync"));

        // TODO: load settings from settings file
        // also note OptionsUI::minimizeOnFocus exists
        setMinimizeOnLostFocus(false);
    }

    // init threadpools
    #pragma region
    EASY_BLOCK("init threadpools");
    updateDirtyPool = new ctpl::thread_pool(6);
    rotateVectorsPool = new ctpl::thread_pool(3);
    EASY_END_BLOCK;
    #pragma endregion

    if(networkMode != NetworkMode::SERVER) {
        // load shaders
        loadShaders();

        EASY_BLOCK("wait for steam/discord init", THREAD_WAIT_PROFILER_COLOR);
        initThread.get(); // steam & discord
        EASY_END_BLOCK;
    }

    EASY_EVENT("Done Loading", profiler::colors::Magenta);
    EASY_END_BLOCK;

    return this->run(argc, argv);
}

void Game::loadShaders() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    if(waterShader) delete waterShader;
    if(waterFlowPassShader) delete waterFlowPassShader;
    if(newLightingShader) delete newLightingShader;
    if(fireShader) delete fireShader;
    if(fire2Shader) delete fire2Shader;

    waterShader = new WaterShader();
    waterFlowPassShader = new WaterFlowPassShader();
    newLightingShader = new NewLightingShader();
    fireShader = new FireShader();
    fire2Shader = new Fire2Shader();
}

void Game::handleWindowSizeChange(int newWidth, int newHeight) {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    int prevWidth = WIDTH;
    int prevHeight = HEIGHT;

    WIDTH = newWidth;
    HEIGHT = newHeight;

    if(loadingTexture) {
        GPU_FreeImage(loadingTexture);
        GPU_FreeImage(texture);
        GPU_FreeImage(worldTexture);
        GPU_FreeImage(lightingTexture);
        GPU_FreeImage(emissionTexture);
        GPU_FreeImage(textureFire);
        GPU_FreeImage(textureFlow);
        GPU_FreeImage(texture2Fire);
        GPU_FreeImage(textureLayer2);
        GPU_FreeImage(textureBackground);
        GPU_FreeImage(textureObjects);
        GPU_FreeImage(textureObjectsLQ);
        GPU_FreeImage(textureObjectsBack);
        GPU_FreeImage(textureParticles);
        GPU_FreeImage(textureEntities);
        GPU_FreeImage(textureEntitiesLQ);
        GPU_FreeImage(temperatureMap);
        GPU_FreeImage(backgroundImage);
    }

    // create textures
    #pragma region
    EASY_BLOCK("create textures");
    logInfo("Creating world textures...");
    loadingOnColor = 0xFFFFFFFF;
    loadingOffColor = 0x000000FF;
    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    loadingTexture = GPU_CreateImage(
        loadingScreenW = (WIDTH / 20), loadingScreenH = (HEIGHT / 20),
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(loadingTexture, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    texture = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    worldTexture = GPU_CreateImage(
        world->width * Settings::hd_objects_size, world->height * Settings::hd_objects_size,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(worldTexture, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(worldTexture);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    lightingTexture = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(lightingTexture, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(lightingTexture);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    emissionTexture = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(emissionTexture, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureFlow = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureFlow, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;


    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureFlowSpead = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureFlowSpead, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureFlowSpead);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureFire = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureFire, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    texture2Fire = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(texture2Fire, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(texture2Fire);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureLayer2 = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureLayer2, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureBackground = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureBackground, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureObjects = GPU_CreateImage(
        world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureObjects, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureObjectsLQ = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureObjectsLQ, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureObjectsBack = GPU_CreateImage(
        world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureObjectsBack, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureObjects);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureObjectsLQ);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureObjectsBack);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureParticles = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureParticles, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureEntities = GPU_CreateImage(
        world->width * (Settings::hd_objects ? Settings::hd_objects_size : 1), world->height * (Settings::hd_objects ? Settings::hd_objects_size : 1),
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureEntities);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureEntities, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    textureEntitiesLQ = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(textureEntitiesLQ);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(textureEntitiesLQ, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    temperatureMap = GPU_CreateImage(
        world->width, world->height,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(temperatureMap, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;

    EASY_BLOCK("GPU_CreateImage", GPU_PROFILER_COLOR);
    backgroundImage = GPU_CreateImage(
        WIDTH, HEIGHT,
        GPU_FormatEnum::GPU_FORMAT_RGBA
    );
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_SetImageFilter", GPU_PROFILER_COLOR);
    GPU_SetImageFilter(backgroundImage, GPU_FILTER_NEAREST);
    EASY_END_BLOCK;
    EASY_BLOCK("GPU_LoadTarget", GPU_PROFILER_COLOR);
    GPU_LoadTarget(backgroundImage);
    EASY_END_BLOCK;

    // create texture pixel buffers
    EASY_BLOCK("create texture pixel buffers");
    pixels = vector<unsigned char>(world->width * world->height * 4, 0);
    pixels_ar = &pixels[0];
    pixelsLayer2 = vector<unsigned char>(world->width * world->height * 4, 0);
    pixelsLayer2_ar = &pixelsLayer2[0];
    pixelsBackground = vector<unsigned char>(world->width * world->height * 4, 0);
    pixelsBackground_ar = &pixelsBackground[0];
    pixelsObjects = vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
    pixelsObjects_ar = &pixelsObjects[0];
    pixelsTemp = vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
    pixelsTemp_ar = &pixelsTemp[0];
    pixelsParticles = vector<unsigned char>(world->width * world->height * 4, SDL_ALPHA_TRANSPARENT);
    pixelsParticles_ar = &pixelsParticles[0];
    pixelsLoading = vector<unsigned char>(loadingTexture->w * loadingTexture->h * 4, SDL_ALPHA_TRANSPARENT);
    pixelsLoading_ar = &pixelsLoading[0];
    pixelsFire = vector<unsigned char>(world->width * world->height * 4, 0);
    pixelsFire_ar = &pixelsFire[0];
    pixelsFlow = vector<unsigned char>(world->width * world->height * 4, 0);
    pixelsFlow_ar = &pixelsFlow[0];
    pixelsEmission = vector<unsigned char>(world->width * world->height * 4, 0);
    pixelsEmission_ar = &pixelsEmission[0];
    EASY_END_BLOCK;
    EASY_END_BLOCK;
    #pragma endregion

    accLoadX -= (newWidth - prevWidth) / 2.0f / scale;
    accLoadY -= (newHeight - prevHeight) / 2.0f / scale;

    tickChunkLoading();

    for(int x = 0; x < world->width; x++) {
        for(int y = 0; y < world->height; y++) {
            world->dirty[x + y * world->width] = true;
            world->layer2Dirty[x + y * world->width] = true;
            world->backgroundDirty[x + y * world->width] = true;
        }
    }

}

void Game::setWindowProgress(WindowProgressState state, float value) {
    // TODO: look into alternatives for linux/crossplatform
    #ifdef _WIN32

    if(!hwnd || !win_taskbar) return;

    switch(state) {
    case WindowProgressState::NONE:
        win_taskbar->SetProgressState(hwnd, TBPF_NOPROGRESS);
        break;
    case WindowProgressState::INDETERMINATE:
        win_taskbar->SetProgressState(hwnd, TBPF_INDETERMINATE);
        break;
    case WindowProgressState::NORMAL:
        win_taskbar->SetProgressState(hwnd, TBPF_NORMAL);
        win_taskbar->SetProgressValue(hwnd, value * 10000, 10000);
        break;
    case WindowProgressState::PAUSED:
        win_taskbar->SetProgressValue(hwnd, value * 10000, 10000);
        win_taskbar->SetProgressState(hwnd, TBPF_PAUSED);
        break;
    case WindowProgressState::ERROR:
        win_taskbar->SetProgressValue(hwnd, value * 10000, 10000);
        win_taskbar->SetProgressState(hwnd, TBPF_ERROR);
        break;
    }

    #endif
}

void Game::setWindowFlash(WindowFlashAction action, int count, int period) {
    // TODO: look into alternatives for linux/crossplatform
    #ifdef _WIN32

    FLASHWINFO flash;
    flash.cbSize = sizeof(FLASHWINFO);
    flash.hwnd = hwnd;
    flash.uCount = count;
    flash.dwTimeout = period;

    // pretty sure these flags are supposed to work but they all seem to do the same thing on my machine so idk
    switch(action) {
    case WindowFlashAction::START:
        flash.dwFlags = FLASHW_ALL;
        break;
    case WindowFlashAction::START_COUNT:
        flash.dwFlags = FLASHW_ALL | FLASHW_TIMER;
        break;
    case WindowFlashAction::START_UNTIL_FG:
        flash.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
        break;
    case WindowFlashAction::STOP:
        flash.dwFlags = FLASHW_STOP;
        break;
    }

    FlashWindowEx(&flash);

    #endif
}

void Game::setDisplayMode(DisplayMode mode) {
    switch(mode) {
    case DisplayMode::WINDOWED:
        SDL_SetWindowDisplayMode(window, NULL);
        SDL_SetWindowFullscreen(window, 0);
        OptionsUI::item_current_idx = 0;
        break;
    case DisplayMode::BORDERLESS:
        SDL_SetWindowDisplayMode(window, NULL);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        OptionsUI::item_current_idx = 1;
        break;
    case DisplayMode::FULLSCREEN:
        SDL_MaximizeWindow(window);

        int w;
        int h;
        SDL_GetWindowSize(window, &w, &h);

        SDL_DisplayMode disp;
        SDL_GetWindowDisplayMode(window, &disp);

        disp.w = w;
        disp.h = h;

        SDL_SetWindowDisplayMode(window, &disp);
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        OptionsUI::item_current_idx = 2;
        break;
    }

    int w;
    int h;
    SDL_GetWindowSize(window, &w, &h);

    GPU_SetWindowResolution(w, h);
    GPU_ResetProjection(realTarget);

    handleWindowSizeChange(w, h);

}

void Game::setVSync(bool vsync) {
    SDL_GL_SetSwapInterval(vsync ? 1 : 0);
    OptionsUI::vsync = vsync;
}

void Game::setMinimizeOnLostFocus(bool minimize) {
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, minimize ? "1" : "0");
    OptionsUI::minimizeOnFocus = minimize;
}

int Game::run(int argc, char *argv[]) {
    startTime = Time::millis();

    // start loading chunks
    #pragma region
    EASY_BLOCK("Queue chunk loading");
    logInfo("Queueing chunk loading...");
    for(int x = -CHUNK_W * 4; x < world->width + CHUNK_W * 4; x += CHUNK_W) {
        for(int y = -CHUNK_H * 3; y < world->height + CHUNK_H * 8; y += CHUNK_H) {
            world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
        }
    }
    EASY_END_BLOCK;
    #pragma endregion

    // start game loop
    #pragma region
    EASY_BLOCK("start game loop");
    logInfo("Starting game loop...");

    freeCamX = world->width / 2.0f - CHUNK_W / 2;
    freeCamY = world->height / 2.0f - (int)(CHUNK_H * 0.75);
    if(world->player) {
        plPosX = world->player->x;
        plPosY = world->player->y;
    } else {
        plPosX = freeCamX;
        plPosY = freeCamY;
    }

    SDL_Event windowEvent;

    long long lastFPS = Time::millis();
    int frames = 0;
    fps = 0;

    lastTime = Time::millis();
    lastTick = lastTime;
    long long lastTickPhysics = lastTime;

    mspt = 33;
    long msptPhysics = 16;

    scale = 3;
    ofsX = (int)(-CHUNK_W * 4);
    ofsY = (int)(-CHUNK_H * 2.5);

    ofsX = (ofsX - WIDTH / 2) / 2 * 3 + WIDTH / 2;
    ofsY = (ofsY - HEIGHT / 2) / 2 * 3 + HEIGHT / 2;

    for(int i = 0; i < frameTimeNum; i++) {
        frameTime[i] = 0;
    }
    objectDelete = new bool[world->width * world->height];

    EASY_END_BLOCK; // start game loop
    #pragma endregion

    fadeInStart = Time::millis();
    fadeInLength = 250;
    fadeInWaitFrames = 5;

    // game loop
    EASY_EVENT("Start of Game Loop", profiler::colors::Magenta);
    while(this->running) {
        EASY_BLOCK("Frame", GAME_PROFILER_COLOR);
        now = Time::millis();
        deltaTime = now - lastTime;

        if(networkMode != NetworkMode::SERVER) {
            #if BUILD_WITH_DISCORD
            DiscordIntegration::tick();
            #endif

            #if BUILD_WITH_STEAM
            if(steamAPI) {
                EASY_BLOCK("SteamAPI tick");

                EASY_END_BLOCK;
            }
            #endif

            // handle window events
            #pragma region
            EASY_BLOCK("poll SDL events", SDL_PROFILER_COLOR);
            while(SDL_PollEvent(&windowEvent)) {

                if(windowEvent.type == SDL_QUIT) {
                    goto exit;
                }

                ImGui_ImplSDL2_ProcessEvent(&windowEvent);

                if(ImGui::GetIO().WantCaptureMouse) {
                    if(windowEvent.type == SDL_MOUSEBUTTONDOWN || windowEvent.type == SDL_MOUSEBUTTONUP || windowEvent.type == SDL_MOUSEMOTION || windowEvent.type == SDL_MOUSEWHEEL) {
                        continue;
                    }
                }

                if(ImGui::GetIO().WantCaptureKeyboard) {
                    if(windowEvent.type == SDL_KEYDOWN || windowEvent.type == SDL_KEYUP) {
                        continue;
                    }
                }

                if(windowEvent.type == SDL_MOUSEWHEEL) {
                    // zoom in/out
                    #pragma region
                    int deltaScale = windowEvent.wheel.y;
                    int oldScale = scale;
                    scale += deltaScale;
                    if(scale < 1) scale = 1;

                    ofsX = (ofsX - WIDTH / 2) / oldScale * scale + WIDTH / 2;
                    ofsY = (ofsY - HEIGHT / 2) / oldScale * scale + HEIGHT / 2;
                    #pragma endregion
                } else if(windowEvent.type == SDL_MOUSEMOTION) {
                    if(Controls::DEBUG_DRAW->get()) {
                        // draw material
                        #pragma region
                        int x = (int)((windowEvent.motion.x - ofsX - camX) / scale);
                        int y = (int)((windowEvent.motion.y - ofsY - camY) / scale);

                        if(lastDrawMX == 0 && lastDrawMY == 0) {
                            lastDrawMX = x;
                            lastDrawMY = y;
                        }

                        world->forLine(lastDrawMX, lastDrawMY, x, y, [&](int index) {
                            int lineX = index % world->width;
                            int lineY = index / world->width;

                            for(int xx = -DebugDrawUI::brushSize / 2; xx < (int)(ceil(DebugDrawUI::brushSize / 2.0)); xx++) {
                                for(int yy = -DebugDrawUI::brushSize / 2; yy < (int)(ceil(DebugDrawUI::brushSize / 2.0)); yy++) {
                                    if(lineX + xx < 0 || lineY + yy < 0 || lineX + xx >= world->width || lineY + yy >= world->height) continue;
                                    MaterialInstance tp = Tiles::create(DebugDrawUI::selectedMaterial, lineX + xx, lineY + yy);
                                    world->tiles[(lineX + xx) + (lineY + yy) * world->width] = tp;
                                    world->dirty[(lineX + xx) + (lineY + yy) * world->width] = true;
                                }
                            }

                            return false;
                        });

                        lastDrawMX = x;
                        lastDrawMY = y;
                        #pragma endregion
                    } else {
                        lastDrawMX = 0;
                        lastDrawMY = 0;
                    }

                    if(Controls::mmouse) {
                        // erase material
                        #pragma region
                        // erase from world
                        int x = (int)((windowEvent.motion.x - ofsX - camX) / scale);
                        int y = (int)((windowEvent.motion.y - ofsY - camY) / scale);

                        if(lastEraseMX == 0 && lastEraseMY == 0) {
                            lastEraseMX = x;
                            lastEraseMY = y;
                        }

                        world->forLine(lastEraseMX, lastEraseMY, x, y, [&](int index) {
                            int lineX = index % world->width;
                            int lineY = index / world->width;

                            for(int xx = -DebugDrawUI::brushSize / 2; xx < (int)(ceil(DebugDrawUI::brushSize / 2.0)); xx++) {
                                for(int yy = -DebugDrawUI::brushSize / 2; yy < (int)(ceil(DebugDrawUI::brushSize / 2.0)); yy++) {

                                    if(abs(xx) + abs(yy) == DebugDrawUI::brushSize) continue;
                                    if(world->getTile(lineX + xx, lineY + yy).mat->physicsType != PhysicsType::AIR) {
                                        world->setTile(lineX + xx, lineY + yy, Tiles::NOTHING);
                                        world->lastMeshZone.x--;
                                    }
                                    if(world->getTileLayer2(lineX + xx, lineY + yy).mat->physicsType != PhysicsType::AIR) {
                                        world->setTileLayer2(lineX + xx, lineY + yy, Tiles::NOTHING);
                                    }
                                }
                            }
                            return false;
                        });

                        lastEraseMX = x;
                        lastEraseMY = y;

                        // erase from rigidbodies
                        // this copies the vector
                        vector<RigidBody*> rbs = world->rigidBodies;

                        for(size_t i = 0; i < rbs.size(); i++) {
                            RigidBody* cur = rbs[i];
                            if(cur->body->IsEnabled()) {
                                float s = sin(-cur->body->GetAngle());
                                float c = cos(-cur->body->GetAngle());
                                bool upd = false;
                                for(float xx = -3; xx <= 3; xx += 0.5) {
                                    for(float yy = -3; yy <= 3; yy += 0.5) {
                                        if(abs(xx) + abs(yy) == 6) continue;
                                        // rotate point

                                        float tx = x + xx - cur->body->GetPosition().x;
                                        float ty = y + yy - cur->body->GetPosition().y;

                                        int ntx = (int)(tx * c - ty * s);
                                        int nty = (int)(tx * s + ty * c);

                                        if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                                            Uint32 pixel = PIXEL(cur->surface, ntx, nty);
                                            if(((pixel >> 24) & 0xff) != 0x00) {
                                                PIXEL(cur->surface, ntx, nty) = 0x00000000;
                                                upd = true;
                                            }

                                        }
                                    }
                                }

                                if(upd) {
                                    GPU_FreeImage(cur->texture);
                                    cur->texture = GPU_CopyImageFromSurface(cur->surface);
                                    GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
                                    //world->updateRigidBodyHitbox(cur);
                                    cur->needsUpdate = true;
                                }
                            }
                        }

                        #pragma endregion
                    } else {
                        lastEraseMX = 0;
                        lastEraseMY = 0;
                    }
                } else if(windowEvent.type == SDL_KEYDOWN) {
                    EASY_BLOCK("Controls::keyEvent");
                    Controls::keyEvent(windowEvent.key);
                    EASY_END_BLOCK;
                } else if(windowEvent.type == SDL_KEYUP) {
                    EASY_BLOCK("Controls::keyEvent");
                    Controls::keyEvent(windowEvent.key);
                    EASY_END_BLOCK;
                }

                if(windowEvent.type == SDL_MOUSEBUTTONDOWN) {
                    if(windowEvent.button.button == SDL_BUTTON_LEFT) {
                        Controls::lmouse = true;

                        if(world->player && world->player->heldItem != NULL) {
                            if(world->player->heldItem->getFlag(ItemFlags::VACUUM)) {
                                world->player->holdVacuum = true;
                            } else if(world->player->heldItem->getFlag(ItemFlags::HAMMER)) {
                                //#define HAMMER_DEBUG_PHYSICS
                                #ifdef HAMMER_DEBUG_PHYSICS
                                int x = (int)((windowEvent.button.x - ofsX - camX) / scale);
                                int y = (int)((windowEvent.button.y - ofsY - camY) / scale);

                                world->physicsCheck(x, y);
                                #else
                                mx = windowEvent.button.x;
                                my = windowEvent.button.y;
                                int startInd = getAimSolidSurface(64);

                                if(startInd != -1) {
                                    //world->player->hammerX = x;
                                    //world->player->hammerY = y;
                                    world->player->hammerX = startInd % world->width;
                                    world->player->hammerY = startInd / world->width;
                                    world->player->holdHammer = true;
                                    //logDebug("hammer down: {0:d} {0:d} {0:d} {0:d} {0:d}", x, y, startInd, startInd % world->width, startInd / world->width);
                                    //world->setTile(world->player->hammerX, world->player->hammerY, MaterialInstance(&Materials::GENERIC_SOLID, 0x00ff00ff));
                                }
                                #endif
                                #undef HAMMER_DEBUG_PHYSICS
                            } else if(world->player->heldItem->getFlag(ItemFlags::CHISEL)) {
                                // if hovering rigidbody, open in chisel
                                #pragma region
                                int x = (int)((mx - ofsX - camX) / scale);
                                int y = (int)((my - ofsY - camY) / scale);

                                vector<RigidBody*> rbs = world->rigidBodies; // copy
                                for(size_t i = 0; i < rbs.size(); i++) {
                                    RigidBody* cur = rbs[i];

                                    bool connect = false;
                                    if(cur->body->IsEnabled()) {
                                        float s = sin(-cur->body->GetAngle());
                                        float c = cos(-cur->body->GetAngle());
                                        bool upd = false;
                                        for(float xx = -3; xx <= 3; xx += 0.5) {
                                            for(float yy = -3; yy <= 3; yy += 0.5) {
                                                if(abs(xx) + abs(yy) == 6) continue;
                                                // rotate point

                                                float tx = x + xx - cur->body->GetPosition().x;
                                                float ty = y + yy - cur->body->GetPosition().y;

                                                int ntx = (int)(tx * c - ty * s);
                                                int nty = (int)(tx * s + ty * c);

                                                if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                                                    Uint32 pixel = PIXEL(cur->surface, ntx, nty);
                                                    if(((pixel >> 24) & 0xff) != 0x00) {
                                                        connect = true;
                                                    }

                                                }
                                            }
                                        }
                                    }

                                    if(connect) {

                                        // previously: open chisel ui

                                        break;
                                    }

                                }
                                #pragma endregion
                            } else if(world->player->heldItem->getFlag(ItemFlags::TOOL)) {
                                // break with pickaxe
                                #pragma region
                                float breakSize = world->player->heldItem->breakSize;

                                int x = (int)(world->player->x + world->player->hw / 2.0f + world->loadZone.x + 10 * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);
                                int y = (int)(world->player->y + world->player->hh / 2.0f + world->loadZone.y + 10 * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);

                                SDL_Surface* tex = SDL_CreateRGBSurfaceWithFormat(0, (int)breakSize, (int)breakSize, 32, SDL_PIXELFORMAT_ARGB8888);

                                int n = 0;
                                for(int xx = 0; xx < breakSize; xx++) {
                                    for(int yy = 0; yy < breakSize; yy++) {
                                        float cx = (float)((xx / breakSize) - 0.5);
                                        float cy = (float)((yy / breakSize) - 0.5);

                                        if(cx * cx + cy * cy > 0.25f) continue;

                                        if(world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SOLID) {
                                            PIXEL(tex, xx, yy) = world->tiles[(x + xx) + (y + yy) * world->width].color;
                                            world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
                                            world->dirty[(x + xx) + (y + yy) * world->width] = true;

                                            n++;
                                        }
                                    }
                                }

                                if(n > 0) {
                                    audioEngine.PlayEvent("event:/Player/Impact");
                                    b2PolygonShape s;
                                    s.SetAsBox(1, 1);
                                    RigidBody* rb = world->makeRigidBody(b2_dynamicBody, (float)x, (float)y, 0, s, 1, (float)0.3, tex);

                                    b2Filter bf = {};
                                    bf.categoryBits = 0x0001;
                                    bf.maskBits = 0xffff;
                                    rb->body->GetFixtureList()[0].SetFilterData(bf);

                                    rb->body->SetLinearVelocity({(float)((rand() % 100) / 100.0 - 0.5), (float)((rand() % 100) / 100.0 - 0.5)});

                                    world->rigidBodies.push_back(rb);
                                    world->updateRigidBodyHitbox(rb);

                                    world->lastMeshLoadZone.x--;
                                    world->updateWorldMesh();
                                }
                                #pragma endregion
                            }
                        }

                    } else if(windowEvent.button.button == SDL_BUTTON_RIGHT) {
                        Controls::rmouse = true;
                        if(world->player) world->player->startThrow = Time::millis();
                    } else if(windowEvent.button.button == SDL_BUTTON_MIDDLE) {
                        Controls::mmouse = true;
                    }
                } else if(windowEvent.type == SDL_MOUSEBUTTONUP) {
                    if(windowEvent.button.button == SDL_BUTTON_LEFT) {
                        Controls::lmouse = false;

                        if(world->player) {
                            if(world->player->heldItem) {
                                if(world->player->heldItem->getFlag(ItemFlags::VACUUM)) {
                                    if(world->player->holdVacuum) {
                                        world->player->holdVacuum = false;
                                    }
                                } else if(world->player->heldItem->getFlag(ItemFlags::HAMMER)) {
                                    if(world->player->holdHammer) {
                                        int x = (int)((windowEvent.button.x - ofsX - camX) / scale);
                                        int y = (int)((windowEvent.button.y - ofsY - camY) / scale);

                                        int dx = world->player->hammerX - x;
                                        int dy = world->player->hammerY - y;
                                        float len = sqrtf(dx * dx + dy * dy);
                                        float udx = dx / len;
                                        float udy = dy / len;

                                        int ex = world->player->hammerX + dx;
                                        int ey = world->player->hammerY + dy;
                                        logDebug("hammer up: {0:d} {0:d} {0:d} {0:d}", ex, ey, dx, dy);
                                        int endInd = -1;

                                        int nSegments = 1 + len / 10;
                                        std::vector<std::tuple<int, int>> points = {};
                                        for(int i = 0; i < nSegments; i++) {
                                            int sx = world->player->hammerX + (int)((float)(dx / nSegments) * (i + 1));
                                            int sy = world->player->hammerY + (int)((float)(dy / nSegments) * (i + 1));
                                            sx += rand() % 3 - 1;
                                            sy += rand() % 3 - 1;
                                            points.push_back(std::tuple<int, int>(sx, sy));
                                        }

                                        int nTilesChanged = 0;
                                        for(size_t i = 0; i < points.size(); i++) {
                                            int segSx = i == 0 ? world->player->hammerX : std::get<0>(points[i - 1]);
                                            int segSy = i == 0 ? world->player->hammerY : std::get<1>(points[i - 1]);
                                            int segEx = std::get<0>(points[i]);
                                            int segEy = std::get<1>(points[i]);

                                            bool hitSolidYet = false;
                                            bool broke = false;
                                            world->forLineCornered(segSx, segSy, segEx, segEy, [&](int index) {
                                                if(world->tiles[index].mat->physicsType != PhysicsType::SOLID) {
                                                    if(hitSolidYet && (abs((index % world->width) - segSx) + (abs((index / world->width) - segSy)) > 1)) {
                                                        broke = true;
                                                        return true;
                                                    }
                                                    return false;
                                                }
                                                hitSolidYet = true;
                                                world->tiles[index] = MaterialInstance(&Materials::GENERIC_SAND, Drawing::darkenColor(world->tiles[index].color, 0.5f));
                                                world->dirty[index] = true;
                                                endInd = index;
                                                nTilesChanged++;
                                                return false;
                                            });

                                            //world->setTile(segSx, segSy, MaterialInstance(&Materials::GENERIC_SOLID, 0x00ff00ff));
                                            if(broke) break;
                                        }

                                        //world->setTile(ex, ey, MaterialInstance(&Materials::GENERIC_SOLID, 0xff0000ff));

                                        int hx = (world->player->hammerX + (endInd % world->width)) / 2;
                                        int hy = (world->player->hammerY + (endInd / world->width)) / 2;

                                        if(world->getTile((int)(hx + udy * 2), (int)(hy - udx * 2)).mat->physicsType == PhysicsType::SOLID) {
                                            world->physicsCheck((int)(hx + udy * 2), (int)(hy - udx * 2));
                                        }

                                        if(world->getTile((int)(hx - udy * 2), (int)(hy + udx * 2)).mat->physicsType == PhysicsType::SOLID) {
                                            world->physicsCheck((int)(hx - udy * 2), (int)(hy + udx * 2));
                                        }

                                        if(nTilesChanged > 0) {
                                            audioEngine.PlayEvent("event:/Player/Impact");
                                        }

                                        //world->setTile((int)(hx), (int)(hy), MaterialInstance(&Materials::GENERIC_SOLID, 0xffffffff));
                                        //world->setTile((int)(hx + udy * 6), (int)(hy - udx * 6), MaterialInstance(&Materials::GENERIC_SOLID, 0xffff00ff));
                                        //world->setTile((int)(hx - udy * 6), (int)(hy + udx * 6), MaterialInstance(&Materials::GENERIC_SOLID, 0x00ffffff));

                                    }
                                    world->player->holdHammer = false;
                                }
                            }
                        }
                    } else if(windowEvent.button.button == SDL_BUTTON_RIGHT) {
                        Controls::rmouse = false;
                        // pick up / throw item
                        #pragma region
                        int x = (int)((mx - ofsX - camX) / scale);
                        int y = (int)((my - ofsY - camY) / scale);

                        bool swapped = false;
                        vector<RigidBody*> rbs = world->rigidBodies; // copy;
                        for(size_t i = 0; i < rbs.size(); i++) {
                            RigidBody* cur = rbs[i];

                            bool connect = false;
                            if(cur->body->IsEnabled()) {
                                float s = sin(-cur->body->GetAngle());
                                float c = cos(-cur->body->GetAngle());
                                bool upd = false;
                                for(float xx = -3; xx <= 3; xx += 0.5) {
                                    for(float yy = -3; yy <= 3; yy += 0.5) {
                                        if(abs(xx) + abs(yy) == 6) continue;
                                        // rotate point

                                        float tx = x + xx - cur->body->GetPosition().x;
                                        float ty = y + yy - cur->body->GetPosition().y;

                                        int ntx = (int)(tx * c - ty * s);
                                        int nty = (int)(tx * s + ty * c);

                                        if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                                            if(((PIXEL(cur->surface, ntx, nty) >> 24) & 0xff) != 0x00) {
                                                connect = true;
                                            }
                                        }
                                    }
                                }
                            }

                            if(connect) {
                                if(world->player) {
                                    world->player->setItemInHand(Item::makeItem(ItemFlags::RIGIDBODY, cur), world);

                                    world->b2world->DestroyBody(cur->body);
                                    world->rigidBodies.erase(std::remove(world->rigidBodies.begin(), world->rigidBodies.end(), cur), world->rigidBodies.end());

                                    swapped = true;
                                }
                                break;
                            }

                        }

                        if(!swapped) {
                            if(world->player) world->player->setItemInHand(NULL, world);
                        }
                        #pragma endregion
                    } else if(windowEvent.button.button == SDL_BUTTON_MIDDLE) {
                        Controls::mmouse = false;
                    }
                }

                if(windowEvent.type == SDL_MOUSEMOTION) {
                    mx = windowEvent.motion.x;
                    my = windowEvent.motion.y;
                }

            }
            EASY_END_BLOCK;
            #pragma endregion
        }

        EASY_BLOCK("tick");

        if(networkMode == NetworkMode::SERVER) {
            server->tick();
        } else if(networkMode == NetworkMode::CLIENT) {
            client->tick();
        }

        if(networkMode != NetworkMode::SERVER) {
            //if(Settings::tick_world)
            updateFrameEarly();
        }

        while(now - lastTick > mspt) {
            if(Settings::tick_world && networkMode != NetworkMode::CLIENT)
                tick();
            target = realTarget;
            lastTick = now;
            tickTime++;
        }

        if(networkMode != NetworkMode::SERVER) {
            if(Settings::tick_world)
                updateFrameLate();
        }
        EASY_END_BLOCK;

        if(networkMode != NetworkMode::SERVER) {
            // render
            #pragma region
            EASY_BLOCK("render");
            target = realTarget;
            EASY_BLOCK("GPU_Clear", GPU_PROFILER_COLOR);
            GPU_Clear(target);
            EASY_END_BLOCK;

            renderEarly();
            target = realTarget;

            renderLate();
            target = realTarget;

            // render ImGui

            EASY_BLOCK("render ImGui", UI_PROFILER_COLOR);

            GPU_ActivateShaderProgram(0, NULL);
            GPU_FlushBlitBuffer();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            DebugUI::Draw(this);
            DebugDrawUI::Draw(this);
            DebugCheatsUI::Draw(this);
            MainMenuUI::Draw(this);
            IngameUI::Draw(this);
            //  ImGui::ShowDemoWindow();

            if(DebugUI::visible) {
                ImGui::Begin("Debug Info");
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

                GPU_Renderer* renderer = GPU_GetCurrentRenderer();
                GPU_RendererID id = renderer->id;

                ImGui::Text("Using renderer: %s (%d.%d)\n", id.name, id.major_version, id.minor_version);
                ImGui::Text("  Shader versions supported: %d to %d\n\n", renderer->min_shader_version, renderer->max_shader_version);

                ImGui::End();
            }

            if(Settings::draw_material_info && !ImGui::GetIO().WantCaptureMouse) {
                EASY_BLOCK("draw material info", RENDER_PROFILER_COLOR);
                int msx = (int)((mx - ofsX - camX) / scale);
                int msy = (int)((my - ofsY - camY) / scale);

                MaterialInstance tile;

                if(msx >= 0 && msy >= 0 && msx < world->width && msy < world->height) {
                    tile = world->tiles[msx + msy * world->width];
                    //Drawing::drawText(target, tile.mat->name.c_str(), font16, mx + 14, my, 0xff, 0xff, 0xff, ALIGN_LEFT);

                    if(tile.mat->id == Materials::GENERIC_AIR.id) {
                        vector<RigidBody*> rbs = world->rigidBodies;

                        for(size_t i = 0; i < rbs.size(); i++) {
                            RigidBody* cur = rbs[i];
                            if(cur->body->IsEnabled()) {
                                float s = sin(-cur->body->GetAngle());
                                float c = cos(-cur->body->GetAngle());
                                bool upd = false;

                                float tx = msx - cur->body->GetPosition().x;
                                float ty = msy - cur->body->GetPosition().y;

                                int ntx = (int)(tx * c - ty * s);
                                int nty = (int)(tx * s + ty * c);

                                if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                                    tile = cur->tiles[ntx + nty * cur->matWidth];
                                }
                            }
                        }
                    }

                    if(tile.mat->id != Materials::GENERIC_AIR.id) {

                        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.11f, 0.11f, 0.11f, 0.4f));
                        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.00f, 1.00f, 1.00f, 0.2f));
                        ImGui::BeginTooltip();
                        ImGui::Text("%s", tile.mat->name.c_str());

                        if(Settings::draw_detailed_material_info) {

                            if(tile.mat->physicsType == PhysicsType::SOUP) {
                                ImGui::Text("fluidAmount = %f", tile.fluidAmount);
                            }

                            int ln = 0;
                            if(tile.mat->interact) {
                                for(size_t i = 0; i < Materials::MATERIALS.size(); i++) {
                                    if(tile.mat->nInteractions[i] > 0) {
                                        char buff2[40];
                                        snprintf(buff2, sizeof(buff2), "    %s", Materials::MATERIALS[i]->name.c_str());
                                        //Drawing::drawText(target, buff2, font16, mx + 14, my + 14 * ++ln, 0xff, 0xff, 0xff, ALIGN_LEFT);
                                        ImGui::Text("%s", buff2);

                                        for(int j = 0; j < tile.mat->nInteractions[i]; j++) {
                                            MaterialInteraction inter = tile.mat->interactions[i][j];
                                            char buff1[40];
                                            if(inter.type == INTERACT_TRANSFORM_MATERIAL) {
                                                snprintf(buff1, sizeof(buff1), "        %s %s r=%d x=%d y=%d", "TRANSFORM", Materials::MATERIALS[inter.data1]->name.c_str(), inter.data2, inter.ofsX, inter.ofsY);
                                            } else if(inter.type == INTERACT_SPAWN_MATERIAL) {
                                                snprintf(buff1, sizeof(buff1), "        %s %s r=%d x=%d y=%d", "SPAWN", Materials::MATERIALS[inter.data1]->name.c_str(), inter.data2, inter.ofsX, inter.ofsY);
                                            }
                                            //Drawing::drawText(target, buff1, font16, mx + 14, my + 14 * ++ln, 0xff, 0xff, 0xff, ALIGN_LEFT);
                                            ImGui::Text("%s", buff1);
                                        }
                                    }
                                }
                            }
                        }

                        ImGui::EndTooltip();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                    }
                }

                EASY_END_BLOCK; // draw material info
            }

            EASY_BLOCK("ImGui::Render()", UI_PROFILER_COLOR);
            ImGui::Render();
            EASY_END_BLOCK;
            SDL_GL_MakeCurrent(window, realTarget->context->context);
            EASY_BLOCK("ImGui_ImplOpenGL3_RenderDrawData", UI_PROFILER_COLOR);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            EASY_END_BLOCK;

            EASY_END_BLOCK;

            // render fade in/out

            EASY_BLOCK("fades");
            if(fadeInWaitFrames > 0) {
                fadeInWaitFrames--;
                fadeInStart = now;
                GPU_RectangleFilled(target, 0, 0, WIDTH, HEIGHT, {0, 0, 0, 255});
            } else if(fadeInStart > 0 && fadeInLength > 0) {
                EASY_BLOCK("fadeIn");
                float thru = 1 - (float)(now - fadeInStart) / fadeInLength;

                if(thru >= 0 && thru <= 1) {
                    GPU_RectangleFilled(target, 0, 0, WIDTH, HEIGHT, {0, 0, 0, (uint8)(thru * 255)});
                } else {
                    fadeInStart = 0;
                    fadeInLength = 0;
                }
                EASY_END_BLOCK;
            }


            if(fadeOutWaitFrames > 0) {
                fadeOutWaitFrames--;
                fadeOutStart = now;
            } else if(fadeOutStart > 0 && fadeOutLength > 0) {
                EASY_BLOCK("fadeIn");
                float thru = (float)(now - fadeOutStart) / fadeOutLength;

                if(thru >= 0 && thru <= 1) {
                    GPU_RectangleFilled(target, 0, 0, WIDTH, HEIGHT, {0, 0, 0, (uint8)(thru * 255)});
                } else {
                    GPU_RectangleFilled(target, 0, 0, WIDTH, HEIGHT, {0, 0, 0, 255});
                    fadeOutStart = 0;
                    fadeOutLength = 0;
                    fadeOutCallback();
                }
                EASY_END_BLOCK;
            }
            EASY_END_BLOCK;

            EASY_BLOCK("GPU_Flip", GPU_PROFILER_COLOR);
            GPU_Flip(target);
            EASY_END_BLOCK;
        }

        if(Time::millis() - now < 2) {
            #ifdef _WIN32
            Sleep(2 - (Time::millis() - now));
            #else
            sleep((2 - (Time::millis() - now))/1000.0f);
            #endif
        }

        EASY_BLOCK("frameCounter");
        frames++;
        if(now - lastFPS >= 1000) {
            lastFPS = now;
            logInfo("{0:d} FPS", frames);
            if(networkMode == NetworkMode::SERVER) {
                logDebug("{0:d} peers connected.", server->server->connectedPeers);
            }
            fps = frames;
            dt_fps.w = -1;
            frames = 0;

            // calculate "feels like" fps

            float sum = 0;
            float num = 0.01;

            for(int i = 0; i < frameTimeNum; i++) {
                float weight = frameTime[i];
                sum += weight * frameTime[i];
                num += weight;
            }

            feelsLikeFps = 1000 / (sum / num);

            dt_feelsLikeFps.w = -1;
        }

        for(int i = 1; i < frameTimeNum; i++) {
            frameTime[i - 1] = frameTime[i];
        }
        frameTime[frameTimeNum - 1] = (uint16_t)(Time::millis() - now);
        EASY_END_BLOCK; // frameCounter

        lastTime = now;

        EASY_END_BLOCK; // render
        #pragma endregion
    }

exit:
    EASY_END_BLOCK; // frame??

    logInfo("Shutting down...");

    std::vector<std::future<void>> results = {};

    for(auto& p : world->chunkCache) {
        if(p.first == INT_MIN) continue;
        for(auto& p2 : p.second) {
            if(p2.first == INT_MIN) continue;

            results.push_back(updateDirtyPool->push([&](int id) {
                Chunk* m = p2.second;
                world->unloadChunk(m);
            }));
        }
    }

    EASY_BLOCK("wait for threads", THREAD_WAIT_PROFILER_COLOR);
    for(int i = 0; i < results.size(); i++) {
        results[i].get();
    }
    EASY_END_BLOCK;

    // release resources & shutdown
    #pragma region
    delete objectDelete;

    running = false;

    if(networkMode != NetworkMode::SERVER) {
        TTF_Quit();

        SDL_DestroyWindow(window);
        SDL_Quit();

        audioEngine.Shutdown();

        #if BUILD_WITH_STEAM
        SteamAPI_Shutdown();
        #endif
    }
    #pragma endregion

    if(profiler::isEnabled()) {
        profiler::stopListen();

        if(clArgs->getBool("profiler-dump")) {
            logDebug("Dumping profile data");

            // dump profiler data
            time_t rawtime;
            struct tm* timeinfo;
            char buf[80];

            time(&rawtime);
            timeinfo = localtime(&rawtime);

            strftime(buf, sizeof(buf), gameDir.getPath("profiler/%Y-%m-%d_%H-%M-%S.prof").c_str(), timeinfo);
            std::string str(buf);
            logDebug("{}", str.c_str());

            profiler::dumpBlocksToFile(str.c_str());
            struct stat buffer;
            if(stat(str.c_str(), &buffer) == 0) {

                std::ifstream src(str.c_str(), std::ios::binary);
                std::ofstream dst(gameDir.getPath("profiler/latest.prof").c_str(), std::ios::binary);

                dst << src.rdbuf();
            }
        }
    }
    
    return EXIT_SUCCESS;
}

void Game::updateFrameEarly() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    // handle controls
    #pragma region
    EASY_BLOCK("controls");
    if(Controls::DEBUG_UI->get()) {
        DebugUI::visible ^= true;
        DebugDrawUI::visible ^= true;
        DebugCheatsUI::visible ^= true;
    }

    if(Settings::draw_frame_graph) {
        if(Controls::STATS_DISPLAY->get()) {
            Settings::draw_frame_graph = false;
            Settings::draw_debug_stats = false;
            Settings::draw_chunk_state = false;
            Settings::draw_detailed_material_info = false;
        }
    } else {
        if(Controls::STATS_DISPLAY->get()) {
            Settings::draw_frame_graph = true;
            Settings::draw_debug_stats = true;

            if(Controls::STATS_DISPLAY_DETAILED->get()) {
                Settings::draw_chunk_state = true;
                Settings::draw_detailed_material_info = true;
            }
        }
    }

    

    if(Controls::DEBUG_REFRESH->get()) {
        for(int x = 0; x < world->width; x++) {
            for(int y = 0; y < world->height; y++) {
                world->dirty[x + y * world->width] = true;
                world->layer2Dirty[x + y * world->width] = true;
                world->backgroundDirty[x + y * world->width] = true;
            }
        }
    }

    if(Controls::DEBUG_RIGID->get()) {
        for(auto& cur : world->rigidBodies) {
            if(cur->body->IsEnabled()) {
                float s = sin(cur->body->GetAngle());
                float c = cos(cur->body->GetAngle());
                bool upd = false;

                for(int xx = 0; xx < cur->matWidth; xx++) {
                    for(int yy = 0; yy < cur->matHeight; yy++) {
                        int tx = xx * c - yy * s + cur->body->GetPosition().x;
                        int ty = xx * s + yy * c + cur->body->GetPosition().y;

                        MaterialInstance tt = cur->tiles[xx + yy * cur->matWidth];
                        if(tt.mat->id != Materials::GENERIC_AIR.id) {
                            if(world->tiles[tx + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
                                world->tiles[tx + ty * world->width] = tt;
                                world->dirty[tx + ty * world->width] = true;
                            } else if(world->tiles[(tx + 1) + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
                                world->tiles[(tx + 1) + ty * world->width] = tt;
                                world->dirty[(tx + 1) + ty * world->width] = true;
                            } else if(world->tiles[(tx - 1) + ty * world->width].mat->id == Materials::GENERIC_AIR.id) {
                                world->tiles[(tx - 1) + ty * world->width] = tt;
                                world->dirty[(tx - 1) + ty * world->width] = true;
                            } else if(world->tiles[tx + (ty + 1) * world->width].mat->id == Materials::GENERIC_AIR.id) {
                                world->tiles[tx + (ty + 1) * world->width] = tt;
                                world->dirty[tx + (ty + 1) * world->width] = true;
                            } else if(world->tiles[tx + (ty - 1) * world->width].mat->id == Materials::GENERIC_AIR.id) {
                                world->tiles[tx + (ty - 1) * world->width] = tt;
                                world->dirty[tx + (ty - 1) * world->width] = true;
                            } else {
                                world->tiles[tx + ty * world->width] = Tiles::createObsidian(tx, ty);
                                world->dirty[tx + ty * world->width] = true;
                            }
                        }
                    }
                }

                if(upd) {
                    GPU_FreeImage(cur->texture);
                    cur->texture = GPU_CopyImageFromSurface(cur->surface);
                    GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
                    //world->updateRigidBodyHitbox(cur);
                    cur->needsUpdate = true;
                }
            }

            world->b2world->DestroyBody(cur->body);
        }
        world->rigidBodies.clear();
    }

    if(Controls::DEBUG_UPDATE_WORLD_MESH->get()) {
        world->updateWorldMesh();
    }

    if(Controls::DEBUG_EXPLODE->get()) {
        int x = (int)((mx - ofsX - camX) / scale);
        int y = (int)((my - ofsY - camY) / scale);
        world->explosion(x, y, 30);
    }

    if(Controls::DEBUG_CARVE->get()) {
        // carve square
        #pragma region
        int x = (int)((mx - ofsX - camX) / scale - 16);
        int y = (int)((my - ofsY - camY) / scale - 16);

        SDL_Surface* tex = SDL_CreateRGBSurfaceWithFormat(0, 32, 32, 32, SDL_PIXELFORMAT_ARGB8888);

        int n = 0;
        for(int xx = 0; xx < 32; xx++) {
            for(int yy = 0; yy < 32; yy++) {

                if(world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SOLID) {
                    PIXEL(tex, xx, yy) = world->tiles[(x + xx) + (y + yy) * world->width].color;
                    world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
                    world->dirty[(x + xx) + (y + yy) * world->width] = true;
                    n++;
                }
            }
        }
        if(n > 0) {
            b2PolygonShape s;
            s.SetAsBox(1, 1);
            RigidBody* rb = world->makeRigidBody(b2_dynamicBody, (float)x, (float)y, 0, s, 1, (float)0.3, tex);
            for(int tx = 0; tx < tex->w; tx++) {
                b2Filter bf = {};
                bf.categoryBits = 0x0002;
                bf.maskBits = 0x0001;
                rb->body->GetFixtureList()[0].SetFilterData(bf);
            }
            world->rigidBodies.push_back(rb);
            world->updateRigidBodyHitbox(rb);

            world->updateWorldMesh();
        }
        #pragma endregion
    }

    if(Controls::DEBUG_BRUSHSIZE_INC->get()) {
        DebugDrawUI::brushSize = DebugDrawUI::brushSize < 50 ? DebugDrawUI::brushSize + 1 : DebugDrawUI::brushSize;
    }

    if(Controls::DEBUG_BRUSHSIZE_DEC->get()) {
        DebugDrawUI::brushSize = DebugDrawUI::brushSize > 1 ? DebugDrawUI::brushSize - 1 : DebugDrawUI::brushSize;
    }

    if(Controls::DEBUG_TOGGLE_PLAYER->get()) {
        if(world->player) {
            freeCamX = world->player->x + world->player->hw / 2.0f;
            freeCamY = world->player->y - world->player->hh / 2.0f;
            world->entities.erase(std::remove(world->entities.begin(), world->entities.end(), world->player), world->entities.end());
            world->b2world->DestroyBody(world->player->rb->body);
            delete world->player;
            world->player = nullptr;
        } else {
            Player* e = new Player();
            e->x = -world->loadZone.x + world->tickZone.x + world->tickZone.w/2.0f;
            e->y = -world->loadZone.y + world->tickZone.y + world->tickZone.h/2.0f;
            e->vx = 0;
            e->vy = 0;
            e->hw = 10;
            e->hh = 20;
            b2PolygonShape sh;
            sh.SetAsBox(e->hw / 2.0f + 1, e->hh / 2.0f);
            e->rb = world->makeRigidBody(b2BodyType::b2_kinematicBody, e->x + e->hw / 2.0f - 0.5, e->y + e->hh / 2.0f - 0.5, 0, sh, 1, 1, NULL);
            e->rb->body->SetGravityScale(0);
            e->rb->body->SetLinearDamping(0);
            e->rb->body->SetAngularDamping(0);

            Item* i3 = new Item();
            i3->setFlag(ItemFlags::VACUUM);
            i3->vacuumParticles = {};
            i3->surface = Textures::loadTexture("assets/objects/testVacuum.png");
            i3->texture = GPU_CopyImageFromSurface(i3->surface);
            GPU_SetImageFilter(i3->texture, GPU_FILTER_NEAREST);
            i3->pivotX = 6;
            e->setItemInHand(i3, world);

            b2Filter bf = {};
            bf.categoryBits = 0x0001;
            //bf.maskBits = 0x0000;
            e->rb->body->GetFixtureList()[0].SetFilterData(bf);

            world->entities.push_back(e);
            world->player = e;

            /*accLoadX = 0;
            accLoadY = 0;*/
        }
    }

    if(Controls::PAUSE->get()) {
        if(this->state == GameState::INGAME) {
            IngameUI::visible = !IngameUI::visible;
        }
    }

    EASY_END_BLOCK;
    #pragma endregion

    EASY_BLOCK("audioEngine.Update");
    audioEngine.Update();
    EASY_END_BLOCK;

    if(state == LOADING) {

    } else {
        audioEngine.SetEventParameter("event:/World/Sand", "Sand", 0);
        if(world->player && world->player->heldItem != NULL && world->player->heldItem->getFlag(ItemFlags::FLUID_CONTAINER)) {
            if(Controls::lmouse && world->player->heldItem->carry.size() > 0) {
                // shoot fluid from container
                #pragma region
                int x = (int)(world->player->x + world->player->hw / 2.0f + world->loadZone.x + 10 * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f));
                int y = (int)(world->player->y + world->player->hh / 2.0f + world->loadZone.y + 10 * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f));

                MaterialInstance mat = world->player->heldItem->carry[world->player->heldItem->carry.size() - 1];
                world->player->heldItem->carry.pop_back();
                world->addParticle(new Particle(mat, (float)x, (float)y, (float)(world->player->vx / 2 + (rand() % 10 - 5) / 10.0f + 1.5f * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f)), (float)(world->player->vy / 2 + -(rand() % 5 + 5) / 10.0f + 1.5f * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f)), 0, (float)0.1));

                int i = (int)world->player->heldItem->carry.size();
                i = (int)((i / (float)world->player->heldItem->capacity) * world->player->heldItem->fill.size());
                UInt16Point pt = world->player->heldItem->fill[i];
                PIXEL(world->player->heldItem->surface, pt.x, pt.y) = 0x00;

                world->player->heldItem->texture = GPU_CopyImageFromSurface(world->player->heldItem->surface);
                GPU_SetImageFilter(world->player->heldItem->texture, GPU_FILTER_NEAREST);

                audioEngine.SetEventParameter("event:/World/Sand", "Sand", 1);
                #pragma endregion
            } else {
                // pick up fluid into container
                #pragma region
                float breakSize = world->player->heldItem->breakSize;

                int x = (int)(world->player->x + world->player->hw / 2.0f + world->loadZone.x + 10 * (float)cos((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);
                int y = (int)(world->player->y + world->player->hh / 2.0f + world->loadZone.y + 10 * (float)sin((world->player->holdAngle + 180) * 3.1415f / 180.0f) - breakSize / 2);

                int n = 0;
                for(int xx = 0; xx < breakSize; xx++) {
                    for(int yy = 0; yy < breakSize; yy++) {
                        if(world->player->heldItem->capacity == 0 || (world->player->heldItem->carry.size() < world->player->heldItem->capacity)) {
                            float cx = (float)((xx / breakSize) - 0.5);
                            float cy = (float)((yy / breakSize) - 0.5);

                            if(cx * cx + cy * cy > 0.25f) continue;

                            if(world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[(x + xx) + (y + yy) * world->width].mat->physicsType == PhysicsType::SOUP) {
                                world->player->heldItem->carry.push_back(world->tiles[(x + xx) + (y + yy) * world->width]);

                                int i = (int)world->player->heldItem->carry.size() - 1;
                                i = (int)((i / (float)world->player->heldItem->capacity) * world->player->heldItem->fill.size());
                                UInt16Point pt = world->player->heldItem->fill[i];
                                Uint32 c = world->tiles[(x + xx) + (y + yy) * world->width].color;
                                PIXEL(world->player->heldItem->surface, pt.x, pt.y) = (world->tiles[(x + xx) + (y + yy) * world->width].mat->alpha << 24) + c;

                                world->player->heldItem->texture = GPU_CopyImageFromSurface(world->player->heldItem->surface);
                                GPU_SetImageFilter(world->player->heldItem->texture, GPU_FILTER_NEAREST);

                                world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
                                world->dirty[(x + xx) + (y + yy) * world->width] = true;
                                n++;
                            }
                        }
                    }
                }

                if(n > 0) {
                    audioEngine.PlayEvent("event:/Player/Impact");
                }
                #pragma endregion
            }
        }

        // rigidbody hover
        #pragma region

        int x = (int)((mx - ofsX - camX) / scale);
        int y = (int)((my - ofsY - camY) / scale);

        bool swapped = false;
        float hoverDelta = 10.0 * deltaTime / 1000.0;

        // this copies the vector
        vector<RigidBody*> rbs = world->rigidBodies;
        for(size_t i = 0; i < rbs.size(); i++) {
            RigidBody* cur = rbs[i];

            if(swapped) {
                cur->hover = (float)std::fmax(0, cur->hover - hoverDelta);
                continue;
            }

            bool connect = false;
            if(cur->body->IsEnabled()) {
                float s = sin(-cur->body->GetAngle());
                float c = cos(-cur->body->GetAngle());
                bool upd = false;
                for(float xx = -3; xx <= 3; xx += 0.5) {
                    for(float yy = -3; yy <= 3; yy += 0.5) {
                        if(abs(xx) + abs(yy) == 6) continue;
                        // rotate point

                        float tx = x + xx - cur->body->GetPosition().x;
                        float ty = y + yy - cur->body->GetPosition().y;

                        int ntx = (int)(tx * c - ty * s);
                        int nty = (int)(tx * s + ty * c);

                        if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                            if(((PIXEL(cur->surface, ntx, nty) >> 24) & 0xff) != 0x00) {
                                connect = true;
                            }
                        }
                    }
                }
            }

            if(connect) {
                swapped = true;
                cur->hover = (float)std::fmin(1, cur->hover + hoverDelta);
            } else {
                cur->hover = (float)std::fmax(0, cur->hover - hoverDelta);
            }
        }
        #pragma endregion

        // update world->tickZone
        #pragma region
        world->tickZone = {CHUNK_W, CHUNK_H, world->width - CHUNK_W * 2, world->height - CHUNK_H * 2};
        if(world->tickZone.x + world->tickZone.w > world->width) {
            world->tickZone.x = world->width - world->tickZone.w;
        }

        if(world->tickZone.y + world->tickZone.h > world->height) {
            world->tickZone.y = world->height - world->tickZone.h;
        }
        #pragma endregion

    }
}

void Game::tick() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    //logDebug("{0:d} {0:d}", accLoadX, accLoadY);
    if(state == LOADING) {
        if(world) {
            // tick chunkloading
            world->frame();
            if(world->readyToMerge.size() == 0 && fadeOutStart == 0) {
                fadeOutStart = now;
                fadeOutLength = 250;
                fadeOutCallback = [&]() {
                    fadeInStart = now;
                    fadeInLength = 500;
                    fadeInWaitFrames = 4;
                    state = stateAfterLoad;
                };

                setWindowProgress(WindowProgressState::NONE, 0.0f);
                setWindowFlash(WindowFlashAction::START_COUNT, 1, 333);

                EASY_EVENT("world done loading", profiler::colors::Magenta);
            }
        }
    } else {

        int lastReadyToMergeSize = (int)world->readyToMerge.size();

        // check chunk loading
        tickChunkLoading();

        if(world->needToTickGeneration) world->tickChunkGeneration();

        // clear objects
        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
        GPU_Clear(textureObjects->target);

        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
        GPU_Clear(textureObjectsLQ->target);

        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
        GPU_Clear(textureObjectsBack->target);

        if(Settings::tick_world && world->readyToMerge.size() == 0) {
            world->tickChunks();
        }

        // render objects
        #pragma region
        EASY_BLOCK("memset objectDelete");
        memset(objectDelete, false, (size_t)world->width * world->height * sizeof(bool));
        EASY_END_BLOCK;

        EASY_BLOCK("render rigidbodies");
        GPU_SetBlendMode(textureObjects, GPU_BLEND_NORMAL);
        GPU_SetBlendMode(textureObjectsLQ, GPU_BLEND_NORMAL);
        GPU_SetBlendMode(textureObjectsBack, GPU_BLEND_NORMAL);

        for(size_t i = 0; i < world->rigidBodies.size(); i++) {
            RigidBody* cur = world->rigidBodies[i];
            if(cur == nullptr) continue;
            if(cur->surface == nullptr) continue;
            if(!cur->body->IsEnabled()) continue;

            float x = cur->body->GetPosition().x;
            float y = cur->body->GetPosition().y;

            // draw
            #pragma region
            GPU_Target* tgt = cur->back ? textureObjectsBack->target : textureObjects->target;
            GPU_Target* tgtLQ = cur->back ? textureObjectsBack->target : textureObjectsLQ->target;
            int scaleObjTex = Settings::hd_objects ? Settings::hd_objects_size : 1;

            GPU_Rect r = {x * scaleObjTex, y * scaleObjTex, (float)cur->surface->w * scaleObjTex, (float)cur->surface->h * scaleObjTex};

            if(cur->texNeedsUpdate) {
                if(cur->texture != nullptr) {
                    GPU_FreeImage(cur->texture);
                }
                cur->texture = GPU_CopyImageFromSurface(cur->surface);
                GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
                cur->texNeedsUpdate = false;
            }

            GPU_BlitRectX(cur->texture, NULL, tgt, &r, cur->body->GetAngle() * 180 / (float)W_PI, 0, 0, GPU_FLIP_NONE);
            #pragma endregion

            // draw outline
            #pragma region
            Uint8 outlineAlpha = (Uint8)(cur->hover * 255);
            if(outlineAlpha > 0) {
                SDL_Color col = {0xff, 0xff, 0x80, outlineAlpha};
                GPU_SetShapeBlendMode(GPU_BLEND_NORMAL_FACTOR_ALPHA); // SDL_BLENDMODE_BLEND
                for(auto& l : cur->outline) {
                    b2Vec2* vec = new b2Vec2[l.GetNumPoints()];
                    for(int j = 0; j < l.GetNumPoints(); j++) {
                        vec[j] = {(float)l.GetPoint(j).x / scale, (float)l.GetPoint(j).y / scale};
                    }
                    Drawing::drawPolygon(tgtLQ, col, vec, (int)x, (int)y, scale, l.GetNumPoints(), cur->body->GetAngle(), 0, 0);
                    delete[] vec;
                }
                GPU_SetShapeBlendMode(GPU_BLEND_NORMAL); // SDL_BLENDMODE_NONE
            }
            #pragma endregion

            // displace fluids
            #pragma region
            float s = sin(cur->body->GetAngle());
            float c = cos(cur->body->GetAngle());

            std::vector<std::pair<int, int>> checkDirs = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

            for(int tx = 0; tx < cur->matWidth; tx++) {
                for(int ty = 0; ty < cur->matHeight; ty++) {
                    MaterialInstance rmat = cur->tiles[tx + ty * cur->matWidth];
                    if(rmat.mat->id == Materials::GENERIC_AIR.id) continue;

                    // rotate point
                    int wx = (int)(tx * c - (ty + 1) * s + x);
                    int wy = (int)(tx * s + (ty + 1) * c + y);

                    for(auto& dir : checkDirs) {
                        int wxd = wx + dir.first;
                        int wyd = wy + dir.second;

                        if(wxd < 0 || wyd < 0 || wxd >= world->width || wyd >= world->height) continue;
                        if(world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::AIR) {
                            world->tiles[wxd + wyd * world->width] = rmat;
                            world->dirty[wxd + wyd * world->width] = true;
                            //objectDelete[wxd + wyd * world->width] = true;
                            break;
                        } else if(world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SAND) {
                            world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
                            world->tiles[wxd + wyd * world->width] = rmat;
                            //objectDelete[wxd + wyd * world->width] = true;
                            world->dirty[wxd + wyd * world->width] = true;
                            cur->body->SetLinearVelocity({cur->body->GetLinearVelocity().x * (float)0.99, cur->body->GetLinearVelocity().y * (float)0.99});
                            cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.98);
                            break;
                        } else if(world->tiles[wxd + wyd * world->width].mat->physicsType == PhysicsType::SOUP) {
                            world->addParticle(new Particle(world->tiles[wxd + wyd * world->width], (float)wxd, (float)(wyd - 3), (float)((rand() % 10 - 5) / 10.0f), (float)(-(rand() % 5 + 5) / 10.0f), 0, (float)0.1));
                            world->tiles[wxd + wyd * world->width] = rmat;
                            //objectDelete[wxd + wyd * world->width] = true;
                            world->dirty[wxd + wyd * world->width] = true;
                            cur->body->SetLinearVelocity({cur->body->GetLinearVelocity().x * (float)0.998, cur->body->GetLinearVelocity().y * (float)0.998});
                            cur->body->SetAngularVelocity(cur->body->GetAngularVelocity() * (float)0.99);
                            break;
                        }
                    }
                }
            }
            #pragma endregion
        }
        EASY_END_BLOCK;
        #pragma endregion

        // render entities
        #pragma region
        EASY_BLOCK("render entities");
        if(lastReadyToMergeSize == 0) {
            world->tickEntities(textureEntities->target);

            if(world->player) {
                if(world->player->holdHammer) {
                    int x = (int)((mx - ofsX - camX) / scale);
                    int y = (int)((my - ofsY - camY) / scale);
                    GPU_Line(textureEntitiesLQ->target, x, y, world->player->hammerX, world->player->hammerY, {0xff, 0xff, 0x00, 0xff});
                }
            }
        }
        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL); // SDL_BLENDMODE_NONE
        EASY_END_BLOCK;
        #pragma endregion

        // entity fluid displacement & make solid
        #pragma region
        for(size_t i = 0; i < world->entities.size(); i++) {
            Entity cur = *world->entities[i];

            for(int tx = 0; tx < cur.hw; tx++) {
                for(int ty = 0; ty < cur.hh; ty++) {

                    int wx = (int)(tx + cur.x + world->loadZone.x);
                    int wy = (int)(ty + cur.y + world->loadZone.y);
                    if(wx < 0 || wy < 0 || wx >= world->width || wy >= world->height) continue;
                    if(world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::AIR) {
                        world->tiles[wx + wy * world->width] = Tiles::OBJECT;
                        objectDelete[wx + wy * world->width] = true;
                    } else if(world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[wx + wy * world->width].mat->physicsType == PhysicsType::SOUP) {
                        world->addParticle(new Particle(world->tiles[wx + wy * world->width], (float)(wx + rand() % 3 - 1 - cur.vx), (float)(wy - abs(cur.vy)), (float)(-cur.vx / 4 + (rand() % 10 - 5) / 5.0f), (float)(-cur.vy / 4 + -(rand() % 5 + 5) / 5.0f), 0, (float)0.1));
                        world->tiles[wx + wy * world->width] = Tiles::OBJECT;
                        objectDelete[wx + wy * world->width] = true;
                        world->dirty[wx + wy * world->width] = true;
                    }
                }
            }
        }
        #pragma endregion

        if(Settings::tick_world && world->readyToMerge.size() == 0) {
            world->tick();
        }

        if(Controls::DEBUG_TICK->get()) {
            world->tick();
        }

        // player movement
        tickPlayer();

        // update particles, tickObjects, update dirty
        // TODO: this is not entirely thread safe since tickParticles changes World::tiles and World::dirty
        #pragma region
        EASY_BLOCK("post World::tick");
        bool hadDirty = false;
        bool hadLayer2Dirty = false;
        bool hadBackgroundDirty = false;
        bool hadFire = false;
        bool hadFlow = false;

        int pitch;
        //void* vdpixels_ar = texture->data;
        //unsigned char* dpixels_ar = (unsigned char*)vdpixels_ar;
        unsigned char* dpixels_ar = pixels_ar;
        unsigned char* dpixelsFire_ar = pixelsFire_ar;
        unsigned char* dpixelsFlow_ar = pixelsFlow_ar;
        unsigned char* dpixelsEmission_ar = pixelsEmission_ar;

        std::vector<std::future<void>> results = {};

        results.push_back(updateDirtyPool->push([&](int id) {
            EASY_BLOCK("particles");
            //SDL_SetRenderTarget(renderer, textureParticles);
            void* particlePixels = pixelsParticles_ar;
            EASY_BLOCK("memset");
            memset(particlePixels, 0, (size_t)world->width * world->height * 4);
            EASY_END_BLOCK; // memset
            world->renderParticles((unsigned char**)&particlePixels);
            world->tickParticles();

            //SDL_SetRenderTarget(renderer, NULL);
            EASY_END_BLOCK; // particles
        }));

        if(world->readyToMerge.size() == 0) {
            results.push_back(updateDirtyPool->push([&](int id) {
                world->tickObjectBounds();
            }));
        }

        EASY_BLOCK("wait for threads", THREAD_WAIT_PROFILER_COLOR);
        for(int i = 0; i < results.size(); i++) {
            results[i].get();
        }
        EASY_END_BLOCK;

        for(size_t i = 0; i < world->rigidBodies.size(); i++) {
            RigidBody* cur = world->rigidBodies[i];
            if(cur == nullptr) continue;
            if(cur->surface == nullptr) continue;
            if(!cur->body->IsEnabled()) continue;

            float x = cur->body->GetPosition().x;
            float y = cur->body->GetPosition().y;

            float s = sin(cur->body->GetAngle());
            float c = cos(cur->body->GetAngle());

            std::vector<std::pair<int, int>> checkDirs = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};
            for(int tx = 0; tx < cur->matWidth; tx++) {
                for(int ty = 0; ty < cur->matHeight; ty++) {
                    MaterialInstance rmat = cur->tiles[tx + ty * cur->matWidth];
                    if(rmat.mat->id == Materials::GENERIC_AIR.id) continue;

                    // rotate point
                    int wx = (int)(tx * c - (ty + 1) * s + x);
                    int wy = (int)(tx * s + (ty + 1) * c + y);

                    bool found = false;
                    for(auto& dir : checkDirs) {
                        int wxd = wx + dir.first;
                        int wyd = wy + dir.second;

                        if(wxd < 0 || wyd < 0 || wxd >= world->width || wyd >= world->height) continue;
                        if(world->tiles[wxd + wyd * world->width] == rmat) {
                            cur->tiles[tx + ty * cur->matWidth] = world->tiles[wxd + wyd * world->width];
                            world->tiles[wxd + wyd * world->width] = Tiles::NOTHING;
                            world->dirty[wxd + wyd * world->width] = true;
                            found = true;

                            //for(int dxx = -1; dxx <= 1; dxx++) {
                            //    for(int dyy = -1; dyy <= 1; dyy++) {
                            //        if(world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->physicsType == PhysicsType::SAND || world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->physicsType == PhysicsType::SOUP) {
                            //            uint32_t color = world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].color;

                            //            unsigned int offset = ((wxd + dxx) + (wyd + dyy) * world->width) * 4;

                            //            dpixels_ar[offset + 2] = ((color >> 0) & 0xff);        // b
                            //            dpixels_ar[offset + 1] = ((color >> 8) & 0xff);        // g
                            //            dpixels_ar[offset + 0] = ((color >> 16) & 0xff);        // r
                            //            dpixels_ar[offset + 3] = world->tiles[(wxd + dxx) + (wyd + dyy) * world->width].mat->alpha;    // a
                            //        }
                            //    }
                            //}

                            //if(!Settings::draw_load_zones) {
                            //    unsigned int offset = (wxd + wyd * world->width) * 4;
                            //    dpixels_ar[offset + 2] = 0;        // b
                            //    dpixels_ar[offset + 1] = 0;        // g
                            //    dpixels_ar[offset + 0] = 0xff;        // r
                            //    dpixels_ar[offset + 3] = 0xff;    // a
                            //}
                            	

                            break;
                        }
                    }

                    if(!found) {
                        if(world->tiles[wx + wy * world->width].mat->id == Materials::GENERIC_AIR.id) {
                            cur->tiles[tx + ty * cur->matWidth] = Tiles::NOTHING;
                        }
                    }
                }
            }

            for(int tx = 0; tx < cur->surface->w; tx++) {
                for(int ty = 0; ty < cur->surface->h; ty++) {
                    MaterialInstance mat = cur->tiles[tx + ty * cur->surface->w];
                    if(mat.mat->id == Materials::GENERIC_AIR.id) {
                        PIXEL(cur->surface, tx, ty) = 0x00000000;
                    } else {
                        PIXEL(cur->surface, tx, ty) = (mat.mat->alpha << 24) + (mat.color & 0x00ffffff);
                    }
                }
            }

            GPU_FreeImage(cur->texture);
            cur->texture = GPU_CopyImageFromSurface(cur->surface);
            GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);

            cur->needsUpdate = true;
        }

        if(world->readyToMerge.size() == 0) {
            if(Settings::tick_box2d) world->tickObjects();
        }

        if(tickTime % 10 == 0) world->tickObjectsMesh();

        for(int i = 0; i < Materials::nMaterials; i++) movingTiles[i] = 0;

        results.clear();
        results.push_back(updateDirtyPool->push([&](int id) {
            EASY_BLOCK("dirty");
            for(int i = 0; i < world->width * world->height; i++) {
                const unsigned int offset = i * 4;

                if(world->dirty[i]) {
                    hadDirty = true;
                    movingTiles[world->tiles[i].mat->id]++;
                    if(world->tiles[i].mat->physicsType == PhysicsType::AIR) {
                        dpixels_ar[offset + 0] = 0;        // b
                        dpixels_ar[offset + 1] = 0;        // g
                        dpixels_ar[offset + 2] = 0;        // r
                        dpixels_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a		

                        dpixelsFire_ar[offset + 0] = 0;        // b
                        dpixelsFire_ar[offset + 1] = 0;        // g
                        dpixelsFire_ar[offset + 2] = 0;        // r
                        dpixelsFire_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a

                        dpixelsEmission_ar[offset + 0] = 0;        // b
                        dpixelsEmission_ar[offset + 1] = 0;        // g
                        dpixelsEmission_ar[offset + 2] = 0;        // r
                        dpixelsEmission_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a

                        world->flowY[i] = 0;
                        world->flowX[i] = 0;
                    } else {
                        Uint32 color = world->tiles[i].color;
                        Uint32 emit = world->tiles[i].mat->emitColor;
                        //float br = world->light[i];
                        dpixels_ar[offset + 2] = ((color >> 0) & 0xff);        // b
                        dpixels_ar[offset + 1] = ((color >> 8) & 0xff);        // g
                        dpixels_ar[offset + 0] = ((color >> 16) & 0xff);        // r
                        dpixels_ar[offset + 3] = world->tiles[i].mat->alpha;    // a

                        dpixelsEmission_ar[offset + 2] = ((emit >> 0) & 0xff);        // b
                        dpixelsEmission_ar[offset + 1] = ((emit >> 8) & 0xff);        // g
                        dpixelsEmission_ar[offset + 0] = ((emit >> 16) & 0xff);        // r
                        dpixelsEmission_ar[offset + 3] = ((emit >> 24) & 0xff);    // a

                        if(world->tiles[i].mat->id == Materials::FIRE.id) {
                            dpixelsFire_ar[offset + 2] = ((color >> 0) & 0xff);        // b
                            dpixelsFire_ar[offset + 1] = ((color >> 8) & 0xff);        // g
                            dpixelsFire_ar[offset + 0] = ((color >> 16) & 0xff);        // r
                            dpixelsFire_ar[offset + 3] = world->tiles[i].mat->alpha;    // a
                            hadFire = true;
                        }
                        if(world->tiles[i].mat->physicsType == PhysicsType::SOUP) {

                            float newFlowX = world->prevFlowX[i] + (world->flowX[i] - world->prevFlowX[i]) * 0.25;
                            float newFlowY = world->prevFlowY[i] + (world->flowY[i] - world->prevFlowY[i]) * 0.25;
                            if(newFlowY < 0) newFlowY *= 0.5;

                            dpixelsFlow_ar[offset + 2] = 0; // b
                            dpixelsFlow_ar[offset + 1] = std::min(std::max(newFlowY * (3.0 / world->tiles[i].mat->iterations + 0.5) / 4.0 + 0.5, 0.0), 1.0) * 255; // g
                            dpixelsFlow_ar[offset + 0] = std::min(std::max(newFlowX * (3.0 / world->tiles[i].mat->iterations + 0.5) / 4.0 + 0.5, 0.0), 1.0) * 255; // r
                            dpixelsFlow_ar[offset + 3] = 0xff; // a
                            hadFlow = true;
                            world->prevFlowX[i] = newFlowX;
                            world->prevFlowY[i] = newFlowY;
                            world->flowY[i] = 0;
                            world->flowX[i] = 0;
                        } else {
                            world->flowY[i] = 0;
                            world->flowX[i] = 0;
                        }
                    }
                }
            }
            EASY_END_BLOCK;
        }));

        //void* vdpixelsLayer2_ar = textureLayer2->data;
        //unsigned char* dpixelsLayer2_ar = (unsigned char*)vdpixelsLayer2_ar;
        unsigned char* dpixelsLayer2_ar = pixelsLayer2_ar;
        results.push_back(updateDirtyPool->push([&](int id) {
            EASY_BLOCK("layer2Dirty");
            for(int i = 0; i < world->width * world->height; i++) {
                /*for (int x = 0; x < world->width; x++) {
                    for (int y = 0; y < world->height; y++) {*/
                    //const unsigned int i = x + y * world->width;
                const unsigned int offset = i * 4;
                if(world->layer2Dirty[i]) {
                    hadLayer2Dirty = true;
                    if(world->layer2[i].mat->physicsType == PhysicsType::AIR) {
                        if(Settings::draw_background_grid) {
                            Uint32 color = ((i) % 2) == 0 ? 0x888888 : 0x444444;
                            dpixelsLayer2_ar[offset + 2] = (color >> 0) & 0xff;        // b
                            dpixelsLayer2_ar[offset + 1] = (color >> 8) & 0xff;        // g
                            dpixelsLayer2_ar[offset + 0] = (color >> 16) & 0xff;       // r
                            dpixelsLayer2_ar[offset + 3] = SDL_ALPHA_OPAQUE;			 // a
                            continue;
                        } else {
                            dpixelsLayer2_ar[offset + 0] = 0;        // b
                            dpixelsLayer2_ar[offset + 1] = 0;        // g
                            dpixelsLayer2_ar[offset + 2] = 0;        // r
                            dpixelsLayer2_ar[offset + 3] = SDL_ALPHA_TRANSPARENT;    // a
                            continue;
                        }
                    }
                    Uint32 color = world->layer2[i].color;
                    dpixelsLayer2_ar[offset + 2] = (color >> 0) & 0xff;        // b
                    dpixelsLayer2_ar[offset + 1] = (color >> 8) & 0xff;        // g
                    dpixelsLayer2_ar[offset + 0] = (color >> 16) & 0xff;        // r
                    dpixelsLayer2_ar[offset + 3] = world->layer2[i].mat->alpha;    // a
                }
            }
            EASY_END_BLOCK;
        }));

        //void* vdpixelsBackground_ar = textureBackground->data;
        //unsigned char* dpixelsBackground_ar = (unsigned char*)vdpixelsBackground_ar;
        unsigned char* dpixelsBackground_ar = pixelsBackground_ar;
        results.push_back(updateDirtyPool->push([&](int id) {
            EASY_BLOCK("backgroundDirty");
            for(int i = 0; i < world->width * world->height; i++) {
                /*for (int x = 0; x < world->width; x++) {
                    for (int y = 0; y < world->height; y++) {*/
                    //const unsigned int i = x + y * world->width;
                const unsigned int offset = i * 4;

                if(world->backgroundDirty[i]) {
                    hadBackgroundDirty = true;
                    Uint32 color = world->background[i];
                    dpixelsBackground_ar[offset + 2] = (color >> 0) & 0xff;        // b
                    dpixelsBackground_ar[offset + 1] = (color >> 8) & 0xff;        // g
                    dpixelsBackground_ar[offset + 0] = (color >> 16) & 0xff;       // r
                    dpixelsBackground_ar[offset + 3] = (color >> 24) & 0xff;       // a
                }

                //}
            }
            EASY_END_BLOCK;
        }));

        EASY_BLOCK("objectDelete");
        for(int i = 0; i < world->width * world->height; i++) {
            /*for (int x = 0; x < world->width; x++) {
                for (int y = 0; y < world->height; y++) {*/
                //const unsigned int i = x + y * world->width;
            const unsigned int offset = i * 4;

            if(objectDelete[i]) {
                world->tiles[i] = Tiles::NOTHING;
            }
        }
        EASY_END_BLOCK;

        //results.push_back(updateDirtyPool->push([&](int id) {

        //}));
        EASY_BLOCK("wait for threads", THREAD_WAIT_PROFILER_COLOR);
        for(int i = 0; i < results.size(); i++) {
            results[i].get();
        }
        EASY_END_BLOCK;

        updateMaterialSounds();

        EASY_BLOCK("particle GPU_UpdateImageBytes");
        GPU_UpdateImageBytes(
            textureParticles,
            NULL,
            &pixelsParticles_ar[0],
            world->width * 4
        );
        EASY_END_BLOCK; // GPU_UpdateImageBytes

        EASY_BLOCK("dirty memset");
        if(hadDirty)		   memset(world->dirty, false, (size_t)world->width * world->height);
        if(hadLayer2Dirty)	   memset(world->layer2Dirty, false, (size_t)world->width * world->height);
        if(hadBackgroundDirty) memset(world->backgroundDirty, false, (size_t)world->width * world->height);
        EASY_END_BLOCK;

        EASY_END_BLOCK; // post World::tick
        #pragma endregion

        if(Settings::tick_temperature && tickTime % 4 == 2) {
            world->tickTemperature();
        }
        if(Settings::draw_temperature_map && tickTime % 4 == 0) {
            renderTemperatureMap(world);
        }

        EASY_BLOCK("GPU_UpdateImageBytes", GPU_PROFILER_COLOR);
        if(hadDirty) {
            GPU_UpdateImageBytes(
                texture,
                NULL,
                &pixels[0],
                world->width * 4
            );

            GPU_UpdateImageBytes(
                emissionTexture,
                NULL,
                &pixelsEmission[0],
                world->width * 4
            );
        }

        if(hadLayer2Dirty) {
            GPU_UpdateImageBytes(
                textureLayer2,
                NULL,
                &pixelsLayer2[0],
                world->width * 4
            );
        }

        if(hadBackgroundDirty) {
            GPU_UpdateImageBytes(
                textureBackground,
                NULL,
                &pixelsBackground[0],
                world->width * 4
            );
        }

        if(hadFlow) {
            GPU_UpdateImageBytes(
                textureFlow,
                NULL,
                &pixelsFlow[0],
                world->width * 4
            );

            waterFlowPassShader->dirty = true;
        }

        if(hadFire) {
            GPU_UpdateImageBytes(
                textureFire,
                NULL,
                &pixelsFire[0],
                world->width * 4
            );
        }

        if(Settings::draw_temperature_map) {
            GPU_UpdateImageBytes(
                temperatureMap,
                NULL,
                &pixelsTemp[0],
                world->width * 4
            );
        }

        /*GPU_UpdateImageBytes(
            textureObjects,
            NULL,
            &pixelsObjects[0],
            world->width * 4
        );*/
        EASY_END_BLOCK;

        if(Settings::tick_box2d && tickTime % 4 == 0) world->updateWorldMesh();

        EASY_END_BLOCK;
    }
}

void Game::tickChunkLoading() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    // if need to load chunks
    if((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
        while(world->toLoad.size() > 0) {
            // tick chunkloading
            world->frame();
        }

        //iterate
        #pragma region
        EASY_BLOCK("iterate");
        for(int i = 0; i < world->width * world->height; i++) {
            const unsigned int offset = i * 4;

            #define UCH_SET_PIXEL(pix_ar, ofs, c_r, c_g, c_b, c_a) \
				pix_ar[ofs + 0] = c_b;\
				pix_ar[ofs + 1] = c_g;\
				pix_ar[ofs + 2] = c_r;\
				pix_ar[ofs + 3] = c_a;

            if(world->dirty[i]) {
                if(world->tiles[i].mat->physicsType == PhysicsType::AIR) {
                    UCH_SET_PIXEL(pixels_ar, offset, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
                } else {
                    Uint32 color = world->tiles[i].color;
                    Uint32 emit = world->tiles[i].mat->emitColor;
                    UCH_SET_PIXEL(pixels_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, world->tiles[i].mat->alpha);
                    UCH_SET_PIXEL(pixelsEmission_ar, offset, (emit >> 0) & 0xff, (emit >> 8) & 0xff, (emit >> 16) & 0xff, (emit >> 24) & 0xff);
                }
            }

            if(world->layer2Dirty[i]) {
                if(world->layer2[i].mat->physicsType == PhysicsType::AIR) {
                    if(Settings::draw_background_grid) {
                        Uint32 color = ((i) % 2) == 0 ? 0x888888 : 0x444444;
                        UCH_SET_PIXEL(pixelsLayer2_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, SDL_ALPHA_OPAQUE);
                    } else {
                        UCH_SET_PIXEL(pixelsLayer2_ar, offset, 0, 0, 0, SDL_ALPHA_TRANSPARENT);
                    }
                    continue;
                }
                Uint32 color = world->layer2[i].color;
                UCH_SET_PIXEL(pixelsLayer2_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, world->layer2[i].mat->alpha);
            }

            if(world->backgroundDirty[i]) {
                Uint32 color = world->background[i];
                UCH_SET_PIXEL(pixelsBackground_ar, offset, (color >> 0) & 0xff, (color >> 8) & 0xff, (color >> 16) & 0xff, (color >> 24) & 0xff);
            }
            #undef UCH_SET_PIXEL
        }
        EASY_END_BLOCK;
        #pragma endregion

        EASY_BLOCK("memset");
        memset(world->dirty, false, (size_t)world->width * world->height);
        memset(world->layer2Dirty, false, (size_t)world->width * world->height);
        memset(world->backgroundDirty, false, (size_t)world->width * world->height);
        EASY_END_BLOCK;

        EASY_BLOCK("loop");
        while((abs(accLoadX) > CHUNK_W / 2 || abs(accLoadY) > CHUNK_H / 2)) {
            int subX = std::fmax(std::fmin(accLoadX, CHUNK_W / 2), -CHUNK_W / 2);
            if(abs(subX) < CHUNK_W / 2) subX = 0;
            int subY = std::fmax(std::fmin(accLoadY, CHUNK_H / 2), -CHUNK_H / 2);
            if(abs(subY) < CHUNK_H / 2) subY = 0;

            world->loadZone.x += subX;
            world->loadZone.y += subY;

            int delta = 4 * (subX + subY * world->width);
            EASY_BLOCK("rotate");
            std::vector<std::future<void>> results = {};
            if(delta > 0) {
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixels_ar[0]), &(pixels_ar[world->width * world->height * 4]) - delta, &(pixels_ar[world->width * world->height * 4]));
                    //rotate(pixels.begin(), pixels.end() - delta, pixels.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsLayer2_ar[0]), &(pixelsLayer2_ar[world->width * world->height * 4]) - delta, &(pixelsLayer2_ar[world->width * world->height * 4]));
                    //rotate(pixelsLayer2.begin(), pixelsLayer2.end() - delta, pixelsLayer2.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsBackground_ar[0]), &(pixelsBackground_ar[world->width * world->height * 4]) - delta, &(pixelsBackground_ar[world->width * world->height * 4]));
                    //rotate(pixelsBackground.begin(), pixelsBackground.end() - delta, pixelsBackground.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsFire_ar[0]), &(pixelsFire_ar[world->width * world->height * 4]) - delta, &(pixelsFire_ar[world->width * world->height * 4]));
                    //rotate(pixelsFire_ar.begin(), pixelsFire_ar.end() - delta, pixelsFire_ar.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsFlow_ar[0]), &(pixelsFlow_ar[world->width * world->height * 4]) - delta, &(pixelsFlow_ar[world->width * world->height * 4]));
                    //rotate(pixelsFlow_ar.begin(), pixelsFlow_ar.end() - delta, pixelsFlow_ar.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsEmission_ar[0]), &(pixelsEmission_ar[world->width * world->height * 4]) - delta, &(pixelsEmission_ar[world->width * world->height * 4]));
                    //rotate(pixelsEmission_ar.begin(), pixelsEmission_ar.end() - delta, pixelsEmission_ar.end());
                }));
            } else if(delta < 0) {
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixels_ar[0]), &(pixels_ar[0]) - delta, &(pixels_ar[world->width * world->height * 4]));
                    //rotate(pixels.begin(), pixels.begin() - delta, pixels.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsLayer2_ar[0]), &(pixelsLayer2_ar[0]) - delta, &(pixelsLayer2_ar[world->width * world->height * 4]));
                    //rotate(pixelsLayer2.begin(), pixelsLayer2.begin() - delta, pixelsLayer2.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsBackground_ar[0]), &(pixelsBackground_ar[0]) - delta, &(pixelsBackground_ar[world->width * world->height * 4]));
                    //rotate(pixelsBackground.begin(), pixelsBackground.begin() - delta, pixelsBackground.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsFire_ar[0]), &(pixelsFire_ar[0]) - delta, &(pixelsFire_ar[world->width * world->height * 4]));
                    //rotate(pixelsFire_ar.begin(), pixelsFire_ar.begin() - delta, pixelsFire_ar.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsFlow_ar[0]), &(pixelsFlow_ar[0]) - delta, &(pixelsFlow_ar[world->width * world->height * 4]));
                    //rotate(pixelsFlow_ar.begin(), pixelsFlow_ar.begin() - delta, pixelsFlow_ar.end());
                }));
                results.push_back(updateDirtyPool->push([&](int id) {
                    rotate(&(pixelsEmission_ar[0]), &(pixelsEmission_ar[0]) - delta, &(pixelsEmission_ar[world->width * world->height * 4]));
                    //rotate(pixelsEmission_ar.begin(), pixelsEmission_ar.begin() - delta, pixelsEmission_ar.end());
                }));
            }
            EASY_BLOCK("wait for threads", THREAD_WAIT_PROFILER_COLOR);
            for(auto& v : results) {
                v.get();
            }
            EASY_END_BLOCK; // wait for threads

            EASY_END_BLOCK; // rotate

            #define CLEARPIXEL(pixels, ofs) \
pixels[ofs + 0] = pixels[ofs + 1] = pixels[ofs + 2] = 0xff; \
pixels[ofs + 3] = SDL_ALPHA_TRANSPARENT;

            EASY_BLOCK("CLEARPIXEL X");
            for(int x = 0; x < abs(subX); x++) {
                for(int y = 0; y < world->height; y++) {
                    const unsigned int offset = (world->width * 4 * y) + x * 4;
                    if(offset < world->width * world->height * 4) {
                        CLEARPIXEL(pixels_ar, offset);
                        CLEARPIXEL(pixelsLayer2_ar, offset);
                        CLEARPIXEL(pixelsObjects_ar, offset);
                        CLEARPIXEL(pixelsBackground_ar, offset);
                        CLEARPIXEL(pixelsFire_ar, offset);
                        CLEARPIXEL(pixelsFlow_ar, offset);
                        CLEARPIXEL(pixelsEmission_ar, offset);
                    }
                }
            }
            EASY_END_BLOCK;

            EASY_BLOCK("CLEARPIXEL Y");
            for(int y = 0; y < abs(subY); y++) {
                for(int x = 0; x < world->width; x++) {
                    const unsigned int offset = (world->width * 4 * y) + x * 4;
                    if(offset < world->width * world->height * 4) {
                        CLEARPIXEL(pixels_ar, offset);
                        CLEARPIXEL(pixelsLayer2_ar, offset);
                        CLEARPIXEL(pixelsObjects_ar, offset);
                        CLEARPIXEL(pixelsBackground_ar, offset);
                        CLEARPIXEL(pixelsFire_ar, offset);
                        CLEARPIXEL(pixelsFlow_ar, offset);
                        CLEARPIXEL(pixelsEmission_ar, offset);
                    }
                }
            }
            EASY_END_BLOCK;

            #undef CLEARPIXEL

            accLoadX -= subX;
            accLoadY -= subY;

            ofsX -= subX * scale;
            ofsY -= subY * scale;
        }

        EASY_END_BLOCK;

        world->tickChunks();
        world->updateWorldMesh();
        world->dirty[0] = true;
        world->layer2Dirty[0] = true;
        world->backgroundDirty[0] = true;

    } else {
        world->frame();
    }
}

void Game::tickPlayer() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    #pragma region
    if(world->player) {
        if(Controls::PLAYER_UP->get() && !Controls::DEBUG_DRAW->get()) {
            if(world->player->ground) {
                world->player->vy = -4;
                audioEngine.PlayEvent("event:/Player/Jump");
            }
        }

        world->player->vy += (float)(((Controls::PLAYER_UP->get() && !Controls::DEBUG_DRAW->get()) ? (world->player->vy > -1 ? -0.8 : -0.35) : 0) + (Controls::PLAYER_DOWN->get() ? 0.1 : 0));
        if(Controls::PLAYER_UP->get() && !Controls::DEBUG_DRAW->get()) {
            audioEngine.SetEventParameter("event:/Player/Fly", "Intensity", 1);
            for(int i = 0; i < 4; i++) {
                Particle* p = new Particle(Tiles::createLava(), (float)(world->player->x + world->loadZone.x + world->player->hw / 2 + rand() % 5 - 2 + world->player->vx), (float)(world->player->y + world->loadZone.y + world->player->hh + world->player->vy), (float)((rand() % 10 - 5) / 10.0f + world->player->vx / 2.0f), (float)((rand() % 10) / 10.0f + 1 + world->player->vy / 2.0f), 0, (float)0.025);
                p->temporary = true;
                p->lifetime = 120;
                world->addParticle(p);
            }
        } else {
            audioEngine.SetEventParameter("event:/Player/Fly", "Intensity", 0);
        }

        if(world->player->vy > 0) {
            audioEngine.SetEventParameter("event:/Player/Wind", "Wind", (float)(world->player->vy / 12.0));
        } else {
            audioEngine.SetEventParameter("event:/Player/Wind", "Wind", 0);
        }

        world->player->vx += (float)((Controls::PLAYER_LEFT->get() ? (world->player->vx > 0 ? -0.4 : -0.2) : 0) + (Controls::PLAYER_RIGHT->get() ? (world->player->vx < 0 ? 0.4 : 0.2) : 0));
        if(!Controls::PLAYER_LEFT->get() && !Controls::PLAYER_RIGHT->get()) world->player->vx *= (float)(world->player->ground ? 0.85 : 0.96);
        if(world->player->vx > 4.5) world->player->vx = 4.5;
        if(world->player->vx < -4.5) world->player->vx = -4.5;
    } else {
        if(state == INGAME) {
            freeCamX += (float)((Controls::PLAYER_LEFT->get() ? -5 : 0) + (Controls::PLAYER_RIGHT->get() ? 5 : 0));
            freeCamY += (float)((Controls::PLAYER_UP->get() ? -5 : 0) + (Controls::PLAYER_DOWN->get() ? 5 : 0));
        } else {

        }
    }
    #pragma endregion

    #pragma region
    if(world->player) {
        desCamX = (float)(-(mx - (WIDTH / 2)) / 4);
        desCamY = (float)(-(my - (HEIGHT / 2)) / 4);

        world->player->holdAngle = (float)(atan2(desCamY, desCamX) * 180 / (float)W_PI);

        desCamX = 0;
        desCamY = 0;
    } else {
        desCamX = 0;
        desCamY = 0;
    }
    #pragma endregion

    #pragma region
    if(world->player) {
        if(world->player->heldItem) {
            if(world->player->heldItem->getFlag(ItemFlags::VACUUM)) {
                if(world->player->holdVacuum) {

                    int wcx = (int)((WIDTH / 2.0f - ofsX - camX) / scale);
                    int wcy = (int)((HEIGHT / 2.0f - ofsY - camY) / scale);

                    int wmx = (int)((mx - ofsX - camX) / scale);
                    int wmy = (int)((my - ofsY - camY) / scale);

                    int mdx = wmx - wcx;
                    int mdy = wmy - wcy;

                    int distSq = mdx * mdx + mdy * mdy;
                    if(distSq <= 256 * 256) {

                        int sind = -1;
                        bool inObject = true;
                        world->forLine(wcx, wcy, wmx, wmy, [&](int ind) {
                            if(world->tiles[ind].mat->physicsType == PhysicsType::OBJECT) {
                                if(!inObject) {
                                    sind = ind;
                                    return true;
                                }
                            } else {
                                inObject = false;
                            }

                            if(world->tiles[ind].mat->physicsType == PhysicsType::SOLID || world->tiles[ind].mat->physicsType == PhysicsType::SAND || world->tiles[ind].mat->physicsType == PhysicsType::SOUP) {
                                sind = ind;
                                return true;
                            }
                            return false;
                        });

                        int x = sind == -1 ? wmx : sind % world->width;
                        int y = sind == -1 ? wmy : sind / world->width;

                        std::function<void(MaterialInstance, int, int)> makeParticle = [&](MaterialInstance tile, int xPos, int yPos) {
                            Particle* par = new Particle(tile, xPos, yPos, 0, 0, 0, (float)0.01f);
                            par->vx = (rand() % 10 - 5) / 5.0f * 1.0f;
                            par->vy = (rand() % 10 - 5) / 5.0f * 1.0f;
                            par->ax = -par->vx / 10.0f;
                            par->ay = -par->vy / 10.0f;
                            if(par->ay == 0 && par->ax == 0) par->ay = 0.01f;

                            //par->targetX = world->player->x + world->player->hw / 2 + world->loadZone.x;
                            //par->targetY = world->player->y + world->player->hh / 2 + world->loadZone.y;
                            //par->targetForce = 0.35f;

                            par->lifetime = 6;

                            par->phase = true;

                            world->player->heldItem->vacuumParticles.push_back(par);

                            par->killCallback = [&]() {
                                auto& v = world->player->heldItem->vacuumParticles;
                                v.erase(std::remove(v.begin(), v.end(), par), v.end());
                            };

                            world->addParticle(par);
                        };

                        int rad = 5;
                        int clipRadSq = rad * rad;
                        clipRadSq += rand() % clipRadSq / 4;
                        for(int xx = -rad; xx <= rad; xx++) {
                            for(int yy = -rad; yy <= rad; yy++) {
                                if(xx * xx + yy * yy > clipRadSq) continue;
                                if((yy == -rad || yy == rad) && (xx == -rad || xx == rad)) {
                                    continue;
                                }

                                MaterialInstance tile = world->tiles[(x + xx) + (y + yy) * world->width];
                                if(tile.mat->physicsType == PhysicsType::SOLID || tile.mat->physicsType == PhysicsType::SAND || tile.mat->physicsType == PhysicsType::SOUP) {
                                    makeParticle(tile, x + xx, y + yy);
                                    world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::NOTHING;
                                    //world->tiles[(x + xx) + (y + yy) * world->width] = Tiles::createFire();
                                    world->dirty[(x + xx) + (y + yy) * world->width] = true;
                                }


                            }
                        }

                        world->particles.erase(std::remove_if(world->particles.begin(), world->particles.end(), [&](Particle* cur) {
                            if(cur->targetForce == 0 && !cur->phase) {
                                int rad = 5;
                                for(int xx = -rad; xx <= rad; xx++) {
                                    for(int yy = -rad; yy <= rad; yy++) {
                                        if((yy == -rad || yy == rad) && (xx == -rad || x == rad)) continue;

                                        if(((int)(cur->x) == (x + xx)) && ((int)(cur->y) == (y + yy))) {

                                            cur->vx = (rand() % 10 - 5) / 5.0f * 1.0f;
                                            cur->vy = (rand() % 10 - 5) / 5.0f * 1.0f;
                                            cur->ax = -cur->vx / 10.0f;
                                            cur->ay = -cur->vy / 10.0f;
                                            if(cur->ay == 0 && cur->ax == 0) cur->ay = 0.01f;

                                            //par->targetX = world->player->x + world->player->hw / 2 + world->loadZone.x;
                                            //par->targetY = world->player->y + world->player->hh / 2 + world->loadZone.y;
                                            //par->targetForce = 0.35f;

                                            cur->lifetime = 6;

                                            cur->phase = true;

                                            world->player->heldItem->vacuumParticles.push_back(cur);

                                            cur->killCallback = [&]() {
                                                auto& v = world->player->heldItem->vacuumParticles;
                                                v.erase(std::remove(v.begin(), v.end(), cur), v.end());
                                            };

                                            return false;
                                        }
                                    }
                                }
                            }

                            return false;
                        }), world->particles.end());

                        vector<RigidBody*> rbs = world->rigidBodies;

                        for(size_t i = 0; i < rbs.size(); i++) {
                            RigidBody* cur = rbs[i];
                            if(cur->body->IsEnabled()) {
                                float s = sin(-cur->body->GetAngle());
                                float c = cos(-cur->body->GetAngle());
                                bool upd = false;
                                for(int xx = -rad; xx <= rad; xx++) {
                                    for(int yy = -rad; yy <= rad; yy++) {
                                        if((yy == -rad || yy == rad) && (xx == -rad || x == rad)) continue;
                                        // rotate point

                                        float tx = x + xx - cur->body->GetPosition().x;
                                        float ty = y + yy - cur->body->GetPosition().y;

                                        int ntx = (int)(tx * c - ty * s);
                                        int nty = (int)(tx * s + ty * c);

                                        if(ntx >= 0 && nty >= 0 && ntx < cur->surface->w && nty < cur->surface->h) {
                                            Uint32 pixel = PIXEL(cur->surface, ntx, nty);
                                            if(((pixel >> 24) & 0xff) != 0x00) {
                                                PIXEL(cur->surface, ntx, nty) = 0x00000000;
                                                upd = true;

                                                makeParticle(MaterialInstance(&Materials::GENERIC_SOLID, pixel), (x + xx), (y + yy));
                                            }

                                        }
                                    }
                                }

                                if(upd) {
                                    GPU_FreeImage(cur->texture);
                                    cur->texture = GPU_CopyImageFromSurface(cur->surface);
                                    GPU_SetImageFilter(cur->texture, GPU_FILTER_NEAREST);
                                    //world->updateRigidBodyHitbox(cur);
                                    cur->needsUpdate = true;
                                }
                            }
                        }

                    }
                }

                if(world->player->heldItem->vacuumParticles.size() > 0) {
                    world->player->heldItem->vacuumParticles.erase(std::remove_if(world->player->heldItem->vacuumParticles.begin(), world->player->heldItem->vacuumParticles.end(), [&](Particle* cur) {

                        if(cur->lifetime <= 0) {
                            cur->targetForce = 0.45f;
                            cur->targetX = world->player->x + world->player->hw / 2.0f + world->loadZone.x;
                            cur->targetY = world->player->y + world->player->hh / 2.0f + world->loadZone.y;
                            cur->ax = 0;
                            cur->ay = 0.01f;
                        }

                        float tdx = cur->targetX - cur->x;
                        float tdy = cur->targetY - cur->y;

                        if(tdx * tdx + tdy * tdy < 10 * 10) {
                            cur->temporary = true;
                            cur->lifetime = 0;
                            //logDebug("vacuum {}", cur->tile.mat->name.c_str());
                            return true;
                        }

                        return false;
                    }), world->player->heldItem->vacuumParticles.end());
                }

            }
        }
    }
    #pragma endregion

}

void Game::updateFrameLate() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);
    if(state == LOADING) {

    } else {

        // update camera
        #pragma region
        int nofsX;
        int nofsY;

        if(world->player) {

            if(now - lastTick <= mspt) {
                float thruTick = (float)((now - lastTick) / (double)mspt);

                plPosX = world->player->x + (int)(world->player->vx * thruTick);
                plPosY = world->player->y + (int)(world->player->vy * thruTick);
            } else {
                //plPosX = world->player->x;
                //plPosY = world->player->y;
            }

            //plPosX = (float)(plPosX + (world->player->x - plPosX) / 25.0);
            //plPosY = (float)(plPosY + (world->player->y - plPosY) / 25.0);

            nofsX = (int)(-((int)plPosX + world->player->hw / 2 + world->loadZone.x) * scale + WIDTH / 2);
            nofsY = (int)(-((int)plPosY + world->player->hh / 2 + world->loadZone.y) * scale + HEIGHT / 2);
        } else {
            plPosX = (float)(plPosX + (freeCamX - plPosX) / 50.0f);
            plPosY = (float)(plPosY + (freeCamY - plPosY) / 50.0f);

            nofsX = (int)(-(plPosX + 0 + world->loadZone.x) * scale + WIDTH / 2.0f);
            nofsY = (int)(-(plPosY + 0 + world->loadZone.y) * scale + HEIGHT / 2.0f);
        }

        accLoadX += (nofsX - ofsX) / (float)scale;
        accLoadY += (nofsY - ofsY) / (float)scale;
        //logDebug("{0:f} {0:f}", plPosX, plPosY);
        //logDebug("a {0:d} {0:d}", nofsX, nofsY);
        //logDebug("{0:d} {0:d}", nofsX - ofsX, nofsY - ofsY);
        ofsX += (nofsX - ofsX);
        ofsY += (nofsY - ofsY);
        #pragma endregion

        camX = (float)(camX + (desCamX - camX) * (now - lastTime) / 250.0f);
        camY = (float)(camY + (desCamY - camY) * (now - lastTime) / 250.0f);
    }
}

void Game::renderEarly() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);
    if(state == LOADING) {
        if(now - lastLoadingTick > 20) {
            // render loading screen
            #pragma region
            EASY_BLOCK("render loading screen");
            unsigned int* ldPixels = (unsigned int*)pixelsLoading_ar;
            bool anyFalse = false;
            //int drop  = (sin(now / 250.0) + 1) / 2 * loadingScreenW;
            //int drop2 = (-sin(now / 250.0) + 1) / 2 * loadingScreenW;
            for(int x = 0; x < loadingScreenW; x++) {
                for(int y = loadingScreenH - 1; y >= 0; y--) {
                    int i = (x + y * loadingScreenW);
                    bool state = ldPixels[i] == loadingOnColor;

                    if(!state) anyFalse = true;
                    bool newState = state;
                    //newState = rand() % 2;

                    if(!state && y == 0) {
                        if(rand() % 6 == 0) {
                            newState = true;
                        }
                        /*if (x >= drop - 1 && x <= drop + 1) {
                            newState = true;
                        }else if (x >= drop2 - 1 && x <= drop2 + 1) {
                            newState = true;
                        }*/
                    }

                    if(state && y < loadingScreenH - 1) {
                        if(ldPixels[(x + (y + 1) * loadingScreenW)] == loadingOffColor) {
                            ldPixels[(x + (y + 1) * loadingScreenW)] = loadingOnColor;
                            newState = false;
                        } else {
                            bool canLeft = x > 0 && ldPixels[((x - 1) + (y + 1) * loadingScreenW)] == loadingOffColor;
                            bool canRight = x < loadingScreenW - 1 && ldPixels[((x + 1) + (y + 1) * loadingScreenW)] == loadingOffColor;
                            if(canLeft && !(canRight && (rand() % 2 == 0))) {
                                ldPixels[((x - 1) + (y + 1) * loadingScreenW)] = loadingOnColor;
                                newState = false;
                            } else if(canRight) {
                                ldPixels[((x + 1) + (y + 1) * loadingScreenW)] = loadingOnColor;
                                newState = false;
                            }
                        }
                    }

                    ldPixels[(x + y * loadingScreenW)] = (newState ? loadingOnColor : loadingOffColor);
                    int sx = WIDTH / loadingScreenW;
                    int sy = HEIGHT / loadingScreenH;
                    //GPU_RectangleFilled(target, x * sx, y * sy, x * sx + sx, y * sy + sy, state ? SDL_Color{ 0xff, 0, 0, 0xff } : SDL_Color{ 0, 0xff, 0, 0xff });
                }
            }
            if(!anyFalse) {
                uint32 tmp = loadingOnColor;
                loadingOnColor = loadingOffColor;
                loadingOffColor = tmp;
            }

            GPU_UpdateImageBytes(
                loadingTexture,
                NULL,
                &pixelsLoading_ar[0],
                loadingScreenW * 4
            );
            EASY_END_BLOCK;
            #pragma endregion

            lastLoadingTick = now;
        } else {
            #ifdef _WIN32
            Sleep(5);
            #else
            sleep(5/1000.0f);
            #endif
        }
        GPU_ActivateShaderProgram(0, NULL);
        GPU_BlitRect(loadingTexture, NULL, target, NULL);
        if(dt_loading.w == -1) {
            dt_loading = Drawing::drawTextParams(target, "Loading...", font64, WIDTH / 2, HEIGHT / 2 - 32, 255, 255, 255, ALIGN_CENTER);
        }
        Drawing::drawText(target, dt_loading, WIDTH / 2, HEIGHT / 2 - 32, ALIGN_CENTER);
    } else {
        // render entities with LERP
        #pragma region
        if(now - lastTick <= mspt) {
            GPU_Clear(textureEntities->target);
            GPU_Clear(textureEntitiesLQ->target);
            if(world->player) {
                float thruTick = (float)((now - lastTick) / (double)mspt);
                EASY_BLOCK("render entities LERP");
                GPU_SetBlendMode(textureEntities, GPU_BLEND_ADD);
                GPU_SetBlendMode(textureEntitiesLQ, GPU_BLEND_ADD);
                int scaleEnt = Settings::hd_objects ? Settings::hd_objects_size : 1;

                for(auto& v : world->entities) {
                    v->renderLQ(textureEntitiesLQ->target, world->loadZone.x + (int)(v->vx * thruTick), world->loadZone.y + (int)(v->vy * thruTick));
                    v->render(textureEntities->target, world->loadZone.x + (int)(v->vx * thruTick), world->loadZone.y + (int)(v->vy * thruTick));
                }

                if(world->player && world->player->heldItem != NULL) {
                    if(world->player->heldItem->getFlag(ItemFlags::HAMMER)) {
                        if(world->player->holdHammer) {
                            int x = (int)((mx - ofsX - camX) / scale);
                            int y = (int)((my - ofsY - camY) / scale);

                            int dx = x - world->player->hammerX;
                            int dy = y - world->player->hammerY;
                            float len = sqrt(dx * dx + dy * dy);
                            if(len > 40) {
                                dx = dx / len * 40;
                                dy = dy / len * 40;
                            }

                            GPU_Line(textureEntitiesLQ->target, world->player->hammerX + dx, world->player->hammerY + dy, world->player->hammerX, world->player->hammerY, {0xff, 0xff, 0x00, 0xff});
                        } else {
                            int startInd = getAimSolidSurface(64);

                            if(startInd != -1) {
                                int x = startInd % world->width;
                                int y = startInd / world->width;
                                GPU_Rectangle(textureEntitiesLQ->target, x - 1, y - 1, x + 1, y + 1, {0xff, 0xff, 0x00, 0xE0});
                            }
                        }
                    }
                }
                GPU_SetBlendMode(textureEntities, GPU_BLEND_NORMAL);
                GPU_SetBlendMode(textureEntitiesLQ, GPU_BLEND_NORMAL);
                EASY_END_BLOCK;
            }
        }
        #pragma endregion

        if(Controls::mmouse) {
            int x = (int)((mx - ofsX - camX) / scale);
            int y = (int)((my - ofsY - camY) / scale);
            GPU_RectangleFilled(textureEntitiesLQ->target, x - DebugDrawUI::brushSize / 2.0f, y - DebugDrawUI::brushSize / 2.0f, x + (int)(ceil(DebugDrawUI::brushSize / 2.0)), y + (int)(ceil(DebugDrawUI::brushSize / 2.0)), {0xff, 0x40, 0x40, 0x90});
            GPU_Rectangle(textureEntitiesLQ->target, x - DebugDrawUI::brushSize / 2.0f, y - DebugDrawUI::brushSize / 2.0f, x + (int)(ceil(DebugDrawUI::brushSize / 2.0)) + 1, y + (int)(ceil(DebugDrawUI::brushSize / 2.0)) + 1, {0xff, 0x40, 0x40, 0xE0});
        }else if(Controls::DEBUG_DRAW->get()) {
            int x = (int)((mx - ofsX - camX) / scale);
            int y = (int)((my - ofsY - camY) / scale);
            GPU_RectangleFilled(textureEntitiesLQ->target, x - DebugDrawUI::brushSize / 2.0f, y - DebugDrawUI::brushSize / 2.0f, x + (int)(ceil(DebugDrawUI::brushSize / 2.0)), y + (int)(ceil(DebugDrawUI::brushSize / 2.0)), {0x00, 0xff, 0xB0, 0x80});
            GPU_Rectangle(textureEntitiesLQ->target, x - DebugDrawUI::brushSize / 2.0f, y - DebugDrawUI::brushSize / 2.0f, x + (int)(ceil(DebugDrawUI::brushSize / 2.0)) + 1, y + (int)(ceil(DebugDrawUI::brushSize / 2.0)) + 1, {0x00, 0xff, 0xB0, 0xE0});
        }

    }
}

void Game::renderLate() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    target = backgroundImage->target;
    GPU_Clear(target);

    if(state == LOADING) {

    } else {
        // draw backgrounds
        #pragma region
        EASY_BLOCK("draw background", RENDER_PROFILER_COLOR);
        Background bg = Backgrounds::TEST_OVERWORLD;
        if(Settings::draw_background && scale <= bg.layers[0].surface.size() && world->loadZone.y > -5 * CHUNK_H) {
            GPU_SetShapeBlendMode(GPU_BLEND_SET);
            GPU_ClearColor(target, {(bg.solid >> 16) & 0xff, (bg.solid >> 8) & 0xff, (bg.solid >> 0) & 0xff, 0xff});

            GPU_Rect* dst = new GPU_Rect();
            GPU_Rect* src = new GPU_Rect();

            float arX = (float)WIDTH / (bg.layers[0].surface[0]->w);
            float arY = (float)HEIGHT / (bg.layers[0].surface[0]->h);

            double time = Time::millis() / 1000.0;

            GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);

            for(size_t i = 0; i < bg.layers.size(); i++) {
                BackgroundLayer cur = bg.layers[i];

                SDL_Surface* texture = cur.surface[(size_t)scale - 1];

                GPU_Image* tex = cur.texture[(size_t)scale - 1];
                GPU_SetBlendMode(tex, GPU_BLEND_NORMAL);

                int tw = texture->w;
                int th = texture->h;

                int iter = (int)ceil((float)WIDTH / (tw)) + 1;
                for(int n = 0; n < iter; n++) {

                    src->x = 0;
                    src->y = 0;
                    src->w = tw;
                    src->h = th;

                    dst->x = (((ofsX + camX) + world->loadZone.x*scale) + n * tw / cur.parralaxX) * cur.parralaxX + world->width / 2.0f * scale - tw / 2.0f;
                    dst->y = ((ofsY + camY) + world->loadZone.y*scale) * cur.parralaxY + world->height / 2.0f * scale - th / 2.0f - HEIGHT / 3.0f * (scale - 1);
                    dst->w = (float)tw;
                    dst->h = (float)th;

                    dst->x += (float)(scale * fmod(cur.moveX * time, tw));

                    //TODO: optimize
                    while(dst->x >= WIDTH - 10) dst->x -= (iter * tw);
                    while(dst->x + dst->w < 0) dst->x += (iter * tw - 1);

                    //TODO: optimize
                    if(dst->x < 0) {
                        dst->w += dst->x;
                        src->x -= (int)dst->x;
                        src->w += (int)dst->x;
                        dst->x = 0;
                    }

                    if(dst->y < 0) {
                        dst->h += dst->y;
                        src->y -= (int)dst->y;
                        src->h += (int)dst->y;
                        dst->y = 0;
                    }

                    if(dst->x + dst->w >= WIDTH) {
                        src->w -= (int)((dst->x + dst->w) - WIDTH);
                        dst->w += WIDTH - (dst->x + dst->w);
                    }

                    if(dst->y + dst->h >= HEIGHT) {
                        src->h -= (int)((dst->y + dst->h) - HEIGHT);
                        dst->h += HEIGHT - (dst->y + dst->h);
                    }

                    GPU_BlitRect(tex, src, target, dst);
                }
            }

            delete dst;
            delete src;
        }
        EASY_END_BLOCK; // draw backgrounds
        #pragma endregion

        EASY_BLOCK("draw world textures", RENDER_PROFILER_COLOR);
        GPU_Rect r1 = GPU_Rect {(float)(ofsX + camX), (float)(ofsY + camY), (float)(world->width * scale), (float)(world->height * scale)};
        GPU_SetBlendMode(textureBackground, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureBackground, NULL, target, &r1);

        GPU_SetBlendMode(textureLayer2, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureLayer2, NULL, target, &r1);

        GPU_SetBlendMode(textureObjectsBack, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureObjectsBack, NULL, target, &r1);

        // shader
        EASY_BLOCK("water shader");
        if(Settings::draw_shaders) {

            if(waterFlowPassShader->dirty && Settings::water_showFlow) {
                EASY_BLOCK("waterFlowPassShader", GPU_PROFILER_COLOR);
                waterFlowPassShader->activate();
                waterFlowPassShader->update(world->width, world->height);
                GPU_SetBlendMode(textureFlow, GPU_BLEND_SET);
                GPU_BlitRect(textureFlow, NULL, textureFlowSpead->target, NULL);
                EASY_END_BLOCK;

                waterFlowPassShader->dirty = false;
            }

            waterShader->activate();
            float t = (now - startTime) / 1000.0;
            waterShader->update(t, target->w * scale, target->h * scale, texture, r1.x, r1.y, r1.w, r1.h, scale, textureFlowSpead, Settings::water_overlay, Settings::water_showFlow, Settings::water_pixelated);
        }

        target = realTarget;

        GPU_BlitRect(backgroundImage, NULL, target, NULL);

        GPU_SetBlendMode(texture, GPU_BLEND_NORMAL);
        GPU_ActivateShaderProgram(0, NULL);
        EASY_END_BLOCK; //water shader

        // done shader

        EASY_BLOCK("draw world");
        int lmsx = (int)((mx - ofsX - camX) / scale);
        int lmsy = (int)((my - ofsY - camY) / scale);

        GPU_Clear(worldTexture->target);

        GPU_BlitRect(texture, NULL, worldTexture->target, NULL);

        GPU_SetBlendMode(textureObjects, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureObjects, NULL, worldTexture->target, NULL);
        GPU_SetBlendMode(textureObjectsLQ, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureObjectsLQ, NULL, worldTexture->target, NULL);

        GPU_SetBlendMode(textureParticles, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureParticles, NULL, worldTexture->target, NULL);

        GPU_SetBlendMode(textureEntitiesLQ, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureEntitiesLQ, NULL, worldTexture->target, NULL);
        GPU_SetBlendMode(textureEntities, GPU_BLEND_NORMAL);
        GPU_BlitRect(textureEntities, NULL, worldTexture->target, NULL);

        EASY_BLOCK("lighting shader");
        if(Settings::draw_shaders) newLightingShader->activate();

        // I use this to only rerender the lighting when a parameter changes or N times per second anyway
        // Doing this massively reduces the GPU load of the shader
        bool needToRerenderLighting = false;

        static long long lastLightingForceRefresh = 0;
        long long now = Time::millis();
        if(now - lastLightingForceRefresh > 100) {
            lastLightingForceRefresh = now;
            needToRerenderLighting = true;
        }

        if(Settings::draw_shaders && world) {
            float lightTx;
            float lightTy;

            if(world->player) {
                lightTx = (world->loadZone.x + world->player->x + world->player->hw / 2.0f) / (float)world->width;
                lightTy = (world->loadZone.y + world->player->y + world->player->hh / 2.0f) / (float)world->height;
            } else {
                lightTx = lmsx / (float)world->width;
                lightTy = lmsy / (float)world->height;
            }

            if(newLightingShader->lastLx != lightTx || newLightingShader->lastLy != lightTy) needToRerenderLighting = true;
            newLightingShader->update(worldTexture, emissionTexture, lightTx, lightTy);
            if(newLightingShader->lastQuality != Settings::lightingQuality) {
                needToRerenderLighting = true;
            }
            newLightingShader->setQuality(Settings::lightingQuality);

            int nBg = 0;
            int range = 64;
            for(int xx = max(0, (int)(lightTx * world->width) - range); xx <= min((int)(lightTx * world->width) + range, world->width - 1); xx++) {
                for(int yy = max(0, (int)(lightTy * world->height) - range); yy <= min((int)(lightTy * world->height) + range, world->height - 1); yy++) {
                    if(world->background[xx + yy * world->width] != 0x00) {
                        nBg++;
                    }
                }
            }

            newLightingShader_insideDes = min(max(0.0f, (float)nBg / ((range * 2) * (range * 2))), 1.0f);
            newLightingShader_insideCur += (newLightingShader_insideDes - newLightingShader_insideCur) / 2.0f * (deltaTime / 1000.0f);

            float ins = newLightingShader_insideCur < 0.05 ? 0.0 : newLightingShader_insideCur;
            if(newLightingShader->lastInside != ins) needToRerenderLighting = true;
            newLightingShader->setInside(ins);
            newLightingShader->setBounds(world->tickZone.x * Settings::hd_objects_size, world->tickZone.y * Settings::hd_objects_size, (world->tickZone.x + world->tickZone.w) * Settings::hd_objects_size, (world->tickZone.y + world->tickZone.h) * Settings::hd_objects_size);

            if(newLightingShader->lastSimpleMode != Settings::simpleLighting) needToRerenderLighting = true;
            newLightingShader->setSimpleMode(Settings::simpleLighting);

            if(newLightingShader->lastEmissionEnabled != Settings::lightingEmission) needToRerenderLighting = true;
            newLightingShader->setEmissionEnabled(Settings::lightingEmission);

            if(newLightingShader->lastDitheringEnabled != Settings::lightingDithering) needToRerenderLighting = true;
            newLightingShader->setDitheringEnabled(Settings::lightingDithering);
        }
        
        if(Settings::draw_shaders && needToRerenderLighting) {
            GPU_Clear(lightingTexture->target);
            GPU_BlitRect(worldTexture, NULL, lightingTexture->target, NULL);
        }
        if(Settings::draw_shaders) GPU_ActivateShaderProgram(0, NULL);

        EASY_END_BLOCK; // lighting shader

        GPU_BlitRect(worldTexture, NULL, target, &r1);

        if(Settings::draw_shaders) {
            GPU_SetBlendMode(lightingTexture, Settings::draw_light_overlay ? GPU_BLEND_NORMAL : GPU_BLEND_MULTIPLY);
            GPU_BlitRect(lightingTexture, NULL, target, &r1);
        }

        EASY_END_BLOCK; // draw world

        if(Settings::draw_shaders) {
            GPU_Clear(texture2Fire->target);
            EASY_BLOCK("fire shader 1");
            fireShader->activate();
            fireShader->update(textureFire);
            GPU_BlitRect(textureFire, NULL, texture2Fire->target, NULL);
            GPU_ActivateShaderProgram(0, NULL);
            EASY_END_BLOCK;

            EASY_BLOCK("fire shader 2");
            fire2Shader->activate();
            fire2Shader->update(texture2Fire);
            GPU_BlitRect(texture2Fire, NULL, target, &r1);
            GPU_ActivateShaderProgram(0, NULL);
            EASY_END_BLOCK;
        }

        // done light

        renderOverlays();

    }
}

void Game::renderOverlays() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    GPU_Rect r1 = GPU_Rect {(float)(ofsX + camX), (float)(ofsY + camY), (float)(world->width * scale), (float)(world->height * scale)};
    GPU_Rect r2 = GPU_Rect {(float)(ofsX + camX + world->tickZone.x * scale), (float)(ofsY + camY + world->tickZone.y * scale), (float)(world->tickZone.w * scale), (float)(world->tickZone.h * scale)};

    if(Settings::draw_temperature_map) {
        GPU_SetBlendMode(temperatureMap, GPU_BLEND_NORMAL);
        GPU_BlitRect(temperatureMap, NULL, target, &r1);
    }
    EASY_END_BLOCK; // draw world textures

    if(Settings::draw_load_zones) {
        GPU_Rect r2m = GPU_Rect {(float)(ofsX + camX + world->meshZone.x * scale),
            (float)(ofsY + camY + world->meshZone.y * scale),
            (float)(world->meshZone.w * scale),
            (float)(world->meshZone.h * scale)};

        GPU_Rectangle2(target, r2m, {0x00, 0xff, 0xff, 0xff});
        GPU_Rectangle2(target, r2, {0xff, 0x00, 0x00, 0xff});
    }

    if(Settings::draw_load_zones) {
        EASY_BLOCK("draw load zones", RENDER_PROFILER_COLOR);
        SDL_Color col = {0xff, 0x00, 0x00, 0x20};
        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);

        GPU_Rect r3 = GPU_Rect {(float)(0), (float)(0), (float)((ofsX + camX + world->tickZone.x * scale)), (float)(HEIGHT)};
        GPU_Rectangle2(target, r3, col);

        GPU_Rect r4 = GPU_Rect {(float)(ofsX + camX + world->tickZone.x * scale + world->tickZone.w * scale), (float)(0), (float)((WIDTH)-(ofsX + camX + world->tickZone.x * scale + world->tickZone.w * scale)), (float)(HEIGHT)};
        GPU_Rectangle2(target, r3, col);

        GPU_Rect r5 = GPU_Rect {(float)(ofsX + camX + world->tickZone.x * scale), (float)(0), (float)(world->tickZone.w * scale), (float)(ofsY + camY + world->tickZone.y * scale)};
        GPU_Rectangle2(target, r3, col);

        GPU_Rect r6 = GPU_Rect {(float)(ofsX + camX + world->tickZone.x * scale), (float)(ofsY + camY + world->tickZone.y * scale + world->tickZone.h * scale), (float)(world->tickZone.w * scale), (float)(HEIGHT - (ofsY + camY + world->tickZone.y * scale + world->tickZone.h * scale))};
        GPU_Rectangle2(target, r6, col);

        col = {0x00, 0xff, 0x00, 0xff};
        GPU_Rect r7 = GPU_Rect {(float)(ofsX + camX + world->width / 2 * scale - (WIDTH / 3 * scale / 2)), (float)(ofsY + camY + world->height / 2 * scale - (HEIGHT / 3 * scale / 2)), (float)(WIDTH / 3 * scale), (float)(HEIGHT / 3 * scale)};
        GPU_Rectangle2(target, r7, col);

        EASY_END_BLOCK; // draw load zones
    }

    if(Settings::draw_physics_debug) {
        //EASY_BLOCK("draw physics meshes", RENDER_PROFILER_COLOR);
        //for(size_t i = 0; i < world->rigidBodies.size(); i++) {
        //    RigidBody cur = *world->rigidBodies[i];

        //    float x = cur.body->GetPosition().x;
        //    float y = cur.body->GetPosition().y;
        //    x = ((x)*scale + ofsX + camX);
        //    y = ((y)*scale + ofsY + camY);

        //    /*SDL_Rect* r = new SDL_Rect{ (int)x, (int)y, cur.surface->w * scale, cur.surface->h * scale };
        //    SDL_RenderCopyEx(renderer, cur.texture, NULL, r, cur.body->GetAngle() * 180 / M_PI, new SDL_Point{ 0, 0 }, SDL_RendererFlip::SDL_FLIP_NONE);
        //    delete r;*/

        //    Uint32 color = 0x0000ff;

        //    SDL_Color col = {(color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, 0xff};

        //    b2Fixture* fix = cur.body->GetFixtureList();
        //    while(fix) {
        //        b2Shape* shape = fix->GetShape();

        //        switch(shape->GetType()) {
        //        case b2Shape::Type::e_polygon:
        //            b2PolygonShape* poly = (b2PolygonShape*)shape;
        //            b2Vec2* verts = poly->m_vertices;

        //            Drawing::drawPolygon(target, col, verts, (int)x, (int)y, scale, poly->m_count, cur.body->GetAngle()/* + fmod((Time::millis() / 1000.0), 360)*/, 0, 0);

        //            break;
        //        }

        //        fix = fix->GetNext();
        //    }
        //}

        //if(world->player) {
        //    RigidBody cur = *world->player->rb;

        //    float x = cur.body->GetPosition().x;
        //    float y = cur.body->GetPosition().y;
        //    x = ((x)*scale + ofsX + camX);
        //    y = ((y)*scale + ofsY + camY);

        //    Uint32 color = 0x0000ff;

        //    SDL_Color col = {(color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, 0xff};

        //    b2Fixture* fix = cur.body->GetFixtureList();
        //    while(fix) {
        //        b2Shape* shape = fix->GetShape();
        //        switch(shape->GetType()) {
        //        case b2Shape::Type::e_polygon:
        //            b2PolygonShape* poly = (b2PolygonShape*)shape;
        //            b2Vec2* verts = poly->m_vertices;

        //            Drawing::drawPolygon(target, col, verts, (int)x, (int)y, scale, poly->m_count, cur.body->GetAngle()/* + fmod((Time::millis() / 1000.0), 360)*/, 0, 0);

        //            break;
        //        }

        //        fix = fix->GetNext();
        //    }
        //}

        //for(size_t i = 0; i < world->worldRigidBodies.size(); i++) {
        //    RigidBody cur = *world->worldRigidBodies[i];

        //    float x = cur.body->GetPosition().x;
        //    float y = cur.body->GetPosition().y;
        //    x = ((x)*scale + ofsX + camX);
        //    y = ((y)*scale + ofsY + camY);

        //    Uint32 color = 0x00ff00;

        //    SDL_Color col = {(color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, 0xff};

        //    b2Fixture* fix = cur.body->GetFixtureList();
        //    while(fix) {
        //        b2Shape* shape = fix->GetShape();
        //        switch(shape->GetType()) {
        //        case b2Shape::Type::e_polygon:
        //            b2PolygonShape* poly = (b2PolygonShape*)shape;
        //            b2Vec2* verts = poly->m_vertices;

        //            Drawing::drawPolygon(target, col, verts, (int)x, (int)y, scale, poly->m_count, cur.body->GetAngle()/* + fmod((Time::millis() / 1000.0), 360)*/, 0, 0);

        //            break;
        //        }

        //        fix = fix->GetNext();
        //    }

        //    /*color = 0xff0000;
        //    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, 0xff);
        //    SDL_RenderDrawPoint(renderer, x, y);
        //    color = 0x00ff00;
        //    SDL_SetRenderDrawColor(renderer, (color >> 16) & 0xff, (color >> 8) & 0xff, (color >> 0) & 0xff, 0xff);
        //    SDL_RenderDrawPoint(renderer, cur.body->GetLocalCenter().x * scale + ofsX, cur.body->GetLocalCenter().y * scale + ofsY);*/
        //}

        int minChX = (int)floor((world->meshZone.x - world->loadZone.x) / CHUNK_W);
        int minChY = (int)floor((world->meshZone.y - world->loadZone.y) / CHUNK_H);
        int maxChX = (int)ceil((world->meshZone.x + world->meshZone.w - world->loadZone.x) / CHUNK_W);
        int maxChY = (int)ceil((world->meshZone.y + world->meshZone.h - world->loadZone.y) / CHUNK_H);

        for(int cx = minChX; cx <= maxChX; cx++) {
            for(int cy = minChY; cy <= maxChY; cy++) {
                Chunk* ch = world->getChunk(cx, cy);
                SDL_Color col = {255, 0, 0, 255};

                float x = ((ch->x * CHUNK_W + world->loadZone.x) * scale + ofsX + camX);
                float y = ((ch->y * CHUNK_H + world->loadZone.y) * scale + ofsY + camY);

                GPU_Rectangle(target, x, y, x + CHUNK_W * scale, y + CHUNK_H * scale, {50, 50, 0, 255});

                //for(int i = 0; i < ch->polys.size(); i++) {
                //    Drawing::drawPolygon(target, col, ch->polys[i].m_vertices, (int)x, (int)y, scale, ch->polys[i].m_count, 0/* + fmod((Time::millis() / 1000.0), 360)*/, 0, 0);
                //}
            }
        }

        //EASY_END_BLOCK; // draw physics meshes

        world->b2world->SetDebugDraw(b2DebugDraw);
        b2DebugDraw->scale = scale;
        b2DebugDraw->xOfs = ofsX + camX;
        b2DebugDraw->yOfs = ofsY + camY;
        b2DebugDraw->SetFlags(0);
        if(Settings::draw_b2d_shape)      b2DebugDraw->AppendFlags(b2Draw::e_shapeBit);
        if(Settings::draw_b2d_joint)      b2DebugDraw->AppendFlags(b2Draw::e_jointBit);
        if(Settings::draw_b2d_aabb)       b2DebugDraw->AppendFlags(b2Draw::e_aabbBit);
        if(Settings::draw_b2d_pair)       b2DebugDraw->AppendFlags(b2Draw::e_pairBit);
        if(Settings::draw_b2d_centerMass) b2DebugDraw->AppendFlags(b2Draw::e_centerOfMassBit);
        world->b2world->DebugDraw();

    }

    EASY_BLOCK("draw fps", RENDER_PROFILER_COLOR);
    if(dt_fps.w == -1) {
        char buffFps[20];
        snprintf(buffFps, sizeof(buffFps), "%d FPS", fps);
        if(dt_fps.t1 != nullptr) GPU_FreeImage(dt_fps.t1);
        if(dt_fps.t2 != nullptr) GPU_FreeImage(dt_fps.t2);
        dt_fps = Drawing::drawTextParams(target, buffFps, font16, WIDTH - 4, 2, 0xff, 0xff, 0xff, ALIGN_RIGHT);
    }

    Drawing::drawText(target, dt_fps, WIDTH - 4, 2, ALIGN_RIGHT);
    EASY_END_BLOCK; // draw fps

    EASY_BLOCK("draw feels like fps", RENDER_PROFILER_COLOR);
    if(dt_feelsLikeFps.w == -1) {
        char buffFps[22];
        snprintf(buffFps, sizeof(buffFps), "Feels Like: %d FPS", feelsLikeFps);
        if(dt_feelsLikeFps.t1 != nullptr) GPU_FreeImage(dt_feelsLikeFps.t1);
        if(dt_feelsLikeFps.t2 != nullptr) GPU_FreeImage(dt_feelsLikeFps.t2);
        dt_feelsLikeFps = Drawing::drawTextParams(target, buffFps, font16, WIDTH - 4, 2, 0xff, 0xff, 0xff, ALIGN_RIGHT);
    }

    Drawing::drawText(target, dt_feelsLikeFps, WIDTH - 4, 2 + 14, ALIGN_RIGHT);
    EASY_END_BLOCK; // draw feels like fps

    if(Settings::draw_chunk_state) {
        EASY_BLOCK("draw chunk state", RENDER_PROFILER_COLOR);
        int chSize = 10;

        int centerX = WIDTH / 2;
        int centerY = CHUNK_UNLOAD_DIST * chSize + 10;


        int pposX = plPosX;
        int pposY = plPosY;
        int pchx = (int)((pposX / CHUNK_W) * chSize);
        int pchy = (int)((pposY / CHUNK_H) * chSize);
        int pchxf = (int)(((float)pposX / CHUNK_W) * chSize);
        int pchyf = (int)(((float)pposY / CHUNK_H) * chSize);

        GPU_Rectangle(target, centerX - chSize * CHUNK_UNLOAD_DIST + chSize, centerY - chSize * CHUNK_UNLOAD_DIST + chSize, centerX + chSize * CHUNK_UNLOAD_DIST + chSize, centerY + chSize * CHUNK_UNLOAD_DIST + chSize, {0xcc, 0xcc, 0xcc, 0xff});

        GPU_Rect r = {0 , 0, (float)chSize, (float)chSize};
        for(auto& p : world->chunkCache) {
            if(p.first == INT_MIN) continue;
            int cx = p.first;
            for(auto& p2 : p.second) {
                if(p2.first == INT_MIN) continue;
                int cy = p2.first;
                Chunk* m = p2.second;
                r.x = centerX + m->x * chSize - pchx;
                r.y = centerY + m->y * chSize - pchy;
                SDL_Color col;
                if(m->generationPhase == -1) {
                    col = {0x60, 0x60, 0x60, 0xff};
                } else if(m->generationPhase == 0) {
                    col = {0xff, 0x00, 0x00, 0xff};
                } else if(m->generationPhase == 1) {
                    col = {0x00, 0xff, 0x00, 0xff};
                } else if(m->generationPhase == 2) {
                    col = {0x00, 0x00, 0xff, 0xff};
                } else if(m->generationPhase == 3) {
                    col = {0xff, 0xff, 0x00, 0xff};
                } else if(m->generationPhase == 4) {
                    col = {0xff, 0x00, 0xff, 0xff};
                } else if(m->generationPhase == 5) {
                    col = {0x00, 0xff, 0xff, 0xff};
                } else {}
                GPU_Rectangle2(target, r, col);
            }
        }

        int loadx = (int)(((float)-world->loadZone.x / CHUNK_W) * chSize);
        int loady = (int)(((float)-world->loadZone.y / CHUNK_H) * chSize);

        int loadx2 = (int)(((float)(-world->loadZone.x + world->loadZone.w) / CHUNK_W) * chSize);
        int loady2 = (int)(((float)(-world->loadZone.y + world->loadZone.h) / CHUNK_H) * chSize);
        GPU_Rectangle(target, centerX - pchx + loadx, centerY - pchy + loady, centerX - pchx + loadx2, centerY - pchy + loady2, {0x00, 0xff, 0xff, 0xff});

        GPU_Rectangle(target, centerX - pchx + pchxf, centerY - pchy + pchyf, centerX + 1 - pchx + pchxf, centerY + 1 - pchy + pchyf, {0x00, 0xff, 0x00, 0xff});

        EASY_END_BLOCK; // draw chunk state
    }

    if(Settings::draw_debug_stats) {
        EASY_BLOCK("draw chunk queue", RENDER_PROFILER_COLOR);
        int dbgIndex = 1;

        int lineHeight = 16;

        char buff1[32];
        std::string buffAsStdStr1;

        snprintf(buff1, sizeof(buff1), "XY: %.2f / %.2f", plPosX, plPosY);
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        snprintf(buff1, sizeof(buff1), "V: %.3f / %.3f", world->player ? world->player->vx : 0, world->player ? world->player->vy : 0);
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);
        
        snprintf(buff1, sizeof(buff1), "Particles: %d", (int)world->particles.size());
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        snprintf(buff1, sizeof(buff1), "Entities: %d", (int)world->entities.size());
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        int rbCt = 0;
        for(auto& r : world->rigidBodies) {
            if(r->body->IsEnabled()) rbCt++;
        }

        snprintf(buff1, sizeof(buff1), "RigidBodies: %d/%d O, %d W", rbCt, (int)world->rigidBodies.size(), (int)world->worldRigidBodies.size());
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        int rbTriACt = 0;
        int rbTriCt = 0;
        for(size_t i = 0; i < world->rigidBodies.size(); i++) {
            RigidBody cur = *world->rigidBodies[i];

            b2Fixture* fix = cur.body->GetFixtureList();
            while(fix) {
                b2Shape* shape = fix->GetShape();

                switch(shape->GetType()) {
                case b2Shape::Type::e_polygon:
                    rbTriCt++;
                    if(cur.body->IsEnabled()) rbTriACt++;
                    break;
                }

                fix = fix->GetNext();
            }
        }

        int rbTriWCt = 0;

        int minChX = (int)floor((world->meshZone.x - world->loadZone.x) / CHUNK_W);
        int minChY = (int)floor((world->meshZone.y - world->loadZone.y) / CHUNK_H);
        int maxChX = (int)ceil((world->meshZone.x + world->meshZone.w - world->loadZone.x) / CHUNK_W);
        int maxChY = (int)ceil((world->meshZone.y + world->meshZone.h - world->loadZone.y) / CHUNK_H);

        for(int cx = minChX; cx <= maxChX; cx++) {
            for(int cy = minChY; cy <= maxChY; cy++) {
                Chunk* ch = world->getChunk(cx, cy);
                for(int i = 0; i < ch->polys.size(); i++) {
                    rbTriWCt++;
                }
            }
        }

        snprintf(buff1, sizeof(buff1), "Tris: %d/%d O, %d W", rbTriACt, rbTriCt, rbTriWCt);
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        int chCt = 0;
        for(auto& p : world->chunkCache) {
            if(p.first == INT_MIN) continue;
            int cx = p.first;
            for(auto& p2 : p.second) {
                if(p2.first == INT_MIN) continue;
                chCt++;
            }
        }

        snprintf(buff1, sizeof(buff1), "Cached Chunks: %d", chCt);
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);

        snprintf(buff1, sizeof(buff1), "world->readyToReadyToMerge (%d)", (int)world->readyToReadyToMerge.size());
        buffAsStdStr1 = buff1;
        Drawing::drawTextBG(target, buffAsStdStr1.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);
        for(size_t i = 0; i < world->readyToReadyToMerge.size(); i++) {
            char buff[10];
            snprintf(buff, sizeof(buff), "    #%d", (int)i);
            std::string buffAsStdStr = buff;
            Drawing::drawTextBG(target, buffAsStdStr.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);
        }
        char buff2[30];
        snprintf(buff2, sizeof(buff2), "world->readyToMerge (%d)", (int)world->readyToMerge.size());
        std::string buffAsStdStr2 = buff2;
        Drawing::drawTextBG(target, buffAsStdStr2.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);
        for(size_t i = 0; i < world->readyToMerge.size(); i++) {
            char buff[20];
            snprintf(buff, sizeof(buff), "    #%d (%d, %d)", (int)i, world->readyToMerge[i]->x, world->readyToMerge[i]->y);
            std::string buffAsStdStr = buff;
            Drawing::drawTextBG(target, buffAsStdStr.c_str(), font16, 4, 2 + (lineHeight * dbgIndex++), 0xff, 0xff, 0xff, {0x00, 0x00, 0x00, 0x40}, ALIGN_LEFT);
        }

        EASY_END_BLOCK; // draw chunk queue
    }

    if(Settings::draw_frame_graph) {
        EASY_BLOCK("draw frame graph", RENDER_PROFILER_COLOR);
        for(int i = 0; i <= 4; i++) {
            if(dt_frameGraph[i].w == -1) {
                char buff[20];
                snprintf(buff, sizeof(buff), "%d", i * 25);
                std::string buffAsStdStr = buff;
                if(dt_frameGraph[i].t1 != nullptr) GPU_FreeImage(dt_frameGraph[i].t1);
                if(dt_frameGraph[i].t2 != nullptr) GPU_FreeImage(dt_frameGraph[i].t2);
                dt_frameGraph[i] = Drawing::drawTextParams(target, buffAsStdStr.c_str(), font14, WIDTH - 20, HEIGHT - 15 - (i * 25) - 2, 0xff, 0xff, 0xff, ALIGN_LEFT);
            }

            Drawing::drawText(target, dt_frameGraph[i], WIDTH - 20, HEIGHT - 15 - (i * 25) - 2, ALIGN_LEFT);
            GPU_Line(target, WIDTH - 30 - frameTimeNum - 5, HEIGHT - 10 - (i * 25), WIDTH - 25, HEIGHT - 10 - (i * 25), {0xff, 0xff, 0xff, 0xff});
        }
        /*for (int i = 0; i <= 100; i += 25) {
            char buff[20];
            snprintf(buff, sizeof(buff), "%d", i);
            std::string buffAsStdStr = buff;
            Drawing::drawText(renderer, buffAsStdStr.c_str(), font14, WIDTH - 20, HEIGHT - 15 - i - 2, 0xff, 0xff, 0xff, ALIGN_LEFT);
            SDL_RenderDrawLine(renderer, WIDTH - 30 - frameTimeNum - 5, HEIGHT - 10 - i, WIDTH - 25, HEIGHT - 10 - i);
        }*/

        for(int i = 0; i < frameTimeNum; i++) {
            int h = frameTime[i];

            SDL_Color col;
            if(h <= (int)(1000 / 144.0)) {
                col = {0x00, 0xff, 0x00, 0xff};
            } else if(h <= (int)(1000 / 60.0)) {
                col = {0xa0, 0xe0, 0x00, 0xff};
            } else if(h <= (int)(1000 / 30.0)) {
                col = {0xff, 0xff, 0x00, 0xff};
            } else if(h <= (int)(1000 / 15.0)) {
                col = {0xff, 0x80, 0x00, 0xff};
            } else {
                col = {0xff, 0x00, 0x00, 0xff};
            }

            GPU_Line(target, WIDTH - frameTimeNum - 30 + i, HEIGHT - 10 - h, WIDTH - frameTimeNum - 30 + i, HEIGHT - 10, col);
            //SDL_RenderDrawLine(renderer, WIDTH - frameTimeNum - 30 + i, HEIGHT - 10 - h, WIDTH - frameTimeNum - 30 + i, HEIGHT - 10);
        }

        GPU_Line(target, WIDTH - 30 - frameTimeNum - 5, HEIGHT - 10 - (int)(1000.0 / fps), WIDTH - 25, HEIGHT - 10 - (int)(1000.0 / fps), {0x00, 0xff, 0xff, 0xff});
        GPU_Line(target, WIDTH - 30 - frameTimeNum - 5, HEIGHT - 10 - (int)(1000.0 / feelsLikeFps), WIDTH - 25, HEIGHT - 10 - (int)(1000.0 / feelsLikeFps), {0xff, 0x00, 0xff, 0xff});

        EASY_END_BLOCK; // draw frame graph
    }

    GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);

    EASY_BLOCK("draw version info", RENDER_PROFILER_COLOR);
    #ifdef DEVELOPMENT_BUILD
    if(dt_versionInfo1.w == -1) {
        char buffDevBuild[40];
        snprintf(buffDevBuild, sizeof(buffDevBuild), "Development Build");
        if(dt_versionInfo1.t1 != nullptr) GPU_FreeImage(dt_versionInfo1.t1);
        if(dt_versionInfo1.t2 != nullptr) GPU_FreeImage(dt_versionInfo1.t2);
        dt_versionInfo1 = Drawing::drawTextParams(target, buffDevBuild, font16, 4, HEIGHT - 32 - 13, 0xff, 0xff, 0xff, ALIGN_LEFT);

        char buffVersion[40];
        snprintf(buffVersion, sizeof(buffVersion), "Version %s - dev", VERSION);
        if(dt_versionInfo2.t1 != nullptr) GPU_FreeImage(dt_versionInfo2.t1);
        if(dt_versionInfo2.t2 != nullptr) GPU_FreeImage(dt_versionInfo2.t2);
        dt_versionInfo2 = Drawing::drawTextParams(target, buffVersion, font16, 4, HEIGHT - 32, 0xff, 0xff, 0xff, ALIGN_LEFT);

        char buffBuildDate[40];
        snprintf(buffBuildDate, sizeof(buffBuildDate), "%s : %s", __DATE__, __TIME__);
        if(dt_versionInfo3.t1 != nullptr) GPU_FreeImage(dt_versionInfo3.t1);
        if(dt_versionInfo3.t2 != nullptr) GPU_FreeImage(dt_versionInfo3.t2);
        dt_versionInfo3 = Drawing::drawTextParams(target, buffBuildDate, font16, 4, HEIGHT - 32 + 13, 0xff, 0xff, 0xff, ALIGN_LEFT);
    }

    Drawing::drawText(target, dt_versionInfo1, 4, HEIGHT - 32 - 13, ALIGN_LEFT);
    Drawing::drawText(target, dt_versionInfo2, 4, HEIGHT - 32, ALIGN_LEFT);
    Drawing::drawText(target, dt_versionInfo3, 4, HEIGHT - 32 + 13, ALIGN_LEFT);
    #elif defined ALPHA_BUILD
    char buffDevBuild[40];
    snprintf(buffDevBuild, sizeof(buffDevBuild), "Alpha Build");
    Drawing::drawText(target, buffDevBuild, font16, 4, HEIGHT - 32, 0xff, 0xff, 0xff, ALIGN_LEFT);

    char buffVersion[40];
    snprintf(buffVersion, sizeof(buffVersion), "Version %s - alpha", VERSION);
    Drawing::drawText(target, buffVersion, font16, 4, HEIGHT - 32 + 13, 0xff, 0xff, 0xff, ALIGN_LEFT);
    #else
    char buffVersion[40];
    snprintf(buffVersion, sizeof(buffVersion), "Version %s", VERSION);
    Drawing::drawText(target, buffVersion, font16, 4, HEIGHT - 32 + 13, 0xff, 0xff, 0xff, ALIGN_LEFT);
    #endif
    EASY_END_BLOCK; // draw version info
}

void Game::renderTemperatureMap(World* world) {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    for(int x = 0; x < world->width; x++) {
        for(int y = 0; y < world->height; y++) {
            auto t = world->tiles[x + y * world->width];
            int32_t temp = t.temperature;
            Uint32 color = (Uint8)((temp + 1024) / 2048.0f * 255);

            const unsigned int offset = (world->width * 4 * y) + x * 4;
            pixelsTemp_ar[offset + 0] = color;        // b
            pixelsTemp_ar[offset + 1] = color;        // g
            pixelsTemp_ar[offset + 2] = color;        // r
            pixelsTemp_ar[offset + 3] = 0xf0;    // a
        }
    }

}

int Game::getAimSurface(int dist) {
    int dcx = this->mx - WIDTH / 2;
    int dcy = this->my - HEIGHT / 2;

    float len = sqrtf(dcx * dcx + dcy * dcy);
    float udx = dcx / len;
    float udy = dcy / len;

    int mmx = WIDTH / 2.0f + udx * dist;
    int mmy = HEIGHT / 2.0f + udy * dist;

    int wcx = (int)((WIDTH / 2.0f - ofsX - camX) / scale);
    int wcy = (int)((HEIGHT / 2.0f - ofsY - camY) / scale);

    int wmx = (int)((mmx - ofsX - camX) / scale);
    int wmy = (int)((mmy - ofsY - camY) / scale);

    int startInd = -1;
    world->forLine(wcx, wcy, wmx, wmy, [&](int ind) {
        if(world->tiles[ind].mat->physicsType == PhysicsType::SOLID || world->tiles[ind].mat->physicsType == PhysicsType::SAND || world->tiles[ind].mat->physicsType == PhysicsType::SOUP) {
            startInd = ind;
            return true;
        }
        return false;
    });

    return startInd;
}

void Game::quitToMainMenu() {
    EASY_FUNCTION(GAME_PROFILER_COLOR);

    if(state == LOADING) return;

    std::string pref = "Saved in: ";

    std::string worldName = "mainMenu";
    char* wn = (char*)worldName.c_str();

    logInfo("Loading main menu @ {}", gameDir.getWorldPath(wn));
    MainMenuUI::visible = false;
    state = LOADING;
    stateAfterLoad = MAIN_MENU;

    EASY_BLOCK("Close world");
    delete world;
    world = nullptr;
    EASY_END_BLOCK;

    WorldGenerator* generator = new MaterialTestGenerator();

    std::string wpStr = gameDir.getWorldPath(wn);

    EASY_BLOCK("Load world");
    world = new World();
    world->noSaveLoad = true;
    world->init(wpStr, (int)ceil(Game::MAX_WIDTH / 3 / (double)CHUNK_W) * CHUNK_W + CHUNK_W * 3, (int)ceil(Game::MAX_HEIGHT / 3 / (double)CHUNK_H) * CHUNK_H + CHUNK_H * 3, target, &audioEngine, networkMode, generator);

    EASY_BLOCK("Queue chunk loading");
    logInfo("Queueing chunk loading...");
    for(int x = -CHUNK_W * 4; x < world->width + CHUNK_W * 4; x += CHUNK_W) {
        for(int y = -CHUNK_H * 3; y < world->height + CHUNK_H * 8; y += CHUNK_H) {
            world->queueLoadChunk(x / CHUNK_W, y / CHUNK_H, true, true);
        }
    }
    EASY_END_BLOCK;
    EASY_END_BLOCK;

    std::fill(pixels.begin(), pixels.end(), 0);
    std::fill(pixelsBackground.begin(), pixelsBackground.end(), 0);
    std::fill(pixelsLayer2.begin(), pixelsLayer2.end(), 0);
    std::fill(pixelsFire.begin(), pixelsFire.end(), 0);
    std::fill(pixelsFlow.begin(), pixelsFlow.end(), 0);
    std::fill(pixelsEmission.begin(), pixelsEmission.end(), 0);
    std::fill(pixelsParticles.begin(), pixelsParticles.end(), 0);

    GPU_UpdateImageBytes(
        texture,
        NULL,
        &pixels[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        textureBackground,
        NULL,
        &pixelsBackground[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        textureLayer2,
        NULL,
        &pixelsLayer2[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        textureFire,
        NULL,
        &pixelsFire[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        textureFlow,
        NULL,
        &pixelsFlow[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        emissionTexture,
        NULL,
        &pixelsEmission[0],
        world->width * 4
    );

    GPU_UpdateImageBytes(
        textureParticles,
        NULL,
        &pixelsParticles[0],
        world->width * 4
    );



    MainMenuUI::visible = true;

    #if BUILD_WITH_DISCORD
    DiscordIntegration::setStart(0);
    DiscordIntegration::setActivityState("On Main Menu");
    DiscordIntegration::flushActivity();
    #endif
}

int Game::getAimSolidSurface(int dist) {
    int dcx = this->mx - WIDTH / 2;
    int dcy = this->my - HEIGHT / 2;

    float len = sqrtf(dcx * dcx + dcy * dcy);
    float udx = dcx / len;
    float udy = dcy / len;

    int mmx = WIDTH / 2.0f + udx * dist;
    int mmy = HEIGHT / 2.0f + udy * dist;

    int wcx = (int)((WIDTH / 2.0f - ofsX - camX) / scale);
    int wcy = (int)((HEIGHT / 2.0f - ofsY - camY) / scale);

    int wmx = (int)((mmx - ofsX - camX) / scale);
    int wmy = (int)((mmy - ofsY - camY) / scale);

    int startInd = -1;
    world->forLine(wcx, wcy, wmx, wmy, [&](int ind) {
        if(world->tiles[ind].mat->physicsType == PhysicsType::SOLID) {
            startInd = ind;
            return true;
        }
        return false;
    });

    return startInd;
}

#if BUILD_WITH_STEAM
extern "C" void __cdecl SteamAPIDebugTextHook(int nSeverity, const char *pchDebugText) {
    ::OutputDebugString(pchDebugText);
    if(nSeverity >= 1) {
        logWarn("[STEAMAPI] {}", pchDebugText);
        // place to set a breakpoint for catching API errors
        int x = 3;
        x = x;
    } else {
        logInfo("[STEAMAPI] {}", pchDebugText);
    }
}
#endif

#if BUILD_WITH_STEAM
void Game::SteamHookMessages() {
    SteamUtils()->SetWarningMessageHook(&SteamAPIDebugTextHook);
}
#endif
