#ifndef STEAMLIBRARY_H
#define STEAMLIBRARY_H

#include <string>
#include <vector>

class steamLibrary
{
public:
    steamLibrary();
    const std::vector<std::string>& getAppIds() const;
    bool launchGame(const std::string& appId);
    std::string getDescription;
private:
    std::string steamPath;
    bool findSteamPath();
    std::vector<std::string> appIds;
    std::vector<std::string> parseApps(const std::string& filename);
    std::string trim(const std::string& str);
    std::string parseAppId(const std::string& str);
};

#endif
