#pragma once

#define DEVELOPMENT_BUILD
#define ALPHA_BUILD

#define VERSION "0.1.0"

#define BUILD_WITH_STEAM 1
#define BUILD_WITH_DISCORD 1

#if BUILD_WITH_STEAM
#define STEAM_APPID 1284340
#endif

#if BUILD_WITH_DISCORD
#define DISCORD_CLIENTID 699396433320607844
#endif

#include <future>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm> 
#include <unordered_map>
#include <vector>
#include <deque>
#include <iterator>
#include <filesystem>
#include <regex> 
#include <memory>

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL_gpu.h"

#include "box2d/b2_math.h"
#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_shape.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_distance_joint.h"

#include "lib/AudioAdventure/include/AudioEngine.h"

#if BUILD_WITH_STEAM
#pragma comment(lib, "lib/steam/redistributable_bin/steam_api.lib")
#include "steam_api.h"
#endif

#if BUILD_WITH_DISCORD
#pragma comment(lib, "lib/discord_game_sdk/lib/x86/discord_game_sdk.dll.lib")
#include "discord.h"
#endif

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"

#include "lib/polypartition-master/src/polypartition.h"

#include "lib/FastNoiseSIMD/FastNoiseSIMD.h"
#include "lib/FastNoise/FastNoise.h"
#include "lib/sparsehash/dense_hash_map.h"


#include "enet/enet.h"
#undef min
#undef max
#undef SendMessage
#undef ERROR

#include "spdlog/common.h"
#undef SPDLOG_ACTIVE_LEVEL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON
#include "spdlog/spdlog.h"

#define logTrace SPDLOG_TRACE
#define logDebug SPDLOG_DEBUG
#define logInfo SPDLOG_INFO
#define logWarn SPDLOG_WARN
#define logError SPDLOG_ERROR
#define logCritical SPDLOG_CRITICAL
