// steamfetcher.cpp
#include "steamfetcher.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

// Class to fetch data on a game based on its steamId or by its game name

SteamFetcher::SteamFetcher() {}

// Fetches data by steamAppId
GameMetadata SteamFetcher::fetchBySteamId(int steamAppId)
{
    GameMetadata result;


    QString appDir = QCoreApplication::applicationDirPath();
    QString coverDir = appDir + "/covers";
    QDir().mkpath(coverDir);

    QNetworkAccessManager manager;
    QString url = QString("https://store.steampowered.com/api/appdetails?appids=%1").arg(steamAppId);
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject appData = doc.object()[QString::number(steamAppId)].toObject();

        if (appData["success"].toBool()) {
            // fetches data from the json and saves it to GameMetadata
            QJsonObject data = appData["data"].toObject();

            result.name = data["name"].toString().toStdString();
            result.description = data["short_description"].toString().toStdString();

            QString imageUrl = QString("https://cdn.cloudflare.steamstatic.com/steam/apps/%1/library_600x900.jpg").arg(steamAppId);

            QNetworkReply *imgReply = manager.get(QNetworkRequest(QUrl(imageUrl)));
            QObject::connect(imgReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
            loop.exec();

            if (imgReply->error() == QNetworkReply::NoError) {
                // fetches the image
                QString safeName = QString::fromStdString(result.name).replace(" ", "_");
                QString savePath = QString(coverDir + "/%1.jpg").arg(safeName);
                QFile file(savePath);
                if (file.open(QIODevice::WriteOnly)) {
                    file.write(imgReply->readAll());
                    file.close();
                    result.imagePath = savePath.toStdString();
                }
            }
            imgReply->deleteLater();
        }
    }
    reply->deleteLater();

    return result;
}

// Fetches data by its name
GameMetadata SteamFetcher::fetchGameData(const std::string &gameName)
{
    QNetworkAccessManager manager;
    QString searchUrl = QString("https://steamcommunity.com/actions/SearchApps/%1").arg(QString::fromStdString(gameName));
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(searchUrl)));

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonArray results = doc.array();

        QString searchName = QString::fromStdString(gameName).toLower(); // searches by name
        // gets the SteamAppId to the exact match
        for (const QJsonValue &result : results) {
            QJsonObject game = result.toObject();
            QString gameName = game["name"].toString();
            // exact match
            if (gameName.toLower() == searchName) {
                QString appIdString = game["appid"].toString();
                int appId = appIdString.toInt();
                reply->deleteLater();
                return fetchBySteamId(appId);
            }
        }
        // If no exact match, use first result
        if (!results.isEmpty()) {
            QString appIdString = results[0].toObject()["appid"].toString();
            int appId = appIdString.toInt();
            reply->deleteLater();
            return fetchBySteamId(appId); // fetches by steamAppId
        }
    }

    reply->deleteLater();
    return GameMetadata();
}
