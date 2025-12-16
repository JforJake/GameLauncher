#ifndef NEWSFETCHER_H
#define NEWSFETCHER_H

#include <QNetworkAccessManager>
#include "gamelibrary.h"

struct NewsItem {
    QString thumbnailUrl;
    QString title;
    QString contents;
    QString source;
    QString url;
    int timestamp;
    long long steamAppId;
};

class NewsFetcher: public QObject
{
    Q_OBJECT

signals:
    void newsReady();

public:
    NewsFetcher(QObject *parent = nullptr);
    QString getTopArticleName();
    QString getTopArticleText();
    QString getTopArticleThumbnail();
    QList<NewsItem> getAllNewsItems();
    void fetchAllNews();

private:
    void loadGameNames();
    void fetchSteamNews(QStringList AppIds);
    void onNewsReceived(QNetworkReply *reply);
    void sortNews();
    int pendingRequests;
    QList<NewsItem> allNewsItems;
    QStringList gameNames;
    QStringList gameAppIds;
    GameLibrary* gameLibrary;
    QNetworkAccessManager* networkManager;
};

#endif // NEWSFETCHER_H
