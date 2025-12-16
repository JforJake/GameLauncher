#ifndef EPICSCANNER_H
#define EPICSCANNER_H

#include "gamelibrary.h"
#include "steamfetcher.h"
#include <QString>

class EpicScanner
{
public:
    explicit EpicScanner(GameLibrary &library);

private:
    void scanManifests(const QString &manifestDir);
    GameLibrary &mGameLibrary;
};

#endif // EPICSCANNER_H
