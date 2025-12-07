#ifndef STEAMFETCHER_H
#define STEAMFETCHER_H
#include <string>

struct GameMetadata {
    std::string name;
    std::string description;
    std::string imagePath;
};

class SteamFetcher
{
public:
    SteamFetcher();
    // Fetch by game name
    GameMetadata fetchGameData(const std::string& gameName);
    // Fetch by SteamAppId
    GameMetadata fetchBySteamId(int steamAppId);
};

#endif
