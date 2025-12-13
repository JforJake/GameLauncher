#ifndef GAMELIBRARY_H
#define GAMELIBRARY_H

#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include <vector>
#include <QString>
#include "steamfetcher.h"

struct Game {
    int id;
    std::string name;
    std::string description;
    std::string directory;
    std::string imagePath;
    long long steamAppId;
    int playtime;
    std::string lastOpened;
    int favorited = false;
};

class GameLibrary
{
public:
    GameLibrary(const std::string& dbPath);
    void addGame(const std::string& name, const std::string& directory);
    void addSteamGame(long long appId, const std::string& name, const std::string& directory);
    std::vector<Game> getAllGames();
    std::vector<Game> getFavoriteGames();
    QStringList returnNames();
    QStringList returnSteamAppIds();
    bool launchGameById(const std::string& appId);
    bool launchGameByPath(const std::string& filePath);
    void removeGameById(const int id);
    void toggleFavorite(const int id);
    bool isFavorited(const int id);
    QString getImagePathForSteamApp(qulonglong appId);

private:
    SQLite::Database db;
    void createTables();
    SteamFetcher steamFetcher;
};

#endif // GAMELIBRARY_H
