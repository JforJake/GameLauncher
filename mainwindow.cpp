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

    if (true) {
        sw->currScreen = QGuiApplication::primaryScreen();
        sw->geometry = sw->currScreen->availableGeometry();

        sw->screenWidth = sw->geometry.width();
        sw->screenHeight = sw->geometry.height();
        sw->windowWidth = 480;
        sw->windowHeight = sw->screenHeight;

        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        sw->currOnTop = 1;
    }

    this->resize(sw->windowWidth, sw->windowHeight);
    this->move(QPoint(sw->screenWidth - sw->windowWidth, 0));
    sw->currSide = 1;

    manager = new QNetworkAccessManager(this);

    QWidget* libContent = ui->LibraryScrollArea->widget();
    libgrid = new QGridLayout(libContent);
    libgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    libgrid->setSpacing(10);

    QWidget* favContent = ui->FavoritesScrollArea->widget();
    favgrid = new QGridLayout(favContent);
    favgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    favgrid->setSpacing(10);

    gameLibrary = new GameLibrary("games.db");

    loadGameLibrary(libgrid);
    loadFavLibrary(favgrid);

    ui->CurrentGameInfo->hide();
    ui->CurrentGameLogo->hide();
    ui->PlayButton->hide();
    ui->RemoveGameButton->hide();
    ui->FavGameButton->hide();

    ui->gameName->setText("");
    ui->gameDesc->setText("");

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

    gameId = obj->property("gameId").toString();
    gameName = obj->property("name").toString();
    appId = obj->property("appId").toString();
    filePath = obj->property("filePath").toString();
    desc = obj->property("desc").toString();



    QPixmap pixmap = obj->property("fullPixmap").value<QPixmap>();

    if (!pixmap.isNull()) {
        auto* scene = new QGraphicsScene();
        scene->addPixmap(pixmap);
        ui->CurrentGameLogo->setScene(scene);
        ui->CurrentGameLogo->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }

    ui->CurrentGameInfo->clear();
    ui->CurrentGameInfo->append("");

    ui->gameName->setText(gameName);
    ui->gameDesc->setText(desc);

    ui->RemoveGameButton->show();
    ui->CurrentGameInfo->show();
    ui->CurrentGameLogo->show();
    ui->PlayButton->show();
    ui->FavGameButton->show();
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
}

void MainWindow::onRemoveButtonClicked() {
    gameLibrary->removeGameById(gameId.toInt());
    appId = 0;
    clearGridLayout(libgrid);
    loadGameLibrary(libgrid);
    ui->CurrentGameInfo->hide();
    ui->CurrentGameLogo->hide();
    ui->PlayButton->hide();
    ui->RemoveGameButton->hide();
    ui->FavGameButton->hide();
}

void MainWindow::onFavButtonClicked() {
    gameLibrary->toggleFavorite(gameId.toInt());
    loadFavLibrary(favgrid);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gameLibrary;
    delete sw;
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
    const int maxCols = 4;
    for (const auto& game : games) {
        QPushButton* gameButton = new QPushButton(QString::fromStdString(game.name));
        gameButton->setProperty("gameId", static_cast<qulonglong>(game.id));
        gameButton->setProperty("appId", static_cast<qulonglong>(game.steamAppId));
        gameButton->setProperty("filePath", QString::fromStdString(game.directory));
        gameButton->setProperty("name", QString::fromStdString(game.name));
        gameButton->setProperty("desc", QString::fromStdString(game.description));

        // load image
        QString imagePath = QString::fromStdString(game.imagePath);
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            QPixmap scaled = pixmap.scaledToWidth(88, Qt::SmoothTransformation);
            gameButton->setFixedSize(scaled.size());
            gameButton->setIcon(QIcon(scaled));
            gameButton->setIconSize(scaled.size());
            gameButton->setText("");
            gameButton->setProperty("fullPixmap", pixmap);
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
    const int maxCols = 4;
    for (const auto& game : games) {
        QPushButton* gameButton = new QPushButton(QString::fromStdString(game.name));
        gameButton->setProperty("gameId", static_cast<qulonglong>(game.id));
        gameButton->setProperty("appId", static_cast<qulonglong>(game.steamAppId));
        gameButton->setProperty("filePath", QString::fromStdString(game.directory));
        gameButton->setProperty("name", QString::fromStdString(game.name));
        gameButton->setProperty("desc", QString::fromStdString(game.description));

        // load image
        QString imagePath = QString::fromStdString(game.imagePath);
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            QPixmap scaled = pixmap.scaledToWidth(88, Qt::SmoothTransformation);
            gameButton->setFixedSize(scaled.size());
            gameButton->setIcon(QIcon(scaled));
            gameButton->setIconSize(scaled.size());
            gameButton->setText("");
            gameButton->setProperty("fullPixmap", pixmap);
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

