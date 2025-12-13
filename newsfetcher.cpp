#include "newsfetcher.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QUrlQuery>

NewsFetcher::NewsFetcher(QObject *parent) {
    networkManager = new QNetworkAccessManager(parent);
    connect(networkManager, &QNetworkAccessManager::finished, this, &NewsFetcher::onNewsReceived);

    loadGameNames();
}



void NewsFetcher::loadGameNames() {
    gameNames.clear();
    QString appDir = QCoreApplication::applicationDirPath();
    gameLibrary = new GameLibrary((appDir + "/games.db").toStdString());

    gameNames = gameLibrary->returnNames();
    gameAppIds = gameLibrary->returnSteamAppIds();
}



// Gets
QString NewsFetcher::getTopArticleName() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().title;
    return "";
}

QString NewsFetcher::getTopArticleText() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().contents;
    return "";
}

QString NewsFetcher::getTopArticleThumbnail() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().thumbnailUrl;
    return "";
}

QList<NewsItem> NewsFetcher::getAllNewsItems() {
    return allNewsItems;
}

// fetching news functions
void NewsFetcher::fetchAllNews() {
    fetchSteamNews(gameAppIds);
}

void NewsFetcher::fetchSteamNews(QStringList AppIds) {
    pendingRequests = 0;
    for (const QString& appId : AppIds) {
        if (appId.isEmpty()) {
            continue;
        }

        QUrl url("https://api.steampowered.com/ISteamNews/GetNewsForApp/v2/");
        QUrlQuery query;
        query.addQueryItem("appid", appId);
        query.addQueryItem("count", "2");
        query.addQueryItem("maxlength", "300");
        url.setQuery(query);

        QNetworkRequest request(url);
        request.setAttribute(QNetworkRequest::User, appId);

        networkManager->get(request);
        pendingRequests++;
    }
    if (pendingRequests == 0) {
        emit newsReady();
    }
}

void NewsFetcher::sortNews() {
    qint64 oneMonthAgoSecs = QDateTime::currentSecsSinceEpoch() - 30*24*60*60; // 30 days
    for (int i = allNewsItems.size() - 1; i >= 0; --i) {    // removes any older than 30 days
        if (allNewsItems[i].timestamp < oneMonthAgoSecs) {
            allNewsItems.removeAt(i);
        }
    }

    std::sort(allNewsItems.begin(), allNewsItems.end(), // sorts list
              [](const NewsItem &a, const NewsItem &b) {
                  return a.timestamp > b.timestamp;
              });
}

void NewsFetcher::onNewsReceived(QNetworkReply *reply) {
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Network error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    QString appId = reply->request().attribute(QNetworkRequest::User).toString();
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject root = doc.object();

    if (root.contains("appnews")) {
        QJsonObject appnews = root["appnews"].toObject();

        if (!appnews.contains("newsitems") || !appnews["newsitems"].isArray()) {
            reply->deleteLater();
            return;
        }

        QJsonArray newsItems = appnews["newsitems"].toArray();


        for (const QJsonValue &value : newsItems) {
            QJsonObject item = value.toObject();

            QString title = item["title"].toString();
            QString url = item["url"].toString();
            QString contents = item["contents"].toString();
            int timestamp = item["date"].toInt();
            QString date = QDateTime::fromSecsSinceEpoch(timestamp).toString("MMM dd, yyyy");

            if (contents.isNull()) contents = "";
            contents.replace(QRegularExpression("<[^>]*>"), "");

            QString thumbnailUrl = QString("https://cdn.cloudflare.steamstatic.com/steam/apps/%1/header.jpg").arg(appId);

            // Store news item with timestamp
            NewsItem newsItem;
            newsItem.thumbnailUrl = thumbnailUrl;
            newsItem.title = title;
            newsItem.contents = contents;
            newsItem.source = "Steam";
            newsItem.url = url;
            newsItem.timestamp = timestamp;
            newsItem.steamAppId = appId.toLongLong();

            allNewsItems.append(newsItem);
        }
        pendingRequests--;
        if (pendingRequests <= 0) {
            sortNews();
            emit newsReady();
        }

    }
    reply->deleteLater();
}
