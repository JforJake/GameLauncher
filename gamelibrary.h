#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>



struct Game {
    int id;
    std::string name;
    std::string directory;
    std::string imagePath;
    long long steamAppId;
    int playtime;
    std::string lastOpened;
};

class GameLibrary
{
public:
    GameLibrary(const std::string& dbPath);
    void addSteamGame(long long appId, const std::string& name, const std::string& directory);
    std::vector<Game> getAllGames();
    bool launchGame(const std::string& appId);

private:
    SQLite::Database db;
    void createTables();
};

#endif // GAMELIBRARY_H
