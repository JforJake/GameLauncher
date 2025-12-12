#ifndef NEWSPAGE_H
#define NEWSPAGE_H

#include <QWidget>
#include <qboxlayout>
#include <qnetworkaccessmanager>
#include <qscrollarea>
#include "gamelibrary.h"

namespace Ui {
class NewsPage;
}

class MainWindow;

class NewsPage : public QWidget
{
    Q_OBJECT


    struct NewsItem {
        QString thumbnailUrl;
        QString title;
        QString contents;
        QString source;
        QString url;
        int timestamp;
    };


public:
    explicit NewsPage(QWidget *parent = nullptr);
    void setupNewsDisplay();
    QWidget* createNewsCard(const QString &thumbnailUrl,
                            const QString &title,
                            const QString &description,
                            const QString &source,
                            const QString &articleUrl);
    void addNewsCard(const QString &thumbnailUrl,
                    const QString &title,
                    const QString &description,
                    const QString &source,
                    const QString &articleUrl);
    QString getTopArticleName();
    QString getTopArticleText();
    QString getTopArticleThumbnail();
    ~NewsPage();

private:
    Ui::NewsPage *ui;
    MainWindow *mw = nullptr;
    void onBackToMenuButtonClicked();
    void loadGameNames();
    void fetchNewsForAllGames();
    void fetchSteamNews(QStringList AppIds);
    void onNewsReceived(QNetworkReply *reply);
    void sortAndDisplayNews();
    QScrollArea *scrollArea;
    QWidget *newsContainer;
    QVBoxLayout *newsLayout;
    QStringList gameNames;
    QStringList gameAppIds;
    GameLibrary* gameLibrary;
    QNetworkAccessManager* networkManager;
    QList<NewsItem> allNewsItems;
    QList<NewsItem> sortedNewsItems;




};

#endif // NEWSPAGE_H
