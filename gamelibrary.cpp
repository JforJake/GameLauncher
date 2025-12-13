#include "gamelibrary.h"
#include <string>
#include <windows.h>
#include <QProcess>

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
            "description TEXT, "
            "directory TEXT, "
            "image_path TEXT, "
            "steam_appid BIGINT UNIQUE,"
            "playtime INTEGER DEFAULT 0,"
            "last_opened TEXT, "
            "favorited INTEGER DEFAULT 0)");
}

void GameLibrary::addGame(const std::string& name, const std::string& directory) {
    GameMetadata metadata = steamFetcher.fetchGameData(name);

    SQLite::Statement query(db, "INSERT OR REPLACE INTO games (name, directory, description, image_path) VALUES (?, ?, ?, ?)");
    query.bind(1, name);
    query.bind(2, directory);
    query.bind(3, metadata.description);
    query.bind(4, metadata.imagePath);
    query.exec();
}

// Adds the game to the database
void GameLibrary::addSteamGame(long long appId, const std::string& name, const std::string& directory) {

    GameMetadata metadata = steamFetcher.fetchBySteamId(appId);

    SQLite::Statement query(db, "INSERT OR REPLACE INTO games (name, directory, steam_appid, description, image_path) VALUES (?, ?, ?, ?, ?)");
    query.bind(1, name);
    query.bind(2, directory);
    query.bind(3, appId);
    query.bind(4, metadata.description);
    query.bind(5, metadata.imagePath);
    query.exec();
}

// returns all games currently in the database
std::vector<Game> GameLibrary::getAllGames() {
    std::vector<Game> games;
    SQLite::Statement query(db, "SELECT id, name, description, directory, image_path, steam_appid, playtime, last_opened FROM games");

    while (query.executeStep()) {
        Game game;
        game.id = query.getColumn(0).getInt();
        game.name = query.getColumn(1).getString();
        game.description = query.getColumn(2).isNull() ? "" : query.getColumn(2).getString();
        game.directory = query.getColumn(3).isNull() ? "" : query.getColumn(3).getString();
        game.imagePath = query.getColumn(4).isNull() ? "" : query.getColumn(4).getString();
        game.steamAppId = query.getColumn(5).isNull() ? 0 : query.getColumn(5).getInt64();
        game.playtime = query.getColumn(6).getInt();
        game.lastOpened = query.getColumn(7).isNull() ? "" : query.getColumn(7).getString();
        games.push_back(game);
    }
    return games;
}

void GameLibrary::removeGameById(int id) {
    SQLite::Statement query(db, "DELETE FROM games WHERE id = ?");
    query.bind(1, id);
    query.exec();
}

// launches a steam game, uses appId to launch. will do from .exe later
bool GameLibrary::launchGameById(const std::string& appId) {
    if (appId.empty()) return false;
    std::string url = "steam://rungameid/" + appId;
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
}

bool GameLibrary::launchGameByPath(const std::string& filePath) {
    if (filePath.empty()) return false;
    return QProcess::startDetached(QString::fromStdString(filePath));
}

void GameLibrary::toggleFavorite(const int id) {
    SQLite::Statement query(db, "UPDATE games SET favorited = 1 - favorited WHERE id = ?");
    query.bind(1, id);
    query.exec();
}


std::vector<Game> GameLibrary::getFavoriteGames() {
    std::vector<Game> games;
    SQLite::Statement query(db, "SELECT id, name, description, directory, image_path, steam_appid, playtime, last_opened FROM games WHERE favorited = 1");
    while (query.executeStep()) {
        Game game;
        game.id = query.getColumn(0).getInt();
        game.name = query.getColumn(1).getString();
        game.description = query.getColumn(2).isNull() ? "" : query.getColumn(2).getString();
        game.directory = query.getColumn(3).isNull() ? "" : query.getColumn(3).getString();
        game.imagePath = query.getColumn(4).isNull() ? "" : query.getColumn(4).getString();
        game.steamAppId = query.getColumn(5).isNull() ? 0 : query.getColumn(5).getInt64();
        game.playtime = query.getColumn(6).getInt();
        game.lastOpened = query.getColumn(7).isNull() ? "" : query.getColumn(7).getString();
        game.favorited = true;
        games.push_back(game);
    }
    return games;
}

QStringList GameLibrary::returnNames() {
    QStringList gameNames;
    SQLite::Statement query(db, "SELECT name FROM games");

    while (query.executeStep()) {
        std::string name = query.getColumn(0).getString();
        gameNames.append(QString::fromStdString(name));
    }
    return gameNames;
};

QStringList GameLibrary::returnSteamAppIds() {
    QStringList gameSteamAppIds;
    SQLite::Statement query(db, "SELECT steam_appid FROM games");

    while (query.executeStep()) {
        std::string name = query.getColumn(0).getString();
        gameSteamAppIds.append(QString::fromStdString(name));
    }
    return gameSteamAppIds;
};

bool GameLibrary::isFavorited(int id) {
    SQLite::Statement query(db, "SELECT favorited FROM games WHERE id = ?");
    query.bind(1, id);
    if (query.executeStep())
        return query.getColumn(0).getInt() == 1;
    return false;
}

QString GameLibrary::getImagePathForSteamApp(qulonglong appId)
{
    SQLite::Statement query(
        db,
        "SELECT image_path FROM games WHERE steam_appid = ?"
        );
    query.bind(1, static_cast<long long>(appId));

    if (query.executeStep()) {
        return QString::fromStdString(
            query.getColumn(0).getString()
            );
    }
    return "";
}
