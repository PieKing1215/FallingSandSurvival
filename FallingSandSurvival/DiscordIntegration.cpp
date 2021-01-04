#include "DiscordIntegration.hpp"
#include "DiscordUtils.hpp"

#include "ProfilerConfig.hpp"


#define BUILD_WITH_EASY_PROFILER
#include <easy/profiler.h>

bool DiscordIntegration::discordAPI = false;

#if BUILD_WITH_DISCORD
discord::Core* DiscordIntegration::core {};
discord::Activity DiscordIntegration::fullActivity {};

bool DiscordIntegration::showDetails = true;
bool DiscordIntegration::showPlaytime = true;

bool DiscordIntegration::init() {
    EASY_FUNCTION(DISCORD_PROFILER_COLOR);

    EASY_BLOCK("discord::Core::Create", DISCORD_PROFILER_COLOR);
    auto result = discord::Core::Create(DISCORD_CLIENTID, DiscordCreateFlags_NoRequireDiscord, &core);
    EASY_END_BLOCK;

    discordAPI = (result == discord::Result::Ok);

    if(discordAPI) {
        logInfo("discord::Core::Create successful.");
        EASY_BLOCK("SetLogHook", DISCORD_PROFILER_COLOR);
        core->SetLogHook(discord::LogLevel::Debug, [](auto level, const char* txt) {
            switch(level) {
            case discord::LogLevel::Info:
                logInfo("[DISCORD SDK] {}", txt);
                break;
            case discord::LogLevel::Debug:
                logDebug("[DISCORD SDK] {}", txt);
                break;
            case discord::LogLevel::Warn:
                logWarn("[DISCORD SDK] {}", txt);
                break;
            case discord::LogLevel::Error:
                logError("[DISCORD SDK] {}", txt);
                break;
            }
        });
        EASY_END_BLOCK;

        #if BUILD_WITH_STEAM
        core->ActivityManager().RegisterSteam(STEAM_APPID);
        #endif

        fullActivity.GetAssets().SetLargeImage("largeicon");
        fullActivity.GetAssets().SetLargeText("Falling Sand Survival");
        fullActivity.SetType(discord::ActivityType::Playing);

    } else {
        logError("discord::Core::Create failed.");
    }

    return discordAPI;
}

void DiscordIntegration::shutdown() {

    core->~Core();

    discordAPI = false;
}

void DiscordIntegration::tick() {
    EASY_FUNCTION(DISCORD_PROFILER_COLOR);

    if(discordAPI) {
        if(core->RunCallbacks() == discord::Result::NotRunning) {
            discordAPI = false;
        }
    }
}

void DiscordIntegration::flushActivity() {
    EASY_FUNCTION(DISCORD_PROFILER_COLOR);

    discord::Activity activity = discord::Activity(fullActivity);

    if(!showDetails) {
        activity.SetDetails("");
        activity.SetState("");
        activity.GetTimestamps().SetStart(0);
        activity.GetTimestamps().SetEnd(0);
    }

    if(!showPlaytime) {
        activity.GetTimestamps().SetStart(0);
        activity.GetTimestamps().SetEnd(0);
    }

    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        logInfo("[DISCORD] UpdateActivity returned: {} ({})", result, DiscordUtils::resultToString(result));
    });
}

#endif
