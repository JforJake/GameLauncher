#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMouseEvent>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <iostream>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QGridLayout>
#include <QTimer>
#include <SQLiteCpp/SQLiteCpp.h>
#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    screen = QGuiApplication::primaryScreen();
    geometry = screen->availableGeometry();

    screenWidth = geometry.width();
    screenHeight = geometry.height();
    windowWidth = 480;
    windowHeight = screenHeight;

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

    this->resize(windowWidth, windowHeight);
    this->move(QPoint(screenWidth - (windowWidth * 4), 0));

    m_anim = new QPropertyAnimation(this, "pos", this);
    m_anim->setDuration(180);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);

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
}

void MainWindow::setWindow() {

}

void MainWindow::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
        m_dragging = true;
        m_dragPos = e->globalPosition().toPoint() - frameGeometry().topLeft();
        e->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *e) {
    if (m_dragging) {
        move(e->globalPosition().toPoint() - m_dragPos);
        e->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;
        snapToEdge();
        e->accept();
    }
}

void MainWindow::snapToEdge() {
    cout << "Test" << endl;
    const int snapDistance = screenWidth / 2; // pixels to trigger snap
    QRect winGeom = frameGeometry();

    // Use the screen the window is currently on
    screen = QGuiApplication::screenAt(winGeom.center());
    if (!screen) screen = QGuiApplication::primaryScreen();

    int x = winGeom.x();
    int y = winGeom.y();
    int w = winGeom.width();

    int previousScreenSide = screenSide;
    screenSide = 0;

    // Snap left
    if (screenSide == 0 && abs(x - geometry.x()) <= snapDistance) {
        m_anim->stop();
        m_anim->setStartValue(pos());
        m_anim->setEndValue(QPoint(geometry.x(), 0));
        m_anim->start();
        screenSide = 3;
        return;
    }

    // Snap right
    int right = x + w;
    int availRight = geometry.x() + geometry.width();
    if (screenSide == 0 && abs(availRight - right) <= snapDistance) {
        m_anim->stop();
        m_anim->setStartValue(pos());
        m_anim->setEndValue(QPoint(availRight - w, 0));
        m_anim->start();
        screenSide = 1;
        return;
    }

    // Snap top
    int availBottom = geometry.y() + geometry.height();
    int bottom = y + winGeom.height();

    if (screenSide == 0 && abs(y - geometry.y()) <= snapDistance) {
        m_anim->stop();
        m_anim->setStartValue(pos());
        m_anim->setEndValue(QPoint(x, geometry.y()));
        m_anim->start();
        screenSide = 2; // assign whatever number you're using
        return;
    }

    // Snap bottom
    if (screenSide == 0 && abs(availBottom - bottom) <= snapDistance) {
        m_anim->stop();
        m_anim->setStartValue(pos());
        m_anim->setEndValue(QPoint(x, availBottom - winGeom.height()));
        m_anim->start();
        screenSide = 4;
        return;
    }

    /*int newX, newY;
    if (screenSide == 1 || screenSide == 3) {
        cout << "Snap to side: " << screenSide << endl;
        newX = qBound(geometry.x(), x, availRight - w);
        newY = geometry.y();
    } else if (screenSide == 2 || screenSide == 4) {
        cout << "Snap to side: " << screenSide << endl;
        //newX = avail.x();
        //newY = qBound(avail.y(), y, availBottom - winGeom.height());
    } else {*/
        cout << "Snap to previous side: " << previousScreenSide << endl;
        this->move(x, y);
        return;
    //}
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

MainWindow::~MainWindow()
{
    delete ui;
    delete gameLibrary;
    // delete scanner;
}

// generates the display for the library from the database
// need to figure out a better way to do this using a games id instead as it just uses steamappId for now.
void MainWindow::loadGameLibrary(QGridLayout* grid)
{
    GameLibrary library("games.db");
    std::vector<Game> games = library.getAllGames();

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
