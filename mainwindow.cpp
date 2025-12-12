#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGridLayout>
#include <QTimer>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>
#include <fstream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sw = new Settings(this);
    sw->hide();

    nw = new NewsPage(this);
    nw->hide();

    QString appDir = QCoreApplication::applicationDirPath();
    std::string settingsPath = (appDir + "/settings.json").toStdString();

    sw->setColorSchemes();
    setWindowFlags(Qt::FramelessWindowHint);
    sw->currScreen = QGuiApplication::primaryScreen();
    sw->geometry = sw->currScreen->availableGeometry();

    sw->screenWidth = sw->geometry.width();
    sw->screenHeight = sw->geometry.height();
    sw->windowWidth = 480;
    sw->windowHeight = sw->screenHeight;

    std::ifstream file(settingsPath);
    if (file.is_open()) {
        file >> sw->j;
        auto& j = sw->j;

        if (j.contains("screenSide"))
            sw->currSide = j["screenSide"];
        if (j.contains("screenNumber"))
            sw->currScreenIndex = j["screenNumber"];
        if (j.contains("alwaysOnTop"))
            sw->currOnTop = j["alwaysOnTop"];
        if (j.contains("colorScheme"))
            sw->currColorScheme = j["colorScheme"];
        if (j.contains("programWidth"))
            sw->currWidthIndex = j["programWidth"];
        if (j.contains("startup")) {
            sw->currStartupIndex = j["startup"];
        }
    } else {
        sw->currSide = 1;
        sw->currScreenIndex = 0;
        sw->currOnTop = 1;
        sw->currColorScheme = 0;
        sw->currWidthIndex = 1;
        sw->currStartupIndex = 0;
    }
    if (sw->currScreenIndex >= 0 && sw->currScreenIndex < sw->screens.size()) {
        sw->currScreen = sw->screens[sw->currScreenIndex];
    } else {
        sw->currScreen = QGuiApplication::primaryScreen();
        sw->currScreenIndex = 0;
    }
    sw->setCurrIndex();
    sw->applySettings();

    manager = new QNetworkAccessManager(this);

    QWidget* libContent = ui->LibraryScrollArea->widget();
    libgrid = new QGridLayout(libContent);
    libgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    libgrid->setSpacing(10);

    QWidget* favContent = ui->FavoritesScrollArea->widget();
    favgrid = new QGridLayout(favContent);
    favgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    favgrid->setSpacing(10);

    gameLibrary = new GameLibrary((appDir + "/games.db").toStdString());

    loadGameLibrary(libgrid);
    loadFavLibrary(favgrid);

    ui->CurrentGame->hide();
    currGameSectionHidden = true;

    ui->gameName->setText("");
    ui->gameDesc->setText("");

    setNewsSection();
    setGlobalStyle();
    ui->SettingsButton->setIcon(QIcon(":/res/res/SettingsIcon.png"));
    ui->ImportButton->setIcon(QIcon(":/res/res/ImportIcon.png"));
    ui->MinimizeButton->setIcon(QIcon(":/res/res/MinimizeIcon.png"));
    ui->ExitButton->setIcon(QIcon(":/res/res/XIcon.png"));
    ui->RemoveGameButton->setIcon(QIcon(":/res/res/XIcon.png"));

    connect(ui->MinimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
    connect(ui->SettingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    connect(ui->PlayButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->ImportButton, &QPushButton::clicked, this, &MainWindow::onImportButtonClicked);
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::onExitButtonClicked);
    connect(ui->MoreNewsButton, &QPushButton::clicked, this, &MainWindow::onMoreNewsButtonClicked);
    connect(ui->RemoveGameButton, &QPushButton::clicked, this, &MainWindow::onRemoveButtonClicked);
    connect(ui->FavGameButton, &QPushButton::clicked, this, &MainWindow::onFavButtonClicked);
}

void MainWindow::onGameButtonClicked()
{

    QObject* obj = sender();
    if (!obj) return;

    if (!currGameSectionHidden && gameId == obj->property("gameId").toString()) {
        ui->CurrentGame->hide();
        currGameSectionHidden = true;
    } else {
        ui->CurrentGame->show();
        currGameSectionHidden = false;

        gameId = obj->property("gameId").toString();
        gameName = obj->property("name").toString();
        appId = obj->property("appId").toString();
        filePath = obj->property("filePath").toString();
        desc = obj->property("desc").toString();
        favorited = obj->property("favorited").toBool();

        QString imagePath = obj->property("imagePath").toString();
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            auto* scene = new QGraphicsScene();
            scene->addPixmap(pixmap);
            ui->CurrentGameLogo->setScene(scene);
            ui->CurrentGameLogo->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }

        ui->gameName->setText(gameName);
        ui->gameDesc->setText(desc);

        if (favorited) {
            ui->FavGameButton->setIcon(QIcon(":/res/res/Favorited.png"));
        } else {
            ui->FavGameButton->setIcon(QIcon(":/res/res/NotFavorited.png"));
        }
    }
}

void MainWindow::onPlayButtonClicked() {
    qulonglong steamAppId = appId.toULongLong();
    if (steamAppId > 0) {
        if (gameLibrary->launchGameById(appId.toStdString()))
            this->showMinimized();
    } else {
        if (gameLibrary->launchGameByPath(filePath.toStdString()))
            this->showMinimized();
    }
}

void MainWindow::onExitButtonClicked() {
    QCoreApplication::quit();
}

void MainWindow::onMinimizeButtonClicked() {
    this->showMinimized();
}

void MainWindow::onImportButtonClicked() {
    Wizard *wizard = new Wizard(gameLibrary, this);
    wizard->setAttribute(Qt::WA_DeleteOnClose);
    connect(wizard, &QWizard::accepted, this, [this]() { loadGameLibrary(libgrid); });
    wizard->show();
}

void MainWindow::onSettingsButtonClicked() {
    ui->centralwidget->hide();
    sw->show();
    sw->resize(sw->windowWidth, sw->windowHeight);
    sw->setCurrIndex();
}

void MainWindow::onMoreNewsButtonClicked() {
    ui->centralwidget->hide();
    nw->show();
    nw->resize(sw->windowWidth, sw->windowHeight);
}

void MainWindow::returnToMainUI() {
    sw->hide();
    ui->centralwidget->show();
    clearGridLayout(libgrid);
    clearGridLayout(favgrid);
    loadGameLibrary(libgrid);
    loadFavLibrary(favgrid);
}

void MainWindow::onRemoveButtonClicked() {
    gameLibrary->removeGameById(gameId.toInt());
    appId = 0;
    clearGridLayout(libgrid);
    loadGameLibrary(libgrid);
    ui->CurrentGame->hide();
}

void MainWindow::onFavButtonClicked() {
    gameLibrary->toggleFavorite(gameId.toInt());
    if (favorited) {
        ui->FavGameButton->setIcon(QIcon(":/res/res/NotFavorited.png"));
    } else {
        ui->FavGameButton->setIcon(QIcon(":/res/res/Favorited.png"));
    }
    loadFavLibrary(favgrid);
}

void MainWindow::clearGridLayout(QGridLayout* grid) {
    while (QLayoutItem* item = grid->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

void MainWindow::loadGameLibrary(QGridLayout* grid)
{
    while (QLayoutItem *item = grid->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    vector<Game> games = gameLibrary->getAllGames();
    int row = 0, col = 0;
    const int maxCols = sw->windowWidth / 120;
    for (const auto& game : games) {
        QPushButton* gameButton = new QPushButton(QString::fromStdString(game.name));
        gameButton->setProperty("gameId", static_cast<qulonglong>(game.id));
        gameButton->setProperty("appId", static_cast<qulonglong>(game.steamAppId));
        gameButton->setProperty("filePath", QString::fromStdString(game.directory));
        gameButton->setProperty("name", QString::fromStdString(game.name));
        gameButton->setProperty("desc", QString::fromStdString(game.description));
        gameButton->setProperty("favorited", game.favorited);

        // load image
        QString imagePath = QString::fromStdString(game.imagePath);
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            QPixmap scaled = pixmap.scaledToWidth(88, Qt::SmoothTransformation);
            gameButton->setFixedSize(scaled.size());
            gameButton->setIcon(QIcon(scaled));
            gameButton->setIconSize(scaled.size());
            gameButton->setText("");
            gameButton->setProperty("imagePath", imagePath);
        } else {
            // if image doesn't exist
            gameButton->setText(QString::fromStdString(game.name));
        }

        connect(gameButton, &QPushButton::clicked, this, &MainWindow::onGameButtonClicked);
        grid->addWidget(gameButton, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::loadFavLibrary(QGridLayout* grid)
{
    while (QLayoutItem *item = grid->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    vector<Game> games = gameLibrary->getFavoriteGames();
    int row = 0, col = 0;
    const int maxCols = sw->windowWidth / 120;
    for (const auto& game : games) {
        QPushButton* gameButton = new QPushButton(QString::fromStdString(game.name));
        gameButton->setProperty("gameId", static_cast<qulonglong>(game.id));
        gameButton->setProperty("appId", static_cast<qulonglong>(game.steamAppId));
        gameButton->setProperty("filePath", QString::fromStdString(game.directory));
        gameButton->setProperty("name", QString::fromStdString(game.name));
        gameButton->setProperty("desc", QString::fromStdString(game.description));
        gameButton->setProperty("favorited", game.favorited);

        // load image
        QString imagePath = QString::fromStdString(game.imagePath);
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            QPixmap scaled = pixmap.scaledToWidth(88, Qt::SmoothTransformation);
            gameButton->setFixedSize(scaled.size());
            gameButton->setIcon(QIcon(scaled));
            gameButton->setIconSize(scaled.size());
            gameButton->setText("");
            gameButton->setProperty("imagePath", imagePath);
        } else {
            // if image doesn't exist
            gameButton->setText(QString::fromStdString(game.name));
        }

        connect(gameButton, &QPushButton::clicked, this, &MainWindow::onGameButtonClicked);
        grid->addWidget(gameButton, row, col);
        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::setNewsSection() {
    ui->NewsLabel->setText(nw->getTopArticleName());
    ui->NewsText->setText(nw->getTopArticleText());
}

void MainWindow::setGlobalStyle() {
    QString tabStyle = R"(
        QTabBar::tab {
            background: palette(Button);
            color: palette(ButtonText);
            padding: 6px 12px;
            margin: 2px;
            border-radius: 6px;
        }
    )";

    qApp->setStyleSheet(qApp->styleSheet() + tabStyle);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gameLibrary;
    delete sw;
    delete nw;
    delete manager;
}
