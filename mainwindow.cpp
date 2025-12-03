#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "importwindow.h"
#include <QMouseEvent>
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

    QWidget* scrollContent = ui->scrollArea->widget();
    libgrid = new QGridLayout(scrollContent);
    libgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    libgrid->setSpacing(10);

    gameLibrary = new GameLibrary("games.db");

    loadGameLibrary(libgrid);

    connect(ui->MinimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::onExitButtonClicked);
    connect(ui->PlayButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->ImportButton, &QPushButton::clicked, this, &MainWindow::onImportButtonClicked);
    connect(ui->SettingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
}

void MainWindow::onGameButtonClicked()
{
    QObject* obj = sender();
    if (!obj) return;

    appId = obj->property("appId").toString();
    QPixmap pixmap = obj->property("fullPixmap").value<QPixmap>();

    if (!pixmap.isNull()) {
        auto* scene = new QGraphicsScene();
        scene->addPixmap(pixmap);
        ui->CurrentGameLogo->setScene(scene);
        ui->CurrentGameLogo->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void MainWindow::onPlayButtonClicked() {
    if (gameLibrary->launchGame(appId.toStdString())) {
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
    ImportWindow *window = new ImportWindow(this, gameLibrary);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->exec();
    loadGameLibrary(libgrid);
}

void MainWindow::onSettingsButtonClicked() {
    ui->centralwidget->hide();
    sw->show();
    sw->resize(sw->windowWidth, sw->windowHeight);
    sw->setCurrIndex();
}

void MainWindow::returnToMainUI() {
    sw->hide();
    ui->centralwidget->show();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete gameLibrary;
    delete sw;
    // delete scanner;
}

// generates the display for the library from the database
// need to figure out a better way to do this using a games id instead as it just uses steamappId for now.
void MainWindow::loadGameLibrary(QGridLayout* grid)
{
    GameLibrary library("games.db");
    vector<Game> games = library.getAllGames();

    int row = 0, col = 0;
    const int maxCols = 4;

    for (const auto& game : games) {
        QPushButton* gameButton = new QPushButton(QString::fromStdString(game.name));
        gameButton->setProperty("appId", static_cast<qulonglong>(game.steamAppId));

        QString url = QString("https://cdn.cloudflare.steamstatic.com/steam/apps/%1/library_600x900.jpg")
                          .arg(game.steamAppId);

        QNetworkReply* reply = manager->get(QNetworkRequest(url));
        connect(reply, &QNetworkReply::finished, [reply, gameButton]() {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            if (!pixmap.isNull()) {
                QPixmap scaled = pixmap.scaledToWidth(88, Qt::SmoothTransformation);
                gameButton->setFixedSize(scaled.size());
                gameButton->setIcon(QIcon(scaled));
                gameButton->setIconSize(scaled.size());
                gameButton->setText("");
                gameButton->setProperty("fullPixmap", pixmap);
            }
            reply->deleteLater();
        });

        connect(gameButton, &QPushButton::clicked, this, &MainWindow::onGameButtonClicked);
        grid->addWidget(gameButton, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}
