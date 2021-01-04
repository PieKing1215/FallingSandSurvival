#pragma once

class DiscordUtils {

public:
    #if BUILD_WITH_DISCORD
    static std::string resultToString(discord::Result res) {
        static std::vector<std::string> codes = {
            "Ok",
            "ServiceUnavailable",
            "InvalidVersion",
            "LockFailed",
            "InternalError",
            "InvalidPayload",
            "InvalidCommand",
            "InvalidPermissions",
            "NotFetched",
            "NotFound",
            "Conflict",
            "InvalidSecret",
            "InvalidJoinSecret",
            "NoEligibleActivity",
            "InvalidInvite",
            "NotAuthenticated",
            "InvalidAccessToken",
            "ApplicationMismatch",
            "InvalidDataUrl",
            "InvalidBase64",
            "NotFiltered",
            "LobbyFull",
            "InvalidLobbySecret",
            "InvalidFilename",
            "InvalidFileSize",
            "InvalidEntitlement",
            "NotInstalled",
            "NotRunning",
            "InsufficientBuffer",
            "PurchaseCancelled",
            "InvalidGuild",
            "InvalidEvent",
            "InvalidChannel",
            "InvalidOrigin",
            "RateLimited",
            "OAuth2Error",
            "SelectChannelTimeout",
            "GetGuildTimeout",
            "SelectVoiceForceRequired",
            "CaptureShortcutAlreadyListening",
            "UnauthorizedForAchievement",
            "InvalidGiftCode",
            "PurchaseError",
            "TransactionAborted"
        };

        return codes[(int)res];
    }
    #endif

};
