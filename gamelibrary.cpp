#include "gamelibrary.h"
#include <string>
#include <windows.h>

GameLibrary::GameLibrary(const std::string& dbPath)
    : db(dbPath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
{
    createTables();
}
void GameLibrary::createTables()
{
    db.exec("CREATE TABLE IF NOT EXISTS games ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT NOT NULL, "
            "directory TEXT, "
            "image_path TEXT, "
            "steam_appid BIGINT UNIQUE,"
            "playtime INTEGER DEFAULT 0,"
            "last_opened TEXT)");
}

// Adds the game to the database
void GameLibrary::addSteamGame(long long appId, const std::string& name, const std::string& directory) {
    SQLite::Statement query(db, "INSERT OR REPLACE INTO games (name, directory, steam_appid) VALUES (?, ?, ?)");
    query.bind(1, name);
    query.bind(2, directory);
    query.bind(3, appId);
    query.exec();
}

// returns all games currently in the database
std::vector<Game> GameLibrary::getAllGames() {
    std::vector<Game> games;
    SQLite::Statement query(db, "SELECT * FROM games");

    while (query.executeStep()) {
        Game game;
        game.id = query.getColumn(0).getInt();
        game.name = query.getColumn(1).getString();
        game.directory = query.getColumn(2).isNull() ? "" : query.getColumn(2).getString();
        game.imagePath = query.getColumn(3).isNull() ? "" : query.getColumn(3).getString();
        game.steamAppId = query.getColumn(4).isNull() ? 0 : query.getColumn(4).getInt64();
        game.playtime = query.getColumn(5).getInt();
        game.lastOpened = query.getColumn(6).isNull() ? "" : query.getColumn(6).getString();

        games.push_back(game);
    }

    return games;
}

QString GameLibrary::getGameDesc(long long appId) {
    QString gameDesc = "";
    try {
        SQLite::Statement query(db, "SELECT description FROM games WHERE steam_appid = ?");
        query.bind(1, appId);

        if (query.executeStep()) {
            gameDesc = QString::fromStdString(query.getColumn(0).getString());
        }

    } catch (std::exception& e) {

    }

    return gameDesc;
}

void GameLibrary::removeGameByAppId(long long appId) {
    SQLite::Statement query(db, "DELETE FROM games WHERE steam_appid = ?");
    query.bind(1, appId);
    query.exec();
}

// launches a steam game, uses appId to launch. will do from .exe later
bool GameLibrary::launchGame(const std::string& appId) {
    if (appId.empty()) return false;
    std::string url = "steam://rungameid/" + appId;
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
}
