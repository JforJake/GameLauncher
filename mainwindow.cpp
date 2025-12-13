#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "wizard.h"
#include <QScreen>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGridLayout>
#include <QTimer>
#include <SQLiteCpp/SQLiteCpp.h>
#include <fstream>
#include <QPainterPath>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //sets up the UI files
    ui->setupUi(this);

    sw = new Settings(this);
    sw->hide();

    newsFetcher = new NewsFetcher(this);

    nw = new NewsPage(newsFetcher, this);
    nw->hide();

    // Finds the applications current directory
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

    // loads settings from the json
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
    firstLoad = true;
    sw->setCurrIndex();
    sw->applySettings();

    manager = new QNetworkAccessManager(this);

    // Grid for the All library
    QWidget* libContent = ui->LibraryScrollArea->widget();
    libgrid = new QGridLayout(libContent);
    libgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    libgrid->setSpacing(10);
    // Grid for the Fav library
    QWidget* favContent = ui->FavoritesScrollArea->widget();
    favgrid = new QGridLayout(favContent);
    favgrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    favgrid->setSpacing(10);

    gameLibrary = new GameLibrary((appDir + "/games.db").toStdString()); // loads database

    connect(newsFetcher, &NewsFetcher::newsReady, this, &MainWindow::setNewsSection); // puts news on the panel once it is ready
    newsFetcher->fetchAllNews();

    // loads the libraries
    loadGameLibrary(libgrid);
    loadFavLibrary(favgrid);

    ui->CurrentGame->hide();
    currGameSectionHidden = true;

    ui->gameName->setText("");
    ui->gameDesc->setText("");

   // setNewsSection();
    setGlobalStyle();
    ui->SettingsButton->setIcon(QIcon(":/res/res/SettingsIcon.png"));
    ui->ImportButton->setIcon(QIcon(":/res/res/ImportIcon.png"));
    ui->MinimizeButton->setIcon(QIcon(":/res/res/MinimizeIcon.png"));
    ui->ExitButton->setIcon(QIcon(":/res/res/XIcon.png"));
    ui->RemoveGameButton->setIcon(QIcon(":/res/res/XIcon.png"));

    // sets the scroll bars
    ui->NewsText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->NewsText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->LibraryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->FavoritesScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->GameNewsLogo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->GameNewsLogo->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // sets the line widths
    ui->GameNewsLogo->setFrameShape(QFrame::NoFrame);
    ui->GameNewsLogo->setLineWidth(0);
    ui->GameNewsLogo->setMidLineWidth(0);
    ui->CurrentGameLogo->setFrameShape(QFrame::NoFrame);
    ui->CurrentGameLogo->setLineWidth(0);
    ui->CurrentGameLogo->setMidLineWidth(0);

    ui->GameNewsLogo->setStyleSheet(
        "QGraphicsView { background: transparent; }"
        "QGraphicsView::viewport { background: transparent; }"
    );
    ui->CurrentGameLogo->setStyleSheet(
        "QGraphicsView { background: transparent; }"
        "QGraphicsView::viewport { background: transparent; }"
    );

    // Connections for the buttons
    connect(ui->MinimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);
    connect(ui->SettingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    connect(ui->PlayButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    connect(ui->ImportButton, &QPushButton::clicked, this, &MainWindow::onImportButtonClicked);
    connect(ui->ExitButton, &QPushButton::clicked, this, &MainWindow::onExitButtonClicked);
    connect(ui->MoreNewsButton, &QPushButton::clicked, this, &MainWindow::onMoreNewsButtonClicked);
    connect(ui->RemoveGameButton, &QPushButton::clicked, this, &MainWindow::onRemoveButtonClicked);
    connect(ui->FavGameButton, &QPushButton::clicked, this, &MainWindow::onFavButtonClicked);
}

// Once a button is clicked,
// The mini menu appears and the library shrinks and vice versa if the game is clicked again.
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

        QString imagePath = obj->property("imagePath").toString();
        QPixmap pixmap(imagePath);

        if (!pixmap.isNull()) {
            auto* scene = new QGraphicsScene();
            scene->addPixmap(pixmap);
            ui->CurrentGameLogo->setScene(scene);
            ui->CurrentGameLogo->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }

        if (sw->windowWidth == 360 && desc.length() > 70) {
            desc = desc.left(67) + "...";
        } else if (sw->windowWidth == 480 && desc.length() > 120) {
            desc = desc.left(117) + "...";
        } else if (sw->windowWidth == 600 && desc.length() > 170) {
            desc = desc.left(167) + "...";
        } else if (sw->windowWidth == 720 && desc.length() > 220) {
            desc = desc.left(217) + "...";
        }
        ui->gameName->setText(gameName);
        ui->gameDesc->setText(desc);

        favorited = gameLibrary->isFavorited(gameId.toInt());

        ui->FavGameButton->setIcon(QIcon(
            favorited ? ":/res/res/Favorited.png"
                      : ":/res/res/NotFavorited.png"
            ));
    }
}

// Launches the game
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

// Closes the window
void MainWindow::onExitButtonClicked() {
    QCoreApplication::quit();
}

// Minimizes the window
void MainWindow::onMinimizeButtonClicked() {
    this->showMinimized();
}

// Opens the wizard to import (a) game/games
void MainWindow::onImportButtonClicked() {
    Wizard *wizard = new Wizard(gameLibrary, this);
    wizard->setAttribute(Qt::WA_DeleteOnClose);
    connect(wizard, &QWizard::accepted, this, [this]() { loadGameLibrary(libgrid); });
    wizard->show();
}

// Opens the settings Menu
void MainWindow::onSettingsButtonClicked() {
    ui->centralwidget->hide();
    sw->show();
    sw->resize(sw->windowWidth, sw->windowHeight);
    sw->setCurrIndex();
}

// Opens the news Menu
void MainWindow::onMoreNewsButtonClicked() {
    ui->centralwidget->hide();
    nw->show();
    nw->resize(sw->windowWidth, sw->windowHeight);
}

// Returns to the main menu
void MainWindow::returnToMainUI() {
    sw->hide();
    ui->centralwidget->show();
    clearGridLayout(libgrid);
    clearGridLayout(favgrid);
    loadGameLibrary(libgrid);
    loadFavLibrary(favgrid);
    setNewsSection();
}

// Removes the selected game from the database
void MainWindow::onRemoveButtonClicked() {
    gameLibrary->removeGameById(gameId.toInt());
    appId = 0;
    clearGridLayout(libgrid);
    loadGameLibrary(libgrid);
    ui->CurrentGame->hide();
}

// Favorites a game and adds it to the Fav tab
void MainWindow::onFavButtonClicked() {
    gameLibrary->toggleFavorite(gameId.toInt());
    if (favorited) {
        ui->FavGameButton->setIcon(QIcon(":/res/res/NotFavorited.png"));
    } else {
        ui->FavGameButton->setIcon(QIcon(":/res/res/Favorited.png"));
    }
    loadFavLibrary(favgrid);
}

// Clears a library in order to refresh
void MainWindow::clearGridLayout(QGridLayout* grid) {
    while (QLayoutItem* item = grid->takeAt(0)) {
        if (QWidget* w = item->widget()) {
            w->deleteLater();
        }
        delete item;
    }
}

// Loads the All library into the grid
void MainWindow::loadGameLibrary(QGridLayout* grid)
{
    while (QLayoutItem *item = grid->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    vector<Game> games = gameLibrary->getAllGames();
    int row = 0, col = 0;
    const int maxCols = sw->windowWidth / 120;
    // cycles through the vector of games to add its property to the buttons in the library tab
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

        // scales the image to the object size from local
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

// Loads the fav library into the grid
void MainWindow::loadFavLibrary(QGridLayout* grid)
{
    while (QLayoutItem *item = grid->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    vector<Game> games = gameLibrary->getFavoriteGames();
    int row = 0, col = 0;
    const int maxCols = sw->windowWidth / 120;

    // cycles through the vector of games to add its property to the buttons in the library tab
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

        // scales the image to the object size from local
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

// sets the news section from the first item in the fetched news
void MainWindow::setNewsSection() {
    // formats the article title
    QString newsLabel = newsFetcher->getTopArticleName();
    if (sw->windowWidth == 360 && newsLabel.length() > 13) {
        ui->NewsLabel->setText(newsLabel.left(10) + "...");
    } else if (sw->windowWidth == 480 && newsLabel.length() > 30) {
        ui->NewsLabel->setText(newsLabel.left(27) + "...");
    } else if (sw->windowWidth == 600 && newsLabel.length() > 47) {
        ui->NewsLabel->setText(newsLabel.left(44) + "...");
    } else if (sw->windowWidth == 720 && newsLabel.length() > 64) {
        ui->NewsLabel->setText(newsLabel.left(61) + "...");
    } else {
        ui->NewsLabel->setText(newsLabel);
    }
    // formats the article description
    QString newsText = newsFetcher->getTopArticleText();
    if (sw->windowWidth == 360 && newsText.length() > 150) {
        ui->NewsText->setText(newsText.left(147) + "...");
    } else if (sw->windowWidth == 480 && newsText.length() > 200) {
        ui->NewsText->setText(newsText.left(197) + "...");
    } else if (sw->windowWidth == 600 && newsText.length() > 240) {
        ui->NewsText->setText(newsText.left(237) + "...");
    } else if (sw->windowWidth == 720 && newsText.length() > 280) {
        ui->NewsText->setText(newsText.left(277) + "...");
    } else {
        ui->NewsText->setText(newsText);
    }

    // makes sure news articles exist
    auto items = newsFetcher->getAllNewsItems();
    if (items.isEmpty())
        return;

    long long steamAppId = items.first().steamAppId;
    QString imagePath = gameLibrary->getImagePathForSteamApp(steamAppId);

    if (imagePath.isEmpty())
        return;

    QPixmap pixmap(imagePath);
    if (pixmap.isNull())
        return;

    QSize targetSize = ui->GameNewsLogo->viewport()->size();

    QPixmap scaled = pixmap.scaled(
        targetSize,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
        );

    auto *scene = new QGraphicsScene(ui->GameNewsLogo);
    scene->addPixmap(scaled);
    scene->setSceneRect(scaled.rect());
    ui->GameNewsLogo->setScene(scene);
    roundLeftCorners(ui->GameNewsLogo, 12);
}

// Sets the style for the window
void MainWindow::setGlobalStyle() {
    QString tabStyle = R"(
        QTabBar::tab {
            background: palette(Button);
            color: palette(ButtonText);
            padding: 6px 12px;
            margin: 2px;
            border-radius: 6px;
        }

        QTabBar::tab:selected {
            background: palette(Highlight);
            color: white;
            font-weight: bold;
        }

        QTabBar::tab:hover {
            background: palette(Midlight);
            color: white
        }
    )";

    qApp->setStyleSheet(qApp->styleSheet() + tabStyle);
}

// adds round corners
void MainWindow::roundLeftCorners(QGraphicsView *view, int radius)
{
    QWidget *vp = view->viewport();
    QSize s = vp->size();

    QPainterPath path;
    path.moveTo(radius, 0);
    path.lineTo(s.width(), 0);
    path.lineTo(s.width(), s.height());
    path.lineTo(radius, s.height());
    path.quadTo(0, s.height(), 0, s.height() - radius);
    path.lineTo(0, radius);
    path.quadTo(0, 0, radius, 0);

    QRegion mask(path.toFillPolygon().toPolygon());
    vp->setMask(mask);
}

// deletes everything after its done
MainWindow::~MainWindow()
{
    delete ui;
    delete gameLibrary;
    delete sw;
    delete nw;
    delete manager;
}
