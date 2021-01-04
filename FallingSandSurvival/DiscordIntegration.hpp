#pragma once

class DiscordIntegration {
    #if BUILD_WITH_DISCORD
    static discord::Core* core;
    static discord::Activity fullActivity;
    #endif

public:
    static bool discordAPI;

    #if BUILD_WITH_DISCORD
    static bool showDetails;
    static bool showPlaytime;

    static bool init();
    static void shutdown();
    static void tick();

    static void flushActivity();

    static void setActivityDetails(std::string str) {
        fullActivity.SetDetails(str.c_str());
    }

    static void setActivityState(std::string str) {
        fullActivity.SetState(str.c_str());
    }

    static void setStart(long long time) {
        fullActivity.GetTimestamps().SetStart(time);
    }

    static void setEnd(long long time) {
        fullActivity.GetTimestamps().SetEnd(time);
    }

    #endif
};
