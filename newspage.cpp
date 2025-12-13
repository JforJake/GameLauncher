#include "newspage.h"
#include "ui_newspage.h"
#include "mainwindow.h"
#include <QPushButton>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QRegularExpression>
#include <QDesktopServices>
#include <QXmlStreamReader>
#include <QCoreApplication>

// Menu that holds the news articles

NewsPage::NewsPage(NewsFetcher *mainNewsFetch, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewsPage)
    , newsFetcher(mainNewsFetch)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(parent);

    // connects button to return to the main menu
    connect(ui->BackToMenuButton, &QPushButton::clicked, this, &NewsPage::onBackToMenuButtonClicked);

    setupNewsDisplay();

    // if the newsFetcher exists
    if (newsFetcher) {
        // connects the emit signal with the function to display the news
        connect(newsFetcher, &NewsFetcher::newsReady, this, &NewsPage::displayNews);

        // if the list isn't empty
        if (!newsFetcher->getAllNewsItems().isEmpty()) {
            displayNews();
        }
    }
}

// resize news items if needed
void NewsPage::resizeNews(int width, int height) {

}

// Sets up the vertical display for the news items
void NewsPage::setupNewsDisplay() {
    scrollArea = ui->scrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    newsContainer = new QWidget();
    newsLayout = new QVBoxLayout(newsContainer);
    newsLayout->setSpacing(10);
    newsLayout->setAlignment(Qt::AlignTop);
    newsLayout->setContentsMargins(10, 10, 10, 10);

    scrollArea->setWidget(newsContainer);
}

// Displays the news contained in the list
void NewsPage::displayNews() {
    auto allNewsItems = newsFetcher->getAllNewsItems();
    // Cycles through the list of news articles
    for (const auto &item : allNewsItems) {
        //caps the description at 85 to not let it trail off
        if (item.contents.length() > 85) {
            addNewsCard(item.thumbnailUrl, item.title, (item.contents.left(82) + "..."), item.source, item.url);
        } else {
            addNewsCard(item.thumbnailUrl, item.title, item.contents, item.source, item.url);
        }
    }
}

// Adds a news card to the display
void NewsPage::addNewsCard(const QString &thumbnailUrl, const QString &title, const QString &description, const QString &source, const QString &articleUrl) {
    QWidget *card = createNewsCard(thumbnailUrl, title, description, source, articleUrl);
    newsLayout->addWidget(card);
}

// creats a news Card based on the news item
QWidget* NewsPage::createNewsCard(const QString &thumbnailUrl,
                                  const QString &title,
                                  const QString &description,
                                  const QString &source,
                                  const QString &articleUrl) {

    // The news card dimensions
    QPushButton *card = new QPushButton(newsContainer);
    card->setFixedHeight(120);
    card->setCursor(Qt::PointingHandCursor);
    card->setStyleSheet(
        "QPushButton {"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   text-align: left;"
        "}"
        "QPushButton:hover {"
        "   border: 1px solid #999;"
        "   background-color: #f9f9f9;"
        "}"
        );

    // attaches an url to the card
    connect(card, &QPushButton::clicked, [articleUrl]() {
        QDesktopServices::openUrl(QUrl(articleUrl));
    });

    // Layout for thumbnail left, text right
    QHBoxLayout *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

    // Adds the thumbnail to the card
    QLabel *thumbnail = new QLabel(card);
    thumbnail->setFixedWidth(160);
    thumbnail->setScaledContents(true);
    thumbnail->setStyleSheet(
        "QLabel {"
        "   background-color: #e0e0e0;"
        "   border-top-left-radius: 8px;"
        "   border-bottom-left-radius: 8px;"
        "}"
        );
    // gets the image and applies it to the card
    QNetworkAccessManager *manager = new QNetworkAccessManager(card);
    QObject::connect(manager, &QNetworkAccessManager::finished,
                     [thumbnail, manager](QNetworkReply *reply) {
        QPixmap pixmap;
            if (reply->error() == QNetworkReply::NoError && pixmap.loadFromData(reply->readAll()) && !pixmap.isNull())
                thumbnail->setPixmap(pixmap);
            else {
                thumbnail->setText("IMG");
                thumbnail->setAlignment(Qt::AlignCenter);
            }
        reply->deleteLater();
        manager->deleteLater();
    });
    if (thumbnailUrl.isEmpty())
        thumbnail->setText("IMG"), thumbnail->setAlignment(Qt::AlignCenter);
    else
        manager->get(QNetworkRequest(QUrl(thumbnailUrl)));

    cardLayout->addWidget(thumbnail); // adds the image to the card
    // Creates the space for the text
    QWidget *contentWidget = new QWidget(card);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(12, 10, 12, 10);
    contentLayout->setSpacing(5);

    // Adds the title to the card
    QLabel *titleLabel = new QLabel(title, contentWidget);
    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   color: #222;"
        "}"
        );
    titleLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Adds the description to the card
    QLabel *descLabel = new QLabel(description, contentWidget);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   color: #555;"
        "}"
        );
    descLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    // Adds the source to the card (bottom right)
    QLabel *sourceLabel = new QLabel(source, contentWidget);
    sourceLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 10px;"
        "   color: #888;"
        "   font-style: italic;"
        "}"
        );
    sourceLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    // adds the text to the card
    contentLayout->addWidget(titleLabel, 1);
    contentLayout->addWidget(descLabel, 1);
    contentLayout->addWidget(sourceLabel, 0);

    cardLayout->addWidget(contentWidget);

    return card;
}

// button to go back to main window
void NewsPage::onBackToMenuButtonClicked() {
    if (mw) {
        this->hide();
        mw->returnToMainUI();
    }
}

NewsPage::~NewsPage()
{
    delete ui;
}
