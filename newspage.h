#ifndef NEWSPAGE_H
#define NEWSPAGE_H

#include <QWidget>
#include <qboxlayout>
#include <qnetworkaccessmanager>
#include <qscrollarea>
#include "gamelibrary.h"
#include "newsfetcher.h"

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
    explicit NewsPage(NewsFetcher* mainNewsFetch, QWidget *parent = nullptr);
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
    ~NewsPage();

private:
    Ui::NewsPage *ui;
    MainWindow *mw = nullptr;
    void onBackToMenuButtonClicked();
    void displayNews();
    QScrollArea *scrollArea;
    QWidget *newsContainer;
    QVBoxLayout *newsLayout;
    NewsFetcher* newsFetcher;




};

#endif // NEWSPAGE_H
