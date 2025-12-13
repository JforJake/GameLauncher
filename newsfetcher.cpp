#include "newsfetcher.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QCoreApplication>
#include <QUrlQuery>
#include <QJsonDocument>

// Class to fetch game news articles

NewsFetcher::NewsFetcher(QObject *parent) {
    networkManager = new QNetworkAccessManager(parent);
    // connection to notify the displays to appear after the news is fetched
    connect(networkManager, &QNetworkAccessManager::finished, this, &NewsFetcher::onNewsReceived);

    loadGameNames();
}


// loads the game Names and SteamAppIds from the database to use for the fetching
void NewsFetcher::loadGameNames() {
    gameNames.clear();
    QString appDir = QCoreApplication::applicationDirPath();
    gameLibrary = new GameLibrary((appDir + "/games.db").toStdString());

    gameNames = gameLibrary->returnNames();
    gameAppIds = gameLibrary->returnSteamAppIds();
}



// Gets the Top Article Name
QString NewsFetcher::getTopArticleName() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().title;
    return "";
}

// Gets the Top Article Description
QString NewsFetcher::getTopArticleText() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().contents;
    return "";
}

// Gets the Top Article Thumbnail
QString NewsFetcher::getTopArticleThumbnail() {
    if (!allNewsItems.isEmpty())
        return allNewsItems.first().thumbnailUrl;
    return "";
}

// returns the entire list of News Articles
QList<NewsItem> NewsFetcher::getAllNewsItems() {
    return allNewsItems;
}

// fetching news functions
void NewsFetcher::fetchAllNews() {
    fetchSteamNews(gameAppIds);
}

// Fetches news about a game using the steam web api
void NewsFetcher::fetchSteamNews(QStringList AppIds) {
    pendingRequests = 0; // to track amount of requests to do
    for (const QString& appId : AppIds) {
        if (appId.isEmpty()) {
            continue;
        }
        // Requests news based on its SteamAppId. 2 articles, with its description capped at 300 characters
        QUrl url("https://api.steampowered.com/ISteamNews/GetNewsForApp/v2/");
        QUrlQuery query;
        query.addQueryItem("appid", appId);
        query.addQueryItem("count", "2");
        query.addQueryItem("maxlength", "300");
        url.setQuery(query);

        QNetworkRequest request(url);
        // gives the request an attribute to identify once its returned
        request.setAttribute(QNetworkRequest::User, appId);

        networkManager->get(request);
        pendingRequests++;
    }
    // once all the requests are finished, it updates the connection
    if (pendingRequests == 0) {
        emit newsReady();
    }
}

// sorts the news to be within 30 days and sorts them by date from earliest to latest
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

// once it recieves the news article it makes it a newsItem and adds it to a list
void NewsFetcher::onNewsReceived(QNetworkReply *reply) {
    // if the reply is with error
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Network error:" << reply->errorString();
        reply->deleteLater();
        return;
    }

    // gets the data from the reply
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

        //  Formats the data from the json to the NewsItem
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
        // subtracts every time a request is complete and sorts it
        pendingRequests--;
        if (pendingRequests <= 0) {
            sortNews();
            emit newsReady();
        }

    }
    reply->deleteLater();
}
