#include "steamLibrary.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <fstream>

steamLibrary::steamLibrary() {
    if (!findSteamPath()) {
        return;
    }

    try {
        appIds = parseApps(steamPath);
    } catch (const std::exception& e) {
        std::cerr << "Error parsing apps: " << e.what() << std::endl;
    }

}

bool steamLibrary::findSteamPath() {
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

std::vector<std::string> steamLibrary::parseApps(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + filename);
    }
    std::vector<std::string> appIds;
    std::string line;
    bool inApps = false;
    int braceDepth = 0;

    while (std::getline(file, line)) {
        line = trim(line);

        if (line == "{") {
            braceDepth++;
        } else if (line == "}") {
            braceDepth--;

            if (inApps && braceDepth == 2) {
                inApps = false;
            }
        }

        if (line.find("\"apps\"") != std::string::npos) {
            inApps = true;
            continue;
        }

        // App IDs are at depth 3 {}
        if (inApps && braceDepth == 3) {
            std::string appId = parseAppId(line);
            if (!appId.empty() && appId != "228980") {
                appIds.push_back(appId);
            }
        }
    }

    return appIds;

}

std::string steamLibrary::parseAppId(const std::string& str) {
    size_t firstQuote = str.find('"');
    if (firstQuote == std::string::npos) return "";

    size_t secondQuote = str.find('"', firstQuote + 1);
    if (secondQuote == std::string::npos) return "";

    return str.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

std::string steamLibrary::trim(const std::string& str) {
    const char* whitespace = " \t\n\r\f\v";
    size_t begin = str.find_first_not_of(whitespace);
    if (begin == std::string::npos) {
        return std::string{};
    }
    size_t end = str.find_last_not_of(whitespace);
    return std::string{str.substr(begin, end - begin + 1)};
}

bool steamLibrary::launchGame(const std::string& appId) {
    if (appId.empty()) return false;
    std::string url = "steam://rungameid/" + appId;
    std::cout << url << std::endl;
    auto result = (INT_PTR)ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    return true;
}

const std::vector<std::string>& steamLibrary::getAppIds() const {
    return appIds;
}
