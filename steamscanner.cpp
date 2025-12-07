#include "steamscanner.h"
#include <windows.h>
#include <vector>
#include <fstream>
#include <filesystem>

std::string steamPath;

steamScanner::steamScanner(GameLibrary& library) : gameLib(library) {
    if (!findSteamPath()) {
        return;
    }

    std::vector<std::string> libraries = parseLocations(steamPath);
    std::vector<std::string> manifests = collectManifests(libraries);
    loadFromManifests(manifests);
}

// gets the steam path just to find the vdf file.
bool steamScanner::findSteamPath() {
    char buffer[512] = {0};
    DWORD bufferSize = sizeof(buffer);

    LONG result = RegGetValueA(
        HKEY_CURRENT_USER,
        "Software\\Valve\\Steam",
        "SteamPath",
        RRF_RT_REG_SZ,
        nullptr,
        buffer,
        &bufferSize
        );

    if (result != ERROR_SUCCESS) {
        return false;
    }
    steamPath = buffer;
    steamPath += "/steamapps/libraryfolders.vdf";

    return true;
}

// saves the library locations into a vector from the vdf
std::vector<std::string> steamScanner::parseLocations(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }
    std::vector<std::string> locations;
    std::string line;

    while (std::getline(file, line)) {
        if (line.find("\"path\"") != std::string::npos) {
            size_t lastQuote = line.rfind('"');
            size_t firstQuote = line.rfind('"', lastQuote -1);
            std::string path = line.substr(firstQuote +1, lastQuote - firstQuote - 1);

            path = removeBackSlash(path);

            path += "/steamapps";
            locations.push_back(path);
        }
    }

    return locations;
}

// collects every appManifest in the library locations and saves them as the full paths
std::vector<std::string> steamScanner::collectManifests(std::vector<std::string> locations) {
    std::vector<std::string> collectedManifests;

    for (const auto& library : locations) {

        for (const auto& entry : std::filesystem::directory_iterator(library)) {
            std::string manifestName = entry.path().filename().string();

            if (manifestName == "appmanifest_228980.acf"){
                continue;
            }

            if(manifestName.find(".acf") == manifestName.size() - 4) {

                std::string manifestEntry = entry.path().string();

                manifestEntry = removeBackSlash(manifestEntry);

                collectedManifests.push_back(manifestEntry);
            }
        }
    }

    return collectedManifests;
}

// loads game information from the vector of appManifests
void steamScanner::loadFromManifests(std::vector<std::string> entries) {
    for (const auto& entry : entries) {
        long long appId;
        std::string name;
        std::string directory;

        std::ifstream file(entry);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file " + entry);
        }
        std::string line;

        while (std::getline(file, line)) {
            if (line.find("\"appid\"") != std::string::npos){
                size_t lastQuote = line.rfind('"');
                size_t firstQuote = line.rfind('"', lastQuote -1);
                appId = std::stoll(line.substr(firstQuote +1, lastQuote - firstQuote - 1));
            }

            if (line.find("\"name\"") != std::string::npos){
                size_t lastQuote = line.rfind('"');
                size_t firstQuote = line.rfind('"', lastQuote -1);
                name = line.substr(firstQuote +1, lastQuote - firstQuote - 1);
            }

            if (line.find("\"installdir\"") != std::string::npos){
                size_t lastQuote = line.rfind('"');
                size_t firstQuote = line.rfind('"', lastQuote -1);
                std::string ending = line.substr(firstQuote +1, lastQuote - firstQuote - 1);
                size_t lastSlash = entry.rfind('/');
                directory = entry.substr(0, lastSlash + 1);
                directory += "common/" + ending;
            }
        }

        gameLib.addSteamGame(appId, name, directory);

    }
}

// removes backslashes and turns them into forwardslashes to turn them into readable directories
std::string steamScanner::removeBackSlash(std::string toBeRemoved) {
    for (size_t i = 0; i < toBeRemoved.length() -1; i++) {
        if (toBeRemoved[i] == '\\' && toBeRemoved[i+1] == '\\') {
            toBeRemoved.replace(i, 2, "/");
        }
    }

    for (size_t i = 0; i < toBeRemoved.length() -1; i++) {
        if (toBeRemoved[i] == '\\') {
            toBeRemoved.replace(i, 1, "/");
        }
    }

    return toBeRemoved;
}
