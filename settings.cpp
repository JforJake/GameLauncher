#include "settings.h"
#include "ui_settings.h"
#include "mainwindow.h"
#include <QPushButton>
#include <Qscreen>
#include <QApplication>
#include <QparallelAnimationGroup>
#include <windows.h>
#include <QCoreApplication>
#include <fstream>
#include <QDir>

Settings::Settings(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
{
    ui->setupUi(this);
    setColorSchemes();

    mw = qobject_cast<MainWindow*>(parent);
    screens = QGuiApplication::screens();

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

    // Adding items for program width setting
    ui->ProgramWidthBox->addItem("360 Pixels", 0);
    ui->ProgramWidthBox->addItem("480 Pixels", 1);
    ui->ProgramWidthBox->addItem("600 Pixels", 2);
    ui->ProgramWidthBox->addItem("720 Pixels", 3);

    // Adding items for startup setting
    ui->StartupBox->addItem("False", 0);
    ui->StartupBox->addItem("True", 1);

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
    ui->ScreenNumberBox->setCurrentIndex(currScreenIndex);
    ui->AlwaysOnTopBox->setCurrentIndex(currOnTop);
    ui->ColorSchemeBox->setCurrentIndex(currColorScheme);
    ui->ProgramWidthBox->setCurrentIndex(currWidthIndex);
    ui->StartupBox->setCurrentIndex(currStartupIndex);
}

void Settings::onApplyButtonClicked() {
    applySettings();
}

void Settings::applySettings() {
    if (!mw) return;

    // Set all values before changes
    currSide = ui->ScreenSideBox->currentData().toInt();
    currScreenIndex = ui->ScreenNumberBox->currentData().toInt();
    currOnTop = ui->AlwaysOnTopBox->currentData().toInt();
    currColorScheme = ui->ColorSchemeBox->currentData().toInt();
    currWidthIndex = ui->ProgramWidthBox->currentData().toInt();
    currStartupIndex = ui->StartupBox->currentData().toInt();

    // Changes program width
    switch(currWidthIndex) {
    case 0:
        windowWidth = 360;
        break;
    case 1:
        windowWidth = 480;
        break;
    case 2:
        windowWidth = 600;
        break;
    case 3:
        windowWidth = 720;
        break;
    }
    this->resize(windowWidth, windowHeight);
    mw->resize(windowWidth, windowHeight);

    // Changes screen side and which screen
    moveApplication(currScreenIndex, currSide);

    // Changes always on top flags
    if (currOnTop == 0) {
        mw->setWindowFlag(Qt::WindowStaysOnTopHint, false);
        mw->show();
    } else {
        mw->setWindowFlag(Qt::WindowStaysOnTopHint, true);
        mw->show();
    }

    // Changes color scheme
    if (currColorScheme == 0) {
        qApp->setStyle("Fusion");
        qApp->setPalette(dark);
    } else {
        qApp->setStyle("Fusion");
        qApp->setPalette(light);
        currColorScheme = 1;
    }
    mw->setGlobalStyle();

    // Changes startup setting
    if (currStartupIndex == 1) {
        enableStartup();
        startupApp = true;
    } else {
        disableStartup();
        startupApp = false;
    }

    j["screenSide"] = ui->ScreenSideBox->currentData().toInt();
    j["screenNumber"] = ui->ScreenNumberBox->currentData().toInt();
    j["alwaysOnTop"] = ui->AlwaysOnTopBox->currentData().toInt();
    j["colorScheme"] = ui->ColorSchemeBox->currentData().toInt();
    j["programWidth"] = ui->ProgramWidthBox->currentData().toInt();
    j["startup"] = ui->StartupBox->currentData().toInt();

    QString appDir = QCoreApplication::applicationDirPath();
    std::ofstream file((appDir + "/settings.json").toStdString());
    file << j.dump(4);   // Pretty-printed JSON
}

// moves the application to the side of the screen
void Settings::moveApplication(int screenIndex, int side) {
    if (screenIndex < 0 || screenIndex >= screens.size()) return;

    currScreen = screens[screenIndex];
    geometry = currScreen->availableGeometry();

    screenWidth = geometry.width();
    screenHeight = geometry.height();
    windowHeight = screenHeight;

    this->resize(windowWidth, windowHeight);
    mw->resize(windowWidth, windowHeight);

    if (mw->firstLoad) {
        if (side == 0) mw->move(QPoint(geometry.x(), geometry.y()));
        else mw->move(QPoint(geometry.x() + (screenWidth - windowWidth), geometry.y()));
    } else {
        m_anim->stop();
        m_anim->setStartValue(QPoint(mw->pos()));

        if (side == 0) {
            m_anim->setEndValue(QPoint(geometry.x(), geometry.y()));
        } else {
            m_anim->setEndValue(QPoint(geometry.x() + (screenWidth - windowWidth), geometry.y()));
        }

        m_anim->start();
    }
    currSide = side;
}

// applies the color scheme to the windows
void Settings::setColorSchemes() {
    // Dark Mode
    // Main window + base background
    dark.setColor(QPalette::Window, QColor(42, 42, 42));
    dark.setColor(QPalette::Base,   QColor(42, 42, 42));

    // Text colors
    dark.setColor(QPalette::WindowText, QColor(255, 255, 255));
    dark.setColor(QPalette::Text,        QColor(255, 255, 255));
    dark.setColor(QPalette::ButtonText,  QColor(0, 0, 0));

    // Buttons
    dark.setColor(QPalette::Button, QColor(0, 156, 148)); // teal

    // Shadows + borders
    dark.setColor(QPalette::Shadow,   QColor(75, 75, 75));
    dark.setColor(QPalette::Dark,     QColor(75, 75, 75));
    dark.setColor(QPalette::Mid,      QColor(75, 75, 75));
    dark.setColor(QPalette::Midlight, QColor(85, 0, 255));
    dark.setColor(QPalette::Light,    QColor(75, 75, 75));

    // Highlight (optional)
    dark.setColor(QPalette::Highlight, QColor(85, 0, 255));

    // Light Mode
    // Main window + base background
    light.setColor(QPalette::Window, QColor(242, 242, 242)); // soft gray
    light.setColor(QPalette::Base,   QColor(255, 255, 255)); // pure white

    // Text colors
    light.setColor(QPalette::WindowText, QColor(0, 0, 0));
    light.setColor(QPalette::Text,        QColor(0, 0, 0));
    light.setColor(QPalette::ButtonText,  QColor(0, 0, 0));

    // Buttons
    light.setColor(QPalette::Button, QColor(0, 156, 148)); // teal

    // Shadows + borders
    light.setColor(QPalette::Shadow,   QColor(160, 160, 160));
    light.setColor(QPalette::Dark,     QColor(160, 160, 160));
    light.setColor(QPalette::Mid,      QColor(192, 192, 192));
    light.setColor(QPalette::Midlight, QColor(85, 0, 255));
    light.setColor(QPalette::Light,    QColor(255, 255, 255));

    // Highlight (same)
    light.setColor(QPalette::Highlight, QColor(85, 0, 255));
}

// allows the application to start on boot
void Settings::enableStartup() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey);

    if (result == ERROR_SUCCESS) {
        std::string appPath =
            "\"" + QDir::toNativeSeparators(QCoreApplication::applicationFilePath()).toStdString() + "\"";

        RegSetValueExA(
            hKey,
            "GameLauncher",
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(appPath.c_str()),
            static_cast<DWORD>(appPath.size() + 1));

        RegCloseKey(hKey);
    }
}

// disallows the application to start on boot
void Settings::disableStartup() {
    HKEY hKey;
    LONG result = RegOpenKeyExA(
        HKEY_CURRENT_USER,
        "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &hKey
        );

    if (result == ERROR_SUCCESS) {
        RegDeleteValueA(hKey, "GameLauncher");
        RegCloseKey(hKey);
    }
}

Settings::~Settings()
{
    delete ui;
}
