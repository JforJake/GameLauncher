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

NewsPage::NewsPage(NewsFetcher *mainNewsFetch, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::NewsPage)
    , newsFetcher(mainNewsFetch)
{
    ui->setupUi(this);
    mw = qobject_cast<MainWindow*>(parent);
    connect(ui->BackToMenuButton, &QPushButton::clicked, this, &NewsPage::onBackToMenuButtonClicked);

    setupNewsDisplay();
    if (newsFetcher) {
        connect(newsFetcher, &NewsFetcher::newsReady, this, &NewsPage::displayNews);

        if (!newsFetcher->getAllNewsItems().isEmpty()) {
            displayNews();
        }
    }
}

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

void NewsPage::displayNews() {
    auto allNewsItems = newsFetcher->getAllNewsItems();
    for (const auto &item : allNewsItems) {
        addNewsCard(item.thumbnailUrl, item.title, item.contents, item.source, item.url);
    }
}



void NewsPage::addNewsCard(const QString &thumbnailUrl, const QString &title, const QString &description, const QString &source, const QString &articleUrl) {
    QWidget *card = createNewsCard(thumbnailUrl, title, description, source, articleUrl);
    newsLayout->addWidget(card);
}

QWidget* NewsPage::createNewsCard(const QString &thumbnailUrl,
                                  const QString &title,
                                  const QString &description,
                                  const QString &source,
                                  const QString &articleUrl) {

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

    connect(card, &QPushButton::clicked, [articleUrl]() {
        QDesktopServices::openUrl(QUrl(articleUrl));
    });

    QHBoxLayout *cardLayout = new QHBoxLayout(card);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);

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


    cardLayout->addWidget(thumbnail);

    QWidget *contentWidget = new QWidget(card);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(12, 10, 12, 10);
    contentLayout->setSpacing(5);

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

    QLabel *descLabel = new QLabel(description, contentWidget);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   color: #555;"
        "}"
        );
    descLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    QLabel *sourceLabel = new QLabel(source, contentWidget);
    sourceLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 10px;"
        "   color: #888;"
        "   font-style: italic;"
        "}"
        );
    sourceLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);

    contentLayout->addWidget(titleLabel, 1);
    contentLayout->addWidget(descLabel, 1);
    contentLayout->addWidget(sourceLabel, 0);

    cardLayout->addWidget(contentWidget);

    return card;
}

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
