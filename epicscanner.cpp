#include "EPICSCANNER.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

EpicScanner::EpicScanner(GameLibrary &library)
    : mGameLibrary(library)
{
    const QString manifestPath =
        "C:/ProgramData/Epic/EpicGamesLauncher/Data/Manifests";

    scanManifests(manifestPath);
}

void EpicScanner::scanManifests(const QString &manifestDir)
{
    QDir dir(manifestDir);
    if (!dir.exists())
        return;

    SteamFetcher fetcher;

    const QStringList files =
        dir.entryList(QStringList() << "*.item", QDir::Files);

    for (const QString &fileName : files) {
        QFile file(dir.filePath(fileName));
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!doc.isObject())
            continue;

        QJsonObject obj = doc.object();

        const QString name = obj.value("DisplayName").toString();
        const QString installLocation = obj.value("InstallLocation").toString();
        const QString launchExe = obj.value("LaunchExecutable").toString();

        if (name.isEmpty() || installLocation.isEmpty() || launchExe.isEmpty())
            continue;

        QString fullExePath = installLocation;
        if (!fullExePath.endsWith("/") && !fullExePath.endsWith("\\"))
            fullExePath += "/";
        fullExePath += launchExe;

        // 🔹 Fetch metadata via SteamFetcher
        GameMetadata meta =
            fetcher.fetchGameData(name.toStdString());

        mGameLibrary.addGame(
            name.toStdString(),
            fullExePath.toStdString(),
            meta.description,
            meta.imagePath
            );
    }
}
