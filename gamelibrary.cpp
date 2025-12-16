#include "gamelibrary.h"
#include <string>
#include <windows.h>
#include <QProcess>
#include <QFileInfo>

// Class to hold the game database utility
// Creates, adds, returns, and deletes from the table


GameLibrary::GameLibrary(const std::string& dbPath)
    : db(dbPath, SQLite::OPEN_READWRITE|SQLite::OPEN_CREATE)
{
    createTables();
}

// Creates the table if it doesnt exist
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

// Adds a game via the manual import method
void GameLibrary::addGame(const std::string& name,
                          const std::string& directory,
                          const std::string& description,
                          const std::string& imagePath)
{
    SQLite::Statement query(
        db,
        "INSERT OR REPLACE INTO games "
        "(name, directory, description, image_path) "
        "VALUES (?, ?, ?, ?)"
        );

    query.bind(1, name);
    query.bind(2, directory);
    query.bind(3, description);
    query.bind(4, imagePath);
    query.exec();
}


// Adds the game via the automatic steam import method
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

// removes a game via its unique id
void GameLibrary::removeGameById(int id) {
    SQLite::Statement query(db, "DELETE FROM games WHERE id = ?");
    query.bind(1, id);
    query.exec();
}

// launches a steam game, uses appId to launch
bool GameLibrary::launchGameById(const std::string& appId) {
    if (appId.empty()) return false;
    std::string url = "steam://rungameid/" + appId;
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
}

// launches a manually imported game via its path
bool GameLibrary::launchGameByPath(const std::string& filePath)
{
    if (filePath.empty())
        return false;

    QString exePath = QString::fromStdString(filePath);
    QFileInfo info(exePath);

    if (!info.exists() || !info.isFile())
        return false;

    // Use the executable's directory as working dir
    QString workingDir = info.absolutePath();

    return QProcess::startDetached(
        exePath,
        QStringList(),
        workingDir
        );
}



// Toggles the favorited from 1 to 0: 1 is favorited, 0 is unfavorited
void GameLibrary::toggleFavorite(const int id) {
    SQLite::Statement query(db, "UPDATE games SET favorited = 1 - favorited WHERE id = ?");
    query.bind(1, id);
    query.exec();
}

// Returns favorited games
std::vector<Game> GameLibrary::getFavoriteGames() {
    std::vector<Game> games;
    SQLite::Statement query(db, "SELECT id, name, description, directory, image_path, steam_appid, playtime, last_opened FROM games WHERE favorited = 1");
    while (query.executeStep()) {
        Game game; // Assigns the database information to its own struct
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

// Returns every game name
QStringList GameLibrary::returnNames() {
    QStringList gameNames;
    SQLite::Statement query(db, "SELECT name FROM games");

    while (query.executeStep()) {
        std::string name = query.getColumn(0).getString();
        gameNames.append(QString::fromStdString(name));
    }
    return gameNames;
};

// Returns every SteamAppId
QStringList GameLibrary::returnSteamAppIds() {
    QStringList gameSteamAppIds;
    SQLite::Statement query(db, "SELECT steam_appid FROM games");

    while (query.executeStep()) {
        std::string name = query.getColumn(0).getString();
        gameSteamAppIds.append(QString::fromStdString(name));
    }
    return gameSteamAppIds;
};

// Returns if a game is favorited
bool GameLibrary::isFavorited(int id) {
    SQLite::Statement query(db, "SELECT favorited FROM games WHERE id = ?");
    query.bind(1, id);
    if (query.executeStep())
        return query.getColumn(0).getInt() == 1;
    return false;
}

// Gets the local image path via its appId
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
