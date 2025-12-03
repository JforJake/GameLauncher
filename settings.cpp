#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"
#include <QPushButton>
#include <Qscreen>
#include <iostream>
#include <QApplication>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);

    mw = qobject_cast<MainWindow*>(parent);
    screens = QGuiApplication::screens();
    setColorSchemes();
    qApp->setPalette(dark);

    // Adding items for screen side setting
    ui->ScreenSideBox->addItem("Left", 0);
    ui->ScreenSideBox->addItem("Right", 1);

    // Adding items for which screen user wants application to appear on
    for (int i = 0; i < screens.count(); i++) {
        QString label = QString("Screen %1").arg(i + 1);
        ui->ScreenNumberBox->addItem(label, i);
    }

    // Adding items for always on top setting
    ui->AlwaysOnTopBox->addItem("False", 0);
    ui->AlwaysOnTopBox->addItem("True", 1);

    // Adding items for color scheme setting
    ui->ColorSchemeBox->addItem("Dark Mode", 0);
    ui->ColorSchemeBox->addItem("Light Mode", 1);

    m_anim = new QPropertyAnimation(mw, "pos", this);
    m_anim->setDuration(250);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);

    // Connecting Buttons
    connect(ui->BackButton, &QPushButton::clicked, this, &Settings::onBackButtonClicked);
    connect(ui->ApplyButton, &QPushButton::clicked, this, &Settings::onApplyButtonClicked);
}

void Settings::onBackButtonClicked() {
    applySettings();
    if (mw) {
        this->hide();
        mw->returnToMainUI();
    }
}

void Settings::setCurrIndex() {
    ui->ScreenSideBox->setCurrentIndex(currSide);
    ui->ScreenNumberBox->setCurrentIndex(QGuiApplication::screens().indexOf(currScreen));
    ui->AlwaysOnTopBox->setCurrentIndex(currOnTop);
}

void Settings::onApplyButtonClicked() {
    applySettings();
}

void Settings::applySettings() {
    if (!mw) return;
    int screen = ui->ScreenNumberBox->currentData().toInt();
    int newSide = ui->ScreenSideBox->currentData().toInt();

    if (screen != QGuiApplication::screens().indexOf(currScreen) || newSide != currSide) {
        moveApplication(screen, newSide);
    }

    if (ui->AlwaysOnTopBox->currentData().toInt() == 0) {
        mw->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        mw->show();
        currOnTop = 0;
    } else {
        mw->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        mw->show();
        currOnTop = 1;
    }

    if (ui->ColorSchemeBox->currentData().toInt() == 0) {
        mw->setPalette(dark);
        this->setPalette(dark);
    } else {
        mw->setPalette(light);
        this->setPalette(light);
    }
}

void Settings::moveApplication(int screenIndex, int side) {
    if (screenIndex < 0 || screenIndex >= screens.size()) return;

    if (screenIndex != QGuiApplication::screens().indexOf(currScreen)) {
        currScreen = screens[screenIndex];
        geometry = currScreen->availableGeometry();
        this->resize(windowWidth, screenHeight);
        mw->resize(windowWidth, screenHeight);
        windowHeight = screenHeight;
    }

    m_anim->stop();
    m_anim->setStartValue(QPoint(mw->pos()));

    if (side == 0) {
        m_anim->setEndValue(QPoint(0, 0));
    } else {
        m_anim->setEndValue(QPoint(screenWidth - windowWidth, 0));
    }

    m_anim->start();
    currSide = side;
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setColorSchemes() {
    // Dark Mode
    // Main window + base background
    dark.setColor(QPalette::Window, QColor(42, 42, 42));
    dark.setColor(QPalette::Base,   QColor(42, 42, 42));

    // Text colors
    dark.setColor(QPalette::WindowText, QColor(255, 255, 255));
    dark.setColor(QPalette::Text,        QColor(255, 255, 255));
    dark.setColor(QPalette::ButtonText,  QColor(255, 255, 255));

    // Buttons
    dark.setColor(QPalette::Button, QColor(0, 156, 148)); // teal

    // Shadows + borders
    dark.setColor(QPalette::Shadow,   QColor(75, 75, 75));
    dark.setColor(QPalette::Dark,     QColor(75, 75, 75));
    dark.setColor(QPalette::Mid,      QColor(75, 75, 75));
    dark.setColor(QPalette::Midlight, QColor(75, 75, 75));
    dark.setColor(QPalette::Light,    QColor(75, 75, 75));

    // Highlight (optional)
    dark.setColor(QPalette::Highlight, QColor(31, 155, 93));

    // Light Mode
    // Main window + base background
    light.setColor(QPalette::Window, QColor(242, 242, 242)); // soft gray
    light.setColor(QPalette::Base,   QColor(255, 255, 255)); // pure white

    // Text colors
    light.setColor(QPalette::WindowText, QColor(0, 0, 0));
    light.setColor(QPalette::Text,        QColor(0, 0, 0));
    light.setColor(QPalette::ButtonText,  QColor(255, 255, 255)); // white on teal

    // Buttons
    light.setColor(QPalette::Button, QColor(0, 156, 148)); // teal

    // Shadows + borders
    light.setColor(QPalette::Shadow,   QColor(160, 160, 160));
    light.setColor(QPalette::Dark,     QColor(160, 160, 160));
    light.setColor(QPalette::Mid,      QColor(192, 192, 192));
    light.setColor(QPalette::Midlight, QColor(215, 215, 215));
    light.setColor(QPalette::Light,    QColor(255, 255, 255));

    // Highlight (same)
    light.setColor(QPalette::Highlight, QColor(0, 156, 148));
}
