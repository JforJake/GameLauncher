#ifndef STEAMSCANNER_H
#define STEAMSCANNER_H

#include <string>
#include <vector>
#include "gamelibrary.h"

class steamScanner
{
public:
    steamScanner(GameLibrary &library);
private:
    bool findSteamPath();
    std::vector<std::string> parseLocations(const std::string& filename);
    std::vector<std::string> collectManifests(std::vector<std::string> manifests);
    void loadFromManifests(std::vector<std::string> entry);
    GameLibrary& gameLib;
    std::string removeBackSlash(std::string toBeRemoved);
};

#endif // STEAMSCANNER_H
